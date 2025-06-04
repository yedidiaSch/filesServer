#ifndef SIGNALS_HANDLER_H
#define SIGNALS_HANDLER_H

#include <csignal>
#include <functional>
#include <map>
#include <atomic>
#include <vector>

class SignalsHandler {
public:
    // Singleton pattern
    static SignalsHandler& getInstance();

    // Register handler for a specific signal
    bool registerSignalHandler(int signalNumber, std::function<void(int)> handler);
    
    // Unregister handler for a specific signal
    bool unregisterSignalHandler(int signalNumber);
    
    // Setup default handlers for common signals
    void setupDefaultHandlers();
    
    // Block signals in the current thread
    static void blockSignals(const std::vector<int>& signals);
    
    // Unblock signals in the current thread
    static void unblockSignals(const std::vector<int>& signals);
    
    // Static signal handler function that will be registered with the OS
    static void signalHandlerFunc(int signal);
    
    // Check if termination was requested
    bool isTerminationRequested() const;

private:
    // Private constructor for singleton pattern
    SignalsHandler();
    
    // Delete copy constructor and assignment operator
    SignalsHandler(const SignalsHandler&) = delete;
    SignalsHandler& operator=(const SignalsHandler&) = delete;
    
    // Map to store signal handlers
    std::map<int, std::function<void(int)>> signalHandlers;
    
    // Flag to indicate termination request
    std::atomic<bool> terminationRequested;
    
    // Pointer to the singleton instance
    static SignalsHandler* instance;
    
    // Default signal handlers
    void defaultSigIntHandler(int signal);
    void defaultSigTermHandler(int signal);
    void defaultSigPipeHandler(int signal);
};

#endif // SIGNALS_HANDLER_H