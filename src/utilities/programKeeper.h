/**
 * @file programKeeper.h
 * @brief Provides utilities for managing program execution flow and termination.
 * 
 * This header defines the ProgramKeeper class which offers static methods to control
 * program flow through user input or atomic flag synchronization mechanisms.
 */

#ifndef PROGRAM_KEEPER_H
#define PROGRAM_KEEPER_H

#include <iostream>
#include <atomic>
#include <chrono>


/**
 * @class ProgramKeeper
 * @brief Manages program execution flow and provides mechanisms for graceful termination.
 * 
 * ProgramKeeper offers static methods to pause program execution either by waiting for user input
 * or by utilizing atomic flags for thread synchronization. This is useful for controlling the
 * main program loop and implementing graceful shutdown procedures.
 */
class ProgramKeeper 
{
public:
    /**
     * @brief Waits for the user to press Enter to continue execution.
     * 
     * This function pauses for a second, prompts the user to press Enter,
     * and then blocks until the Enter key is pressed.
     */
    static void waitForUserInput();

    /**
     * @brief Blocks execution until the stop condition is set to false.
     * 
     * This method polls the m_stopCondition atomic flag every 100 milliseconds
     * and continues to wait as long as the condition remains true.
     */
    static void waitForAtomicTrue();

    /**
     * @brief Signals that any threads waiting on the stop condition should exit their wait state.
     * 
     * Sets the m_stopCondition atomic flag to false, causing waitForAtomicTrue() to exit its loop.
     */
    static void signalStop();

private:
    /**
     * @brief Atomic flag that controls the wait condition.
     * 
     * When true, waitForAtomicTrue() will continue to wait.
     * When false, waitForAtomicTrue() will exit its waiting loop.
     * Initialized to true by default.
     */
    static std::atomic<bool> m_stopCondition;
};




#endif /* PROGRAM_KEEPER_H */


