#ifndef FILES_MONITOR_H
#define FILES_MONITOR_H

#include "../utilities/threadBase.h"
#include "../utilities/subject.h"
#include <atomic>
#include <string>
#include <unordered_map>
#include <mutex>
#include <functional>
#include <vector>

/**
 * @class filesMonitor
 * @brief Monitors a directory for file system events (creation, deletion, modification)
 *
 * The filesMonitor class uses inotify to detect file system events in a specified directory
 * and notifies registered observers when events matching configured filters occur.
 * It runs in a separate thread to avoid blocking the main application.
 * 
 * @note This class inherits from ThreadBase for thread management and subject for observer pattern
 * implementation.
 */
class filesMonitor : public ThreadBase,
                     public Subject    
{
public:
    /**
     * @enum EventType
     * @brief File system event types that can be detected
     */
    enum class EventType {
        CREATED,         ///< File created in the monitored directory
        MODIFIED,        ///< File content modified
        DELETED,         ///< File deleted from the monitored directory
        ATTRIB_CHANGED   ///< File attributes (permissions, ownership) changed
    };

    /**
     * @struct FileEvent
     * @brief Data structure containing information about a file system event
     */
    struct FileEvent {
        std::string filename;  ///< Name of the file that triggered the event
        EventType eventType;   ///< Type of event that occurred
    };

    /**
     * @brief Constructs a filesMonitor instance for the specified directory
     * @param dir_path Path to the directory to monitor
     * @throw std::invalid_argument if the directory path is empty
     */
    explicit filesMonitor(const std::string& dir_path);
    
    /**
     * @brief Destructor - stops monitoring and cleans up resources
     */
    ~filesMonitor();

    /**
     * @brief Start monitoring the directory in a separate thread
     * @return true if monitoring started successfully, false otherwise
     * @note If monitoring is already active, this function returns false
     */
    bool Start();
    
    /**
     * @brief Stop monitoring the directory and clean up resources
     * @note This method is thread-safe and can be called from any context
     */
    void Stop();
    
    /**
     * @brief Add a filter pattern to limit notifications to files matching the pattern
     * @param pattern String pattern to match against filenames
     * @note Patterns are substring matches (not regex or glob patterns)
     * @note If no filters are added, all files will generate notifications
     */
    void AddFilter(const std::string& pattern);
    
    /**
     * @brief Remove a previously added filter pattern
     * @param pattern The filter pattern to remove
     */
    void RemoveFilter(const std::string& pattern);

protected:
    /**
     * @brief Thread function that performs the actual file monitoring
     * @note Inherited from ThreadBase
     */
    void thread() override;

private:
    std::string m_dir_path;          ///< Path to the monitored directory
    std::atomic_bool m_run_flag;     ///< Flag controlling the monitoring thread
    int m_inotify_fd;                ///< File descriptor for the inotify instance
    int m_watch_fd;                  ///< Watch descriptor for the monitored directory
    
    mutable std::mutex m_filter_mutex;  ///< Mutex for thread-safe access to filters
    std::vector<std::string> m_filters; ///< List of filename patterns to filter events
    
    /**
     * @brief Check if a filename matches any of the configured filters
     * @param filename Name of the file to check against filters
     * @return true if filename matches a filter or if no filters are defined
     */
    bool matchesFilter(const std::string& filename) const;
    
    /**
     * @brief Initialize the inotify system and add a watch for the monitored directory
     * @return true on successful setup, false if an error occurred
     */
    bool setupInotify();
    
    /**
     * @brief Clean up inotify resources (watch and file descriptors)
     */
    void cleanupInotify();
    
    /**
     * @brief Process an inotify event and notify observers if applicable
     * @param event Pointer to the inotify_event structure to process
     */
    void processEvent(const struct inotify_event* event);
};

#endif /* FILES_MONITOR_H */