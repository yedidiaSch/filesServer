#include <gtest/gtest.h>
#include <chrono>
#include <thread>
#include "threadBase.h"
#include "TimerFd.h"
#include "QueueThread.h"

// Mock class for testing ThreadBase
class MockThreadBase : public ThreadBase {
public:
    MockThreadBase() : executed(false) {}
    void thread() override {
        executed = true;
    }
    bool executed;
};

// Mock class for testing TimerFd
class MockTimerFd : public TimerFd {
public:
    MockTimerFd() : timeoutCalled(false) {}
    void onTimeout() override {
        timeoutCalled = true;
    }
    bool timeoutCalled;
};

// Test ThreadBase
TEST(ThreadBaseTest, StartStop) {
    MockThreadBase mockThread;
    mockThread.start();
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    mockThread.stop();
    EXPECT_TRUE(mockThread.executed);
}

// Test TimerFd
TEST(TimerFdTest, TimerFunctionality) {
    MockTimerFd timer;
    timer.SetTimer(std::chrono::milliseconds(50)); // Shorter delay for quicker response
    timer.Start();
    std::this_thread::sleep_for(std::chrono::milliseconds(300)); // Longer wait to ensure the timer expires
    timer.Stop();
    EXPECT_TRUE(timer.timeoutCalled);
}

// Test QueueThread
TEST(QueueThreadTest, QueueFunctionality) {
    QueueThread queueThread;
    bool taskExecuted = false;
    queueThread.put([&taskExecuted]() {
        taskExecuted = true;
    });
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    queueThread.stop();
    EXPECT_TRUE(taskExecuted);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}