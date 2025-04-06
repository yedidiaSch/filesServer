#ifndef NETWORK_MNGR_H
#define NETWORK_MNGR_H

#include <string>
#include <libssh/libssh.h>
#include <libssh/sftp.h>
#include "filesMonitor.h"  
#include "../utilities/IObserver.h"

/**
 * @class sftpMngr
 * @brief Manages SFTP connections to transfer ELF files to remote server
 *
 * This class handles establishing SSH connections, SFTP sessions,
 * and file transfers to a remote server securely.
 */
class sftpMngr : public IObserver
{
public:
    /**
     * @brief Constructor with server connection parameters
     * 
     * @param serverAddress IP address or hostname of the server
     * @param username SSH username for authentication
     * @param password SSH password for authentication
     * @param port SSH port number (default: 22)
     */
    sftpMngr(const std::string& serverAddress, const std::string& username, 
                const std::string& password, int port = 22);
    
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
     * @brief Initialize SFTP subsystem on established SSH connection
     * @return true if initialization successful, false otherwise
     */
    bool initializeSFTP();


    void handleFileCreation         (const std::string& filename);
    void handleFileModification     (const std::string& filename);
    void handleFileDeletion         (const std::string& filename);
    
    std::string m_serverAddress;
    std::string m_username;
    std::string m_password;
    int m_port;
    
    ssh_session m_sshSession;
    sftp_session m_sftpSession;
};

#endif // NETWORK_MNGR_H
