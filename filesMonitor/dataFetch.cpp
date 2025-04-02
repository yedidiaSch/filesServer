#include "dataFetch.h"
#include "filesMonitor.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>

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
    // Read the file content
    std::vector<char> content = readFileContent(filename);

    if (!content.empty()) 
    {
        printVectorAsHex(content);
        std::cout << "Content size: " << content.size() << " bytes" << std::endl;
    } 
    
    else 
    {
        std::cout << "Failed to read content of " << filename << std::endl;
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


