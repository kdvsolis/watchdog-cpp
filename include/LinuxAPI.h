#ifndef _WIN32
#pragma once
#include "NativeAPIInterface.h"

class LinuxAPI : public NativeAPIInterface {
    public:
        bool startProcess(const std::string& name, const std::string& path, const std::vector<std::string>& startupParams) override;
        bool isProcessRunning(const std::string& name) override;
        bool stopProcess(const std::string& name) override;
        bool bringToForeground(const std::string& processName) override;
        std::string getForegroundProcessName() override;  // Add this method

    private:
        std::vector<DWORD> findProcessIdsByName(const std::string& processName);
        HWND findWindowByProcessId(DWORD pid);
};

#endif