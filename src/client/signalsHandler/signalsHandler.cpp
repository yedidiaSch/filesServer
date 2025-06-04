#include "signalsHandler.h"
#include <csignal>
#include <functional>
#include <iostream>
#include <unordered_map>

namespace client {

// Static instance for singleton pattern
SignalsHandler* SignalsHandler::instance_ = nullptr;
std::unordered_map<int, std::function<void()>> SignalsHandler::signalHandlers_;

SignalsHandler* SignalsHandler::getInstance() {
    if (instance_ == nullptr) {
        instance_ = new SignalsHandler();
    }
    return instance_;
}

SignalsHandler::SignalsHandler() {
    // Initialize with default handlers
}

SignalsHandler::~SignalsHandler() {
    // Cleanup
}

void SignalsHandler::setupSignalHandlers() {
    signal(SIGINT, SignalsHandler::signalCallbackHandler);
    signal(SIGTERM, SignalsHandler::signalCallbackHandler);
    signal(SIGABRT, SignalsHandler::signalCallbackHandler);
}

void SignalsHandler::registerHandler(int signum, std::function<void()> handler) {
    signalHandlers_[signum] = handler;
}

void SignalsHandler::signalCallbackHandler(int signum) {
    std::cout << "Received signal " << signum << std::endl;
    
    // Execute registered handler if exists
    if (signalHandlers_.find(signum) != signalHandlers_.end()) {
        signalHandlers_[signum]();
    }
    
    // Default behavior for common signals
    switch (signum) {
        case SIGINT:
        case SIGTERM:
            std::cout << "Terminating application gracefully..." << std::endl;
            exit(signum);
            break;
        default:
            break;
    }
}

} // namespace client