#ifndef NETWORK_MNGR_H
#define NETWORK_MNGR_H

#include <string>
#include <libssh/libssh.h>
#include <libssh/sftp.h>
#include "filesMonitor.h"  
#include "../utilities/IObserver.h"
#include "../utilities/QueueThread.h"
#include "../utilities/TimerFd.h"

/**
 * @class sftpMngr
 * @brief Manages SFTP connections to transfer ELF files to remote server
 *
 * This class handles establishing SSH connections, SFTP sessions,
 * and file transfers to a remote server securely. It automatically 
 * disconnects after a period of inactivity to conserve resources.
 */
class sftpMngr : public IObserver, public TimerFd
{
public:
    /**
     * @brief Constructor with server connection parameters
     * 
     * @param serverAddress IP address or hostname of the server
     * @param username SSH username for authentication
     * @param password SSH password for authentication
     * @param port SSH port number (default: 22)
     * @param remotePath Base directory on the remote server for file uploads (default: "/")
     * @param connectionTimeout Timeout in milliseconds after which an idle connection is closed (default: 5 minutes)
     */
    sftpMngr(const std::string& serverAddress, const std::string& username, 
                const std::string& password, int port = 22, 
                const std::string& remotePath = "/",
                std::chrono::milliseconds connectionTimeout = std::chrono::minutes(5));
    
    /**
     * @brief Destructor that ensures proper disconnection
     */
    ~sftpMngr();
    
    /**
     * @brief Establish connection to the SFTP server
     * @return true if connection successful, false otherwise
     */
    bool connect();
    
    /**
     * @brief Close connections and release resources
     */
    void disconnect();
    
    /**
     * @brief Transfer a file to the remote server via SFTP
     * 
     * @param localFilePath Path to the local file to send
     * @param remoteFilePath Destination path on the server
     * @return true if transfer successful, false otherwise
     */
    bool sendFile(const std::string& localFilePath, const std::string& remoteFilePath);

    /**
     * @brief Callback function triggered when a file change notification is received
     * @param params A pointer to the notification parameters (typically a FileChangeInfo object)
     * 
     * This method is called when the observed subject (FilesMonitor) sends a notification.
     * It processes the notification parameters and determines the type of file change event
     * (creation, modification, or deletion), then delegates to the appropriate handler method.
     */
     void update(void* params) override;

    /**
     * @brief Reset the connection timeout timer after activity
     * 
     * Resets the inactivity timer to keep the connection alive after recent activity.
     */
    void resetConnectionTimer();

protected:
    /**
     * @brief Handler called when the connection timeout timer expires
     * 
     * Disconnects the SFTP session and SSH connection after the inactivity period.
     */
    void onTimeout() override;

private:
    /**
     * @brief Initialize SFTP subsystem on established SSH connection
     * @return true if initialization successful, false otherwise
     */
    bool initializeSFTP();

    /**
     * @brief Handle file creation events
     * @param filename The name of the created file
     * 
     * Processes newly created files and sends them to the remote server.
     */
    void handleFileCreation(const std::string& filename);
    
    /**
     * @brief Handle file modification events
     * @param filename The name of the modified file
     * 
     * Processes modified files and sends the updated version to the remote server.
     */
    void handleFileModification(const std::string& filename);
    
    /**
     * @brief Handle file deletion events
     * @param filename The name of the deleted file
     * 
     * Handles the deletion of files by removing the corresponding file on the remote server.
     */
    void handleFileDeletion(const std::string& filename);
       
    // Server connection information
    std::string     m_serverAddress;    ///< IP address or hostname of the SFTP server
    std::string     m_username;         ///< SSH username for authentication
    std::string     m_password;         ///< SSH password for authentication
    int             m_port;             ///< SSH port number
    std::string     m_remotePath;       ///< Base path on remote server for file operations
    
    ssh_session     m_sshSession;       ///< SSH session handle
    sftp_session    m_sftpSession;      ///< SFTP session handle

    QueueThread *   itsQueueThread;     ///< Thread for processing file transfer tasks

    /**
     * @brief The timeout duration for idle connections
     */
    std::chrono::milliseconds m_connectionTimeout;

    /**
     * @brief Flag to track if a connection is active
     */
    bool m_isConnected;
};

#endif // NETWORK_MNGR_H
