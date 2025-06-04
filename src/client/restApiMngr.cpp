#include "restApiMngr.h"
#include <curl/curl.h>
#include <filesystem>
#include <fstream>
#include <iostream>

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

bool RestApiMngr::sendFile(const std::string& localFilePath)
{
    CURL* curl = curl_easy_init();
    if (!curl)
    {
        std::cerr << "Failed to init curl" << std::endl;
        return false;
    }

    std::ifstream file(localFilePath, std::ios::binary);
    if (!file)
    {
        std::cerr << "Failed to open file: " << localFilePath << std::endl;
        curl_easy_cleanup(curl);
        return false;
    }

    std::filesystem::path p(localFilePath);
    curl_easy_setopt(curl, CURLOPT_URL, (m_serverUrl + "/upload").c_str());
    curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);
    curl_easy_setopt(curl, CURLOPT_READFUNCTION, nullptr);
    curl_easy_setopt(curl, CURLOPT_READDATA, &file);
    curl_easy_setopt(curl, CURLOPT_INFILESIZE_LARGE, static_cast<curl_off_t>(std::filesystem::file_size(p)));

    CURLcode res = curl_easy_perform(curl);
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

    std::string url = m_serverUrl + "/file/" + std::filesystem::path(filename).filename().string();
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "DELETE");

    CURLcode res = curl_easy_perform(curl);
    curl_easy_cleanup(curl);
    return res == CURLE_OK;
}

void RestApiMngr::handleFileCreation(const std::string& filename)
{
    auto task = [this, filename]() {
        sendFile(filename);
    };
    itsQueueThread->put(task);
}

void RestApiMngr::handleFileModification(const std::string& filename)
{
    auto task = [this, filename]() {
        sendFile(filename);
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

