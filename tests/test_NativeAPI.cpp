#include <gtest/gtest.h>
#include "WindowsAPI.h"
#include "LinuxAPI.h"
#include <vector>
#include <string>
#include <chrono>
#include <thread>

#ifdef _WIN32
#define NATIVE_API WindowsAPI
#else
#define NATIVE_API LinuxAPI
#endif

class NativeAPITest : public ::testing::Test {
protected:
    NATIVE_API nativeAPI;

    void SetUp() override {
        // Setup code if needed
    }

    void TearDown() override {
        // Teardown code if needed
    }
};

// Helper function to add a delay
void wait_for_seconds(int seconds) {
    std::this_thread::sleep_for(std::chrono::seconds(seconds));
}

// Mock a simple process start and stop
TEST_F(NativeAPITest, StartProcess) {
    std::string processName = "Notepad.exe";
    std::string processPath = "C:\\Windows\\SysWOW64\\notepad.exe";  // Use a dummy path for testing
    std::vector<std::string> params = { "" };

    bool result = nativeAPI.startProcess(processName, processPath, params);
    EXPECT_TRUE(result) << "Failed to start the process";
    wait_for_seconds(5); // Wait for 5 seconds
}

TEST_F(NativeAPITest, IsProcessRunning) {
    std::string processName = "Notepad.exe";
    std::string processPath = "C:\\Windows\\SysWOW64\\notepad.exe";  // Use a dummy path for testing
    std::vector<std::string> params = { "Untitled" };

    nativeAPI.startProcess(processName, processPath, params);
    wait_for_seconds(5); // Wait for 5 seconds

    // Simulate that the process is running
    bool result = nativeAPI.isProcessRunning(processName);
    EXPECT_TRUE(result) << "Process should not be running";
}

TEST_F(NativeAPITest, StopProcess) {
    std::string processName = "Notepad.exe";
    std::string processPath = "C:\\Windows\\SysWOW64\\notepad.exe";  // Use a dummy path for testing
    std::vector<std::string> params = { "Untitled" };

    nativeAPI.startProcess(processName, processPath, params);
    wait_for_seconds(5); // Wait for 5 seconds

    // Simulate stopping the process
    bool result = nativeAPI.stopProcess(processName);
    EXPECT_TRUE(result) << "Process should not be stopped as it's not running";
}

TEST_F(NativeAPITest, BringToForeground) {
    std::string processName = "Notepad.exe";
    std::string processPath = "C:\\Windows\\SysWOW64\\notepad.exe";  // Use a dummy path for testing
    std::vector<std::string> params = { "Untitled" };

    nativeAPI.startProcess(processName, processPath, params);
    wait_for_seconds(5); // Wait for 5 seconds

    // Simulate bringing a process to the foreground
    bool result = nativeAPI.bringToForeground(processName);
    EXPECT_TRUE(result) << "Failed to bring process to foreground";
}

TEST_F(NativeAPITest, GetForegroundProcessName) {
    std::string processName = "Notepad.exe";
    std::string processPath = "C:\\Windows\\SysWOW64\\notepad.exe";  // Use a dummy path for testing
    std::vector<std::string> params = { "Untitled" };

    bool result = nativeAPI.startProcess(processName, processPath, params);
    wait_for_seconds(5); // Wait for 5 seconds
    processName = nativeAPI.getForegroundProcessName();

    // Since there is no actual process in this mock environment, it should be empty
    EXPECT_NE(processName, "") << "Foreground process name should not be empty";
}
