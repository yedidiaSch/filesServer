/**
 * @file dataFetch.h
 * @brief Header file for the dataFetch class that monitors file content changes.
 * 
 * This file contains the declaration of the dataFetch class which is responsible
 * for detecting changes in files and reading their contents when changes occur.
 * It interacts with the filesMonitor to handle file change events.
 */

#ifndef DATA_FETCH_H
#define DATA_FETCH_H

#include "../utilities/IObserver.h"
#include <string>
#include <fstream>
#include <iostream>
#include <vector>

/**
 * @class dataFetch
 * @brief Observes file content changes and responds to file modification events.
 * 
 * The dataFetch class provides functionality to read content when changes are detected.
 * It works in conjunction with the filesMonitor class which provides the file change notifications.
 */
class dataFetch : public IObserver
{
public:

    /**
     * @brief Constructs a dataFetch instance.
     * @param monitor Reference to the filesMonitor that provides file change notifications.
     */
     dataFetch();
    

     ~dataFetch();
    /**
     * @brief Callback function triggered when a file changes.
     * @param filename The path of the file that was changed.
     * 
     * This method is called when the monitored file system detects changes to a file.
     * It reads the current content of the modified file.
     */
     void update(void* params) override;

private:
    /**
     * @brief Reads the current content of a file.
     * @param filename The path of the file to read.
     * @return The content of the file as vector<char>.
     * 
     * This method opens the specified file and reads its entire content into a string.
     * If the file cannot be opened, it may log an error or throw an exception.
     */
     std::vector<char> readFileContent(const std::string& filename);

     void handleFileChange(const std::string& filename);
    

};

#endif // DATA_FETCH_H