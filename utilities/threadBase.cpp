#include "threadBase.h"



ThreadBase::ThreadBase() : m_running(false)
{
}

ThreadBase::~ThreadBase()
{
    stop();  // Ensure the thread stops before destruction
}

void ThreadBase::start()
{
    std::lock_guard<std::mutex> lock(m_mutex);

    if (!m_running)
    {
        m_running = true;
        m_thread = std::thread(&ThreadBase::thread, this);
    }
}

void ThreadBase::stop()
{
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_running = false;
    }

    if (m_thread.joinable())
    {
        m_thread.join();  // Wait for the thread to finish
    }
}