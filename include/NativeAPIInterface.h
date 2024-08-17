#pragma once
#include <string>
#include <vector>

class NativeAPIInterface {
    public:
        virtual bool startProcess(const std::string& name, const std::string& path, const std::vector<std::string>& startupParams) = 0;
        virtual bool isProcessRunning(const std::string& name) = 0;
        virtual bool stopProcess(const std::string& name) = 0;
        virtual bool bringToForeground(const std::string& processName) = 0;
        virtual std::string getForegroundProcessName() = 0;
        virtual ~NativeAPIInterface() = default;
};
