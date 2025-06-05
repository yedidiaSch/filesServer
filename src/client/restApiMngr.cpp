#include "restApiMngr.h"
#include <curl/curl.h>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <thread>
#include <chrono>

RestApiMngr::RestApiMngr(const std::string& serverUrl)
    : m_serverUrl(serverUrl)
{
    itsQueueThread = new QueueThread();
}

RestApiMngr::~RestApiMngr()
{
    if (itsQueueThread)
    {
        delete itsQueueThread;
        itsQueueThread = nullptr;
    }
}

void RestApiMngr::update(void* params)
{
    filesMonitor::FileEvent* fileEvent = static_cast<filesMonitor::FileEvent*>(params);
    if (!fileEvent)
    {
        std::cerr << "FileEvent is null." << std::endl;
        return;
    }

    const std::string filename = fileEvent->filename;
    if (filename.empty())
    {
        std::cerr << "Filename is empty." << std::endl;
        return;
    }

    switch (fileEvent->eventType)
    {
        case filesMonitor::EventType::CREATED:
            handleFileCreation(filename);
            break;
        case filesMonitor::EventType::MODIFIED:
            handleFileModification(filename);
            break;
        case filesMonitor::EventType::DELETED:
            handleFileDeletion(filename);
            break;
        case filesMonitor::EventType::ATTRIB_CHANGED:
            handleFileModification(filename);
            break;
        default:
            std::cerr << "Unknown event type." << std::endl;
            break;
    }
}

size_t readCallback(void* ptr, size_t size, size_t nmemb, void* stream) {
    std::ifstream* file = static_cast<std::ifstream*>(stream);
    if (!file->read(static_cast<char*>(ptr), size * nmemb)) {
        return file->gcount(); // Return the number of bytes read
    }
    return size * nmemb;
}

bool RestApiMngr::sendFile(const std::string& localFilePath)
{
    CURL* curl = curl_easy_init();
    if (!curl)
    {
        std::cerr << "Failed to init curl" << std::endl;
        return false;
    }

    if (!std::filesystem::exists(localFilePath)) {
        std::cerr << "File does not exist: " << localFilePath << std::endl;
        curl_easy_cleanup(curl);
        return false;
    }

    std::filesystem::path p(localFilePath);
    curl_mime* mime = curl_mime_init(curl);
    curl_mimepart* part = curl_mime_addpart(mime);

    curl_mime_name(part, "file");
    curl_mime_filedata(part, localFilePath.c_str());

    curl_easy_setopt(curl, CURLOPT_URL, (m_serverUrl + "/api/files/upload").c_str());
    curl_easy_setopt(curl, CURLOPT_MIMEPOST, mime);

    CURLcode res = curl_easy_perform(curl);

    long response_code = 0;
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);
    std::cout << "HTTP Response Code: " << response_code << std::endl;

    if (res != CURLE_OK) {
        std::cerr << "Failed to send file: " << curl_easy_strerror(res) << std::endl;
    } else {
        std::cout << "File sent successfully: " << localFilePath << std::endl;
    }


    curl_mime_free(mime);
    curl_easy_cleanup(curl);
    return res == CURLE_OK;
}

bool RestApiMngr::deleteFile(const std::string& filename)
{
    CURL* curl = curl_easy_init();
    if (!curl)
    {
        std::cerr << "Failed to init curl" << std::endl;
        return false;
    }

    std::string url = m_serverUrl + "/api/files/" + std::filesystem::path(filename).filename().string();
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "DELETE");

    CURLcode res = curl_easy_perform(curl);
    curl_easy_cleanup(curl);
    return res == CURLE_OK;
}

bool RestApiMngr::shouldSendFile(const std::string& filename)
{
    auto now = std::chrono::steady_clock::now();
    auto it = recentUploads.find(filename);
    if (it == recentUploads.end())
    {
        return true;
    }
    auto diff = std::chrono::duration_cast<std::chrono::seconds>(now - it->second);
    return diff.count() > 2;
}

void RestApiMngr::handleFileCreation(const std::string& filename)
{
    auto task = [this, filename]() {
        std::this_thread::sleep_for(std::chrono::milliseconds(300));
        if (shouldSendFile(filename))
        {
            sendFile(filename);
            recentUploads[filename] = std::chrono::steady_clock::now();
            std::cout << "File sent: " << filename << std::endl;
        }
        else
        {
            std::cout << "Skipping duplicate send of: " << filename << std::endl;
        }
    };
    itsQueueThread->put(task);
}

void RestApiMngr::handleFileModification(const std::string& filename)
{
    auto task = [this, filename]() {
        std::this_thread::sleep_for(std::chrono::milliseconds(300));
        if (shouldSendFile(filename))
        {
            sendFile(filename);
            recentUploads[filename] = std::chrono::steady_clock::now();
            std::cout << "File sent: " << filename << std::endl;
        }
        else
        {
            std::cout << "Skipping duplicate send of: " << filename << std::endl;
        }
    };
    itsQueueThread->put(task);
}

void RestApiMngr::handleFileDeletion(const std::string& filename)
{
    auto task = [this, filename]() {
        deleteFile(filename);
    };
    itsQueueThread->put(task);
}

