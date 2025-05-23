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
#include <sys/stat.h>
#include <ctime>

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


    struct FileData {
        std::string filename;
        size_t fileSize;
        std::time_t modificationTime;
        std::vector<char> content;
        std::string checksum;  // e.g., MD5 or SHA-256
    };


    /**
     * @brief Constructs a dataFetch instance.
     * @param monitor Reference to the filesMonitor that provides file change notifications.
     */
     dataFetch();
    

     ~dataFetch();
    /**
     * @brief Callback function triggered when a notification is received.
     * @param params A pointer to the parameters associated with the notification.
     * 
     * This method is called when the observed subject sends a notification.
     * It processes the parameters and handles the appropriate action based on the notification type.
     * For file changes, it typically extracts the filename from the parameters and calls handleFileChange().
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
     

     /**
      * @brief Processes a file that has been changed.
      * @param filename The path of the file that was changed.
      * 
      * This method is called after a file change is detected to handle
      * the appropriate actions, such as reading the updated content and metadata,
      * calculating a new checksum, and possibly notifying other components
      * about the file change.
      */
     void handleFileChange(const std::string& filename);

     /**
      * @brief Reads the file content along with its metadata.
      * @param filename The path of the file to read.
      * @return The file data including metadata.
      */
     FileData readFileWithMetadata(const std::string& filename);

     /**
      * @brief Calculates the checksum of the given data.
      * @param data The data for which the checksum is to be calculated.
      * @return The checksum as a string.
      */
     std::string calculateChecksum(const std::vector<char>& data);
};

#endif // DATA_FETCH_H