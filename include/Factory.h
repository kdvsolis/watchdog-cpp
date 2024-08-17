#pragma once
#include "NativeAPIInterface.h"
#include "WindowsAPI.h"
#include "LinuxAPI.h"

std::unique_ptr<NativeAPIInterface> createAPI() {
#ifdef _WIN32
    return std::make_unique<WindowsAPI>();
#else
    return std::make_unique<LinuxAPI>();
#endif
}
