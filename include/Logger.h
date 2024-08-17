// Logger.h
#pragma once
#include <string>
#include <fstream>

class Logger {
public:
    Logger(const std::string& filename);
    void log(const std::string& message);

private:
    std::ofstream logFile;
};
