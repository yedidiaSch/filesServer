/**
 * @file restApiMngr.h
 * @brief REST client that uploads and deletes files on a remote server.
 */
#ifndef REST_API_MNGR_H
#define REST_API_MNGR_H

#include <string>
#include <unordered_map>
#include <chrono>
#include "filesMonitor.h"
#include "../utilities/IObserver.h"
#include "../utilities/QueueThread.h"

/**
 * @class RestApiMngr
 * @brief Handles file transfer operations using a REST API.
 *
 * This class observes file events from filesMonitor and sends the
 * appropriate REST requests to the remote server. File operations are
 * performed asynchronously using QueueThread from the utilities module.
 */
class RestApiMngr : public IObserver
{
public:
    /**
     * @brief Construct a RestApiMngr.
     * @param serverUrl Base URL of the REST server (e.g. "http://127.0.0.1:8080")
     */
    explicit RestApiMngr(const std::string& serverUrl);

    /**
     * @brief Destructor cleans up resources.
     */
    ~RestApiMngr();

    /**
     * @brief Callback for file events from filesMonitor.
     *
     * The parameter must be a pointer to filesMonitor::FileEvent. The
     * event is queued for processing on a separate thread.
     */
    void update(void* params) override;

private:
    /**
     * @brief Upload a file to the server using HTTP POST.
     * @param localFilePath Path to the local file on disk.
     * @return true on success, false otherwise.
     */
    bool sendFile(const std::string& localFilePath);

    /**
     * @brief Issue an HTTP DELETE request for a remote file.
     * @param filename Name of the file to remove on the server.
     * @return true on success, false otherwise.
     */
    bool deleteFile(const std::string& filename);

    /**
     * @brief Process a file creation event.
     * @param filename Path to the newly created file.
     */
    void handleFileCreation(const std::string& filename);

    /**
     * @brief Process a file modification event.
     * @param filename Path to the modified file.
     */
    void handleFileModification(const std::string& filename);

    /**
     * @brief Process a file deletion event.
     * @param filename Path to the deleted file.
     */
    void handleFileDeletion(const std::string& filename);

    /**
     * @brief Determine if a file should be sent based on recent uploads.
     * @param filename Name of the file to check.
     * @return true if the file was not sent in the last 2 seconds.
     */
    bool shouldSendFile(const std::string& filename);

    /** Base REST server URL */
    std::string    m_serverUrl;

    /** Queue thread used to run HTTP requests asynchronously */
    QueueThread*   itsQueueThread;

    /** Map tracking the last upload time for each file */
    std::unordered_map<std::string, std::chrono::steady_clock::time_point> recentUploads;
};

#endif // REST_API_MNGR_H
