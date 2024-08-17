#include "WatchdogWorkflow.h"

WatchdogWorkflow::WatchdogWorkflow(std::unique_ptr<NativeAPIInterface> api, std::shared_ptr<ConfigurationLoader> configLoader, std::shared_ptr<Logger> logger)
    : api(std::move(api)), configLoader(configLoader), logger(logger) {}

void WatchdogWorkflow::run() {
    configLoader->loadConfig();
    startAllProcesses();
    manageForegroundProcess();  // Handle initial foreground process

    while (true) {
        monitorProcesses();
        manageForegroundProcess();  // Handle any foreground process changes
        std::this_thread::sleep_for(std::chrono::seconds(5));
    }
}

void WatchdogWorkflow::startAllProcesses() {
    for (const auto& process : configLoader->getProcesses()) {
        if (!api->isProcessRunning(process.name)) {
            api->startProcess(process.name, process.path, process.startupParams);
            logger->log("Started process: " + process.name);
            std::this_thread::sleep_for(std::chrono::seconds(1));  // Add a small delay
        }
        else {
            logger->log("Process already running: " + process.name);
        }
    }
}

void WatchdogWorkflow::monitorProcesses() {
    configLoader->reloadConfigIfChanged();
    checkAndRestartProcesses();
}

void WatchdogWorkflow::checkAndRestartProcesses() {
    for (const auto& process : configLoader->getProcesses()) {
        if (!api->isProcessRunning(process.name)) {
            api->startProcess(process.name, process.path, process.startupParams);
            logger->log("Restarted process: " + process.name);
            std::this_thread::sleep_for(std::chrono::seconds(1));  // Add a small delay
        }
        else {
            logger->log("Process is running: " + process.name);
        }
    }
}

void WatchdogWorkflow::manageForegroundProcess() {
    static std::string currentForegroundProcess;

    std::string newForegroundProcess = configLoader->getForegroundProcess();
    if (newForegroundProcess != currentForegroundProcess) {
        if (!newForegroundProcess.empty()) {
            // Try to bring the window to the foreground
            bool success = api->bringToForeground(newForegroundProcess);
            if (success) {
                logger->log("Brought to foreground: " + newForegroundProcess);
                currentForegroundProcess = newForegroundProcess;
            }
            else {
                logger->log("Failed to bring to foreground: " + newForegroundProcess);
            }
        }
    }

    // Polling or check if the window is still in focus periodically
    std::string currentForegroundProcessName = api->getForegroundProcessName();
    if (currentForegroundProcessName != currentForegroundProcess) {
        // Log if the window was lost or transitioned
        if (!currentForegroundProcess.empty()) {
            logger->log("Lost foreground: " + currentForegroundProcess);
        }
        currentForegroundProcess.clear();
    }
}
