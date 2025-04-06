#include "sftpMngr.h"
#include <iostream>
#include <fstream>
#include <filesystem>
#include <fcntl.h>

// Constructor
sftpMngr::sftpMngr(   const std::string& serverAddress, const std::string& username, 
                            const std::string& password, int port)
                            : m_serverAddress(serverAddress), m_username(username), 
                            m_password(password), m_port(port), m_sshSession(nullptr),
                            m_sftpSession(nullptr) 
{
}

// Destructor
sftpMngr::~sftpMngr() 
{
    disconnect();
}

// Connect to SSH server
bool sftpMngr::connect() 
{
    // Create new SSH session
    m_sshSession = ssh_new();
    if (m_sshSession == nullptr) {
        std::cerr << "Failed to create SSH session" << std::endl;
        return false;
    }

    // Set SSH session options
    ssh_options_set(m_sshSession, SSH_OPTIONS_HOST, m_serverAddress.c_str());
    ssh_options_set(m_sshSession, SSH_OPTIONS_USER, m_username.c_str());
    ssh_options_set(m_sshSession, SSH_OPTIONS_PORT, &m_port);

    // Connect to server
    int rc = ssh_connect(m_sshSession);
    if (rc != SSH_OK) {
        std::cerr << "Error connecting to " << m_serverAddress << ": " 
                  << ssh_get_error(m_sshSession) << std::endl;
        ssh_free(m_sshSession);
        m_sshSession = nullptr;
        return false;
    }

    // Verify server identity
    if (ssh_session_is_known_server(m_sshSession) != SSH_KNOWN_HOSTS_OK) 
    {
        ssh_session_update_known_hosts(m_sshSession);  // Add to known hosts
    }

    // Authenticate with password
    rc = ssh_userauth_password(m_sshSession, nullptr, m_password.c_str());
    if (rc != SSH_AUTH_SUCCESS) {
        std::cerr << "Authentication error: " << ssh_get_error(m_sshSession) << std::endl;
        ssh_disconnect(m_sshSession);
        ssh_free(m_sshSession);
        m_sshSession = nullptr;
        return false;
    }

    // Initialize SFTP after successful SSH connection
    return initializeSFTP();
}

// Disconnect SSH and SFTP sessions
void sftpMngr::disconnect() 
{
    // Clean up SFTP session
    if (m_sftpSession != nullptr) 
    {
        sftp_free(m_sftpSession);
        m_sftpSession = nullptr;
    }

    // Clean up SSH session
    if (m_sshSession != nullptr) 
    {
        ssh_disconnect(m_sshSession);
        ssh_free(m_sshSession);
        m_sshSession = nullptr;
    }
}

// Initialize SFTP subsystem
bool sftpMngr::initializeSFTP() 
{
    if (m_sshSession == nullptr) {
        std::cerr << "SSH session not established" << std::endl;
        return false;
    }

    // Create SFTP session
    m_sftpSession = sftp_new(m_sshSession);
    if (m_sftpSession == nullptr) {
        std::cerr << "Failed to create SFTP session: " << ssh_get_error(m_sshSession) << std::endl;
        return false;
    }

    // Initialize SFTP session
    int rc = sftp_init(m_sftpSession);
    if (rc != SSH_OK) {
        std::cerr << "Failed to initialize SFTP session: " 
                  << sftp_get_error(m_sftpSession) << std::endl;
        sftp_free(m_sftpSession);
        m_sftpSession = nullptr;
        return false;
    }

    return true;
}

// Send a file via SFTP
bool sftpMngr::sendFile(const std::string& localFilePath, const std::string& remoteFilePath) 
{
    if (m_sftpSession == nullptr) {
        std::cerr << "SFTP session not established" << std::endl;
        return false;
    }

    // Open local file for reading
    std::ifstream localFile(localFilePath, std::ios::binary);
    if (!localFile.is_open()) {
        std::cerr << "Failed to open local file: " << localFilePath << std::endl;
        return false;
    }

    // Get file size
    std::filesystem::path path(localFilePath);
    size_t fileSize = std::filesystem::file_size(path);

    // Create remote file
    sftp_file remoteFile = sftp_open(m_sftpSession, remoteFilePath.c_str(),
                                    O_WRONLY | O_CREAT | O_TRUNC, S_IRWXU);
    if (remoteFile == nullptr) {
        std::cerr << "Failed to create remote file: " 
                  << sftp_get_error(m_sftpSession) << std::endl;
        localFile.close();
        return false;
    }

    // Transfer file data
    bool success = true;
    const size_t bufferSize = 4096;
    char buffer[bufferSize];
    
    while (localFile.good() && !localFile.eof()) {
        localFile.read(buffer, bufferSize);
        std::streamsize bytesRead = localFile.gcount();
        
        if (bytesRead > 0) {
            ssize_t bytesWritten = sftp_write(remoteFile, buffer, bytesRead);
            if (bytesWritten != bytesRead) {
                std::cerr << "Failed to write data to remote file: " 
                          << sftp_get_error(m_sftpSession) << std::endl;
                success = false;
                break;
            }
        }
    }

    // Clean up
    sftp_close(remoteFile);
    localFile.close();
    
    return success;
}


void sftpMngr::update(void* params)
{
    
    // Cast params to the appropriate type
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
            handleFileCreation(filename);
            break;

        case filesMonitor::EventType::MODIFIED:
            std::cout << "File modified: " << filename << std::endl;
            break;

        case filesMonitor::EventType::DELETED:
            std::cout << "File deleted: " << filename << std::endl;
            return; // No need to read content for deleted files

        case filesMonitor::EventType::ATTRIB_CHANGED:
            std::cout << "File attributes changed: " << filename << std::endl;
            break;

        default:
            std::cerr << "Unknown event type." << std::endl;
            return;
    }


   
}

void sftpMngr::handleFileCreation(const std::string& filename)
{
    // Handle file change
    if (!filename.empty()) 
    {
        std::cout << "File created: " << filename << std::endl;

        // Send the file to the server
        if (!sendFile(filename, "/remote/path/" + filename)) {
            std::cerr << "Failed to send file: " << filename << std::endl;
        }
    } 
    else 
    {
        std::cerr << "Invalid parameters received" << std::endl;
    }
}