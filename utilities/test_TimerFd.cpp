#include <gtest/gtest.h>
#include <chrono>
#include <thread>
#include "TimerFd.h"

// Mock class for testing TimerFd
class MockTimerFd : public TimerFd {
public:
    MockTimerFd() : timeoutCount(0) {}

    void onTimeout() override {
        ++timeoutCount; // Increment the count each time the timer expires
    }

    int timeoutCount; // Tracks the number of times the timer expired
};

// Test: Timer triggers correctly
TEST(TimerFdTest, TimerTriggersCorrectly) {
    MockTimerFd timer;
    timer.SetTimer(std::chrono::milliseconds(100), std::chrono::milliseconds(100)); // Trigger every 100ms
    timer.Start();

    std::this_thread::sleep_for(std::chrono::milliseconds(350)); // Wait for 3-4 timeouts
    timer.Stop();

    EXPECT_GE(timer.timeoutCount, 3); // Expect at least 3 timeouts
    EXPECT_LE(timer.timeoutCount, 4); // Ensure no extra timeouts occurred
}

// Test: Timer stops correctly
TEST(TimerFdTest, TimerStopsCorrectly) {
    MockTimerFd timer;
    timer.SetTimer(std::chrono::milliseconds(100), std::chrono::milliseconds(100));
    timer.Start();

    std::this_thread::sleep_for(std::chrono::milliseconds(150)); // Wait for 1-2 timeouts
    timer.Stop();

    int countAfterStop = timer.timeoutCount;
    std::this_thread::sleep_for(std::chrono::milliseconds(200)); // Wait to ensure no more timeouts occur

    EXPECT_EQ(timer.timeoutCount, countAfterStop); // Ensure no additional timeouts occurred after stop
}

// Test: Timer one-shot mode
TEST(TimerFdTest, TimerOneShot) {
    MockTimerFd timer;
    timer.SetTimer(std::chrono::milliseconds(100), std::chrono::milliseconds(0)); // One-shot timer
    timer.Start();

    std::this_thread::sleep_for(std::chrono::milliseconds(200)); // Wait for the timer to expire
    timer.Stop();

    EXPECT_EQ(timer.timeoutCount, 1); // Ensure the timer triggered exactly once
}

// Test: Timer handles invalid settings
TEST(TimerFdTest, TimerHandlesInvalidSettings) {
    MockTimerFd timer;

    // Invalid timer settings should throw an exception
    EXPECT_THROW(timer.SetTimer(std::chrono::milliseconds(-1)), std::runtime_error);
}

// Test: Timer handles rapid start/stop
TEST(TimerFdTest, TimerHandlesRapidStartStop) {
    MockTimerFd timer;

    for (int i = 0; i < 10; ++i) {
        timer.SetTimer(std::chrono::milliseconds(100), std::chrono::milliseconds(100));
        timer.Start();
        std::this_thread::sleep_for(std::chrono::milliseconds(50)); // Let it run briefly
        timer.Stop();
    }

    EXPECT_LE(timer.timeoutCount, 10); // Ensure no unexpected behavior
}