#include "ConfigurationLoader.h"

using json = nlohmann::json;

ConfigurationLoader::ConfigurationLoader(const std::string& configFilePath) 
    : configFilePath(configFilePath), lastModifiedTime(0) {}

bool ConfigurationLoader::loadConfig() {
    std::ifstream configFile(configFilePath);
    if (!configFile.is_open()) {
        std::cerr << "Failed to open config file: " << configFilePath << std::endl;
        return false;
    }

    try {
        configFile >> configJson;

        // Parse processes
        processes.clear();
        for (const auto& process : configJson["processes"]) {
            ProcessConfig pc;
            pc.name = process["name"];
            pc.path = process["path"];
            pc.startupParams = process["startupParams"].get<std::vector<std::string>>();
            pc.foreground = process.value("foreground", false); // Default value is false
            processes.push_back(pc);
        }

        // Update last modified time
        lastModifiedTime = std::filesystem::last_write_time(configFilePath).time_since_epoch().count();

    } catch (const nlohmann::json::parse_error& e) {
        std::cerr << "JSON parse error: " << e.what() << std::endl;
        return false;
    }

    return true;
}


bool ConfigurationLoader::reloadConfigIfChanged() {
    if (checkFileModified()) {
        return loadConfig();
    }
    return false;
}

bool ConfigurationLoader::checkFileModified() {
    auto currentModifiedTime = std::filesystem::last_write_time(configFilePath).time_since_epoch().count();
    return currentModifiedTime != lastModifiedTime;
}

const std::vector<ProcessConfig>& ConfigurationLoader::getProcesses() const {
    return processes;
}

const std::string ConfigurationLoader::getForegroundProcess() {
    for (const auto& process : processes) {
        if (process.foreground) {
            return process.name;
        }
    }
    return "";  // Return empty string if no foreground process is set
}
