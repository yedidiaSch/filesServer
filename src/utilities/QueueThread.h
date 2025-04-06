#ifndef QUEUE_THREAD_H
#define QUEUE_THREAD_H

#include <queue>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <atomic>
#include "threadBase.h"

/**
 * @class QueueThread
 * @brief A class that provides a thread with a task queue.
 * 
 * QueueThread is derived from ThreadBase and provides methods to add tasks to a queue.
 * The thread processes tasks from the queue.
 */
class QueueThread : public ThreadBase 
{

public:

    /**
     * @brief Constructor for QueueThread.
     * Initializes the task queue and condition variable.
     */
    QueueThread             ();

    /**
     * @brief Destructor for QueueThread.
     * Ensures the thread is stopped and the queue is cleared.
     */
    ~QueueThread            ();

    /**
     * @brief Adds a task to the queue.
     * 
     * @param task The task to be added to the queue.
     */
    void put                (std::function<void()> task);

protected:

    /**
     * @brief The thread function that processes tasks from the queue.
     */
    void thread             () override;

private:

    std::queue<std::function<void()>>   taskQueue;          // Queue to store tasks

    std::mutex                          queueMutex;         // Mutex to protect access to the queue

    std::condition_variable             queueCondition;     // Condition variable to notify the thread of new tasks
};

#endif // QUEUE_THREAD_H
