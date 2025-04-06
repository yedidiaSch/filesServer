#ifndef TIMER_FD_H
#define TIMER_FD_H

#include <chrono>
#include <functional>
#include <atomic>
#include <mutex>
#include "threadBase.h"

static const int SEC_TO_MILI    = 1000;      // Conversion factor from seconds to milliseconds
static const int MILI_TO_NANO   = 1000000;   // Conversion factor from milliseconds to nanoseconds

/**
 * @class TimerFd
 * @brief A class that provides timer functionality using file descriptors.
 * 
 * TimerFd is derived from ThreadBase and provides methods to set, start, and stop a timer.
 * Derived classes must implement the onTimeout() method to define the timeout action.
 */
class TimerFd : public ThreadBase
{

public:

    /**
     * @brief Constructor for TimerFd.
     * Initializes the timer file descriptor.
     */
    TimerFd                     ();

    /**
     * @brief Destructor for TimerFd.
     * Cleans up the timer file descriptor.
     */
    ~TimerFd                    ();

    /**
     * @brief Sets the timer with a delay and an optional interval.
     * 
     * @param delay The initial delay before the timer expires.
     * @param interval The interval for periodic timer expiration. Default is 0 (one-shot timer).
     */
    void SetTimer               ( std::chrono::milliseconds delay,
                                  std::chrono::milliseconds interval = std::chrono::milliseconds(0));

    /**
     * @brief Gets the remaining time on the timer.
     * 
     * @return The remaining time in milliseconds.
     */
    long GetTimer               ();

    /**
     * @brief Starts the timer.
     */
    void Start                  ();

    /**
     * @brief Stops the timer.
     */
    void Stop                   ();

protected:

    /**
     * @brief The thread function that handles the timer expiration.
     * This function is called internally by the ThreadBase class.
     */
    void thread                 () override;

    /**
     * @brief Pure virtual function to be implemented by derived classes.
     * This function is called when the timer expires.
     */
    virtual void onTimeout() = 0;

private:

    int m_timer_fd; ///< The file descriptor for the timer.

    std::mutex m_mutex; ///< Mutex to protect access to the timer.
    // Remove the shadowed m_running declaration since it's already in ThreadBase
};


#endif // TIMER_FD_H

