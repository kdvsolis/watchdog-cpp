// WatchdogWorkflow.h
#pragma once
#include <memory>
#include <thread>
#include <chrono>
#include <iostream>
#include "NativeAPIInterface.h"
#include "ConfigurationLoader.h"
#include "Logger.h" // Add a logger

class WatchdogWorkflow {
public:
    WatchdogWorkflow(std::unique_ptr<NativeAPIInterface> api, std::shared_ptr<ConfigurationLoader> configLoader, std::shared_ptr<Logger> logger);
    void run();
    void startAllProcesses();
    void monitorProcesses();
    void checkAndRestartProcesses();
    void manageForegroundProcess();


private:
    std::unique_ptr<NativeAPIInterface> api;
    std::shared_ptr<ConfigurationLoader> configLoader;
    std::shared_ptr<Logger> logger; // Add a logger
    std::string foregroundProcessName;

};