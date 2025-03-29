#ifndef THREAD_BASE_H
#define THREAD_BASE_H

#include <iostream>
#include <thread>
#include <atomic>
#include <chrono>
#include <mutex>

/**
 * @class ThreadBase
 * @brief A base class for creating threads.
 * 
 * ThreadBase provides basic functionality to start and stop a thread.
 * Derived classes must implement the thread() method.
 */
class ThreadBase
{
public:

    /**
     * @brief Constructor for ThreadBase.
     * Initializes the thread and running flag.
     */
    ThreadBase              ();

    /**
     * @brief Destructor for ThreadBase.
     * Ensures the thread is stopped before destruction.
     */
    virtual ~ThreadBase     ();

    /**
     * @brief Starts the thread.
     */
    void start              ();

    /**
     * @brief Stops the thread.
     */
    void stop               ();

protected:

    std::thread         m_thread;           // The worker thread

    std::atomic<bool>   m_running;          // Flag to control thread execution

    std::mutex          m_mutex;            // Mutex for thread safety

    /**
     * @brief The thread function to be implemented by derived classes.
     */
    virtual void thread     () = 0;         // Must be implemented by derived classes
};

#endif /* THREAD_BASE_H */

