#include "filesMonitor.h"

#include <iostream>
#include <string>
#include <vector>
#include <sys/inotify.h>
#include <unistd.h>
#include <poll.h>
#include <cstring>
#include <algorithm>
#include <fcntl.h>
#include <errno.h>

// Buffer for reading inotify events
static const size_t EVENT_BUF_LEN = 4096;

filesMonitor::filesMonitor(const std::string& dir_path) 
    : m_dir_path(dir_path),
      m_run_flag(false),
      m_inotify_fd(-1),
      m_watch_fd(-1)
{
    // Validate directory path
    if (m_dir_path.empty()) {
        throw std::invalid_argument("Directory path cannot be empty");
    }
}

filesMonitor::~filesMonitor()
{
    Stop();
    cleanupInotify();
}

bool filesMonitor::Start()
{
    if (m_run_flag.load()) {
        return false; // Already running
    }
    
    if (!setupInotify()) {
        return false;
    }
    
    m_run_flag.store(true);
    
    // Create and start the monitoring thread
    start();
    
    return true;
}

void filesMonitor::Stop()
{
    stop();  // This already sets m_running to false and joins the thread
    cleanupInotify();
}

void filesMonitor::AddFilter(const std::string& pattern)
{
    std::lock_guard<std::mutex> lock(m_filter_mutex);
    if (std::find(m_filters.begin(), m_filters.end(), pattern) == m_filters.end()) {
        m_filters.push_back(pattern);
    }
}

void filesMonitor::RemoveFilter(const std::string& pattern)
{
    std::lock_guard<std::mutex> lock(m_filter_mutex);
    m_filters.erase(std::remove(m_filters.begin(), m_filters.end(), pattern), m_filters.end());
}

bool filesMonitor::matchesFilter(const std::string& filename) const
{
    std::lock_guard<std::mutex> lock(m_filter_mutex);
    
    // If no filters are defined, accept all files
    if (m_filters.empty()) {
        return true;
    }
    
    // Check if the filename matches any of the filters
    for (const auto& filter : m_filters) {
        if (filename.find(filter) != std::string::npos) {
            return true;
        }
    }
    
    return false;
}

bool filesMonitor::setupInotify()
{
    // Initialize inotify
    m_inotify_fd = inotify_init1(IN_NONBLOCK);
    if (m_inotify_fd == -1) {
        std::cerr << "Failed to initialize inotify: " << strerror(errno) << std::endl;
        return false;
    }

    // Add watch for the directory
    uint32_t mask = IN_CREATE | IN_DELETE | IN_MODIFY | IN_ATTRIB;
    m_watch_fd = inotify_add_watch(m_inotify_fd, m_dir_path.c_str(), mask);
    if (m_watch_fd == -1) {
        std::cerr << "Failed to add watch on directory " << m_dir_path << ": " 
                  << strerror(errno) << std::endl;
        close(m_inotify_fd);
        m_inotify_fd = -1;
        return false;
    }

    return true;
}

void filesMonitor::cleanupInotify()
{
    if (m_watch_fd != -1) {
        inotify_rm_watch(m_inotify_fd, m_watch_fd);
        m_watch_fd = -1;
    }
    
    if (m_inotify_fd != -1) {
        close(m_inotify_fd);
        m_inotify_fd = -1;
    }
}

void filesMonitor::processEvent(const struct inotify_event* event)
{
    std::cout << "Event received: mask=" << event->mask << std::endl;

    // Skip if no name is provided or it's a directory
    if (!event->len || (event->mask & IN_ISDIR)) {
        return;
    }
    
    std::string filename(event->name);
    
    // Check if file matches filters
    if (!matchesFilter(filename)) {
        return;
    }
    
    // Create appropriate event
    FileEvent fileEvent;
    fileEvent.filename = filename;
    
    // Create a pointer to heap-allocated string that will be passed to observers
    std::string* filenamePtr = new std::string(filename);
    
    if (event->mask & IN_CREATE) {
        fileEvent.eventType = EventType::CREATED;
        notify(filenamePtr);  // Pass pointer to the string
    }
    else if (event->mask & IN_DELETE) {
        fileEvent.eventType = EventType::DELETED;
        notify(filenamePtr);
    }
    else if (event->mask & IN_MODIFY) {
        fileEvent.eventType = EventType::MODIFIED;
        notify(filenamePtr);
    }
    else if (event->mask & IN_ATTRIB) {
        fileEvent.eventType = EventType::ATTRIB_CHANGED;
        notify(filenamePtr);
    }
}

void filesMonitor::thread()
{
    char buffer[EVENT_BUF_LEN];
    
    // Set up polling
    struct pollfd pfd = {
        .fd = m_inotify_fd,
        .events = POLLIN,
        .revents = 0
    };

    while (m_run_flag.load()) {
        // Wait for events with timeout
        int poll_ret = poll(&pfd, 1, 500);  // 500ms timeout
        
        if (poll_ret < 0) {
            if (errno == EINTR) {
                continue;  // Interrupted, just retry
            }
            std::cerr << "Poll error: " << strerror(errno) << std::endl;
            break;
        }
        
        // Timeout occurred, just loop again
        if (poll_ret == 0) {
            continue;
        }
        
        // Check if we have inotify events
        if (!(pfd.revents & POLLIN)) {
            continue;
        }
        
        // Read events
        ssize_t length = read(m_inotify_fd, buffer, EVENT_BUF_LEN);
        
        if (length < 0) {
            if (errno == EAGAIN) {
                continue;  // No data available right now
            }
            std::cerr << "Error reading inotify events: " << strerror(errno) << std::endl;
            break;
        }
        
        // Process events
        ssize_t i = 0;
        while (i < length) {
            struct inotify_event* event = reinterpret_cast<struct inotify_event*>(&buffer[i]);
            processEvent(event);
            i += sizeof(struct inotify_event) + event->len;
        }
    }
}