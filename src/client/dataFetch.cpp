#include "dataFetch.h"
#include "filesMonitor.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>

#include <openssl/sha.h>

void printVectorAsHex(const std::vector<char>& data) {
    for (unsigned char c : data) {  // Treat as unsigned to avoid negative values
        std::cout << std::hex << std::setw(2) << std::setfill('0') << (int)c << " ";
    }
    std::cout << std::endl;
}



dataFetch::dataFetch() 
{
}

dataFetch::~dataFetch() 
{
}

void dataFetch::update(void* params)
{
    filesMonitor::FileEvent* fileEvent = static_cast<filesMonitor::FileEvent*>(params);
    if (fileEvent == nullptr) {
        std::cerr << "FileEvent is null." << std::endl;
        return;
    }

    std::string filename  = fileEvent->filename;

    if (filename.empty()) {
        std::cerr << "Filename is empty." << std::endl;
        return;
    }

    switch (fileEvent->eventType) 
    {
        case filesMonitor::EventType::CREATED:
            std::cout << "File created: " << filename << std::endl;
            handleFileChange(filename);
            break;

        case filesMonitor::EventType::MODIFIED:
            std::cout << "File modified: " << filename << std::endl;
            handleFileChange(filename);
            break;

        case filesMonitor::EventType::DELETED:
            std::cout << "File deleted: " << filename << std::endl;
            return; // No need to read content for deleted files

        case filesMonitor::EventType::ATTRIB_CHANGED:
            std::cout << "File attributes changed: " << filename << std::endl;
            handleFileChange(filename);
            break;

        default:
            std::cerr << "Unknown event type." << std::endl;
            return;
    }
}

void dataFetch::handleFileChange(const std::string& filename)
{
    try {
        // Read the file content and metadata
        FileData fileData = readFileWithMetadata(filename);
        
        // Print metadata
        std::cout << "File: " << fileData.filename << std::endl;
        std::cout << "Size: " << fileData.fileSize << " bytes" << std::endl;
        std::cout << "Modified: " << std::ctime(&fileData.modificationTime);
        std::cout << "Checksum: " << fileData.checksum << std::endl;
        
        // Print content as hex if needed
        std::cout << "Content preview: ";
        if (!fileData.content.empty()) {
            // Print first few bytes as hex
            printVectorAsHex(std::vector<char>(
                fileData.content.begin(), 
                fileData.content.begin() + std::min(fileData.content.size(), static_cast<size_t>(20))
            ));
            std::cout << "Content size: " << fileData.content.size() << " bytes" << std::endl;
        } else {
            std::cout << "Empty file" << std::endl;
        }
    } catch (const std::exception& e) {
        std::cerr << "Error processing file " << filename << ": " << e.what() << std::endl;
    }
}

std::vector<char> dataFetch::readFileContent(const std::string& filename) 
{
    std::ifstream file(filename, std::ios::binary);
    if (!file) {
        throw std::runtime_error("Failed to open file: " + filename);
    }

    // Get file size
    file.seekg(0, std::ios::end);
    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);

    // Read file into vector
    std::vector<char> buffer(size);
    if (!file.read(buffer.data(), size)) {
        throw std::runtime_error("Failed to read file: " + filename);
    }

    return buffer;
}

// Modified function to gather file metadata along with content
dataFetch::FileData dataFetch::readFileWithMetadata(const std::string& filename) 
{
    FileData fileData;
    fileData.filename = filename;
    
    // Get file info
    struct stat fileInfo;
    if (stat(filename.c_str(), &fileInfo) != 0) {
        throw std::runtime_error("Failed to get file info: " + filename);
    }
    
    fileData.fileSize = fileInfo.st_size;
    fileData.modificationTime = fileInfo.st_mtime;
    
    // Read content
    std::ifstream file(filename, std::ios::binary);
    if (!file) {
        throw std::runtime_error("Failed to open file: " + filename);
    }
    
    // Get file size
    file.seekg(0, std::ios::end);
    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);
    
    // Read file into vector
    fileData.content.resize(size);
    if (!file.read(fileData.content.data(), size)) {
        throw std::runtime_error("Failed to read file: " + filename);
    }
    
    // Calculate checksum (implement calculateChecksum separately)
    fileData.checksum = calculateChecksum(fileData.content);
    
    return fileData;
}

std::string dataFetch::calculateChecksum(const std::vector<char>& data) {
    // Use SHA-256 algorithm from OpenSSL
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256_CTX sha256;
    SHA256_Init(&sha256);
    SHA256_Update(&sha256, data.data(), data.size());
    SHA256_Final(hash, &sha256);
    
    // Convert hash to hex string
    std::stringstream ss;
    for(int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
        ss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(hash[i]);
    }
    
    return ss.str();
}


