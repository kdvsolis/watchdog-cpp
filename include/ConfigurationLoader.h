#pragma once
#include <string>
#include <vector>
#include <nlohmann/json.hpp>
#include <fstream>
#include <filesystem>
#include <iostream>


struct ProcessConfig {
    std::string name;
    std::string path;
    std::vector<std::string> startupParams;
    bool foreground;
};


class ConfigurationLoader {
    public:
        ConfigurationLoader(const std::string& configFilePath);
        bool loadConfig();
        const std::vector<ProcessConfig>& getProcesses() const;
        bool reloadConfigIfChanged();
        const std::string getForegroundProcess();

    private:
        std::string configFilePath;
        std::vector<ProcessConfig> processes;
        std::time_t lastModifiedTime;
        nlohmann::json configJson;

        bool checkFileModified();
};
