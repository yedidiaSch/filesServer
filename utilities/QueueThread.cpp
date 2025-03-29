#include "QueueThread.h"
#include <iostream>



QueueThread::QueueThread() 
{
    start();
}

QueueThread::~QueueThread() 
{
    stop();
}

void QueueThread::put(std::function<void()> task) 
{
    {
        std::unique_lock<std::mutex> lock(queueMutex);
        taskQueue.push(task);
    }

    queueCondition.notify_one();
}

void QueueThread::thread() 
{
    while (m_running) 
    {
        std::function<void()> task;
        {

            std::unique_lock<std::mutex> lock(queueMutex);
            queueCondition.wait(lock, [this] { return !m_running || !taskQueue.empty(); });
            if (!m_running && taskQueue.empty()) 
            {
                return;
            }

            task = std::move(taskQueue.front());
            taskQueue.pop();

        }

        task();
    }
}