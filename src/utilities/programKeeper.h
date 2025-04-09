#ifndef PROGRAM_KEEPER_H
#define PROGRAM_KEEPER_H

#include <iostream>
#include <atomic>
#include <chrono>

#include "threadBase.h"

class ProgramKeeper 
{
public:
   
    
    // Function to wait for user input (Enter key)
    static void waitForUserInput() 
    {
        std::this_thread::sleep_for(std::chrono::seconds(1)); // Sleep for 1 second
        std::cout << "Press Enter to exit...\n";
        std::cin.get();
    }

    // Function to wait in this thread till atomic variable is set to true
    static void waitForAtomicTrue() 
    {
        while (m_stopCondition.load()) 
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    }

    // Function to signal that waiting should end
    static void signalStop() 
    {
        m_stopCondition.store(false);
    }


private:

    static std::atomic<bool> m_stopCondition; // Atomic variable to control the wait
};

std::atomic<bool> ProgramKeeper::m_stopCondition{true};

#endif /* PROGRAM_KEEPER_H */


