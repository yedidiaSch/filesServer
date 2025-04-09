#include "programKeeper.h"
#include <iostream>
#include <thread>

std::atomic<bool> ProgramKeeper::m_stopCondition{true};

void ProgramKeeper::waitForUserInput() 
{
    // Pause for a second as mentioned in the documentation
    std::this_thread::sleep_for(std::chrono::seconds(1));
    
    std::cout << "Press Enter to exit..." << std::endl;
    std::cin.get(); // Block until Enter key is pressed
}

void ProgramKeeper::waitForAtomicTrue() 
{
    while (m_stopCondition.load()) 
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

void ProgramKeeper::signalStop() 
{
    m_stopCondition.store(false);
}