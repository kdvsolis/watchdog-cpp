#ifndef _WIN32
#include "LinuxAPI.h"
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <signal.h>
#include <dirent.h>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <iostream>
#include <vector>
#include <cstring>
#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/Xutil.h>

// Struct for storing process ID and window ID
struct EnumData {
    pid_t pid;
    Window window;
};

// Start a process using fork and exec
bool LinuxAPI::startProcess(const std::string& name, const std::string& path, const std::vector<std::string>& params) {
    pid_t pid = fork();
    if (pid == 0) {  // Child process
        std::vector<const char*> args;
        args.push_back(path.c_str());
        for (const auto& param : params) {
            args.push_back(param.c_str());
        }
        args.push_back(nullptr);

        execvp(path.c_str(), const_cast<char* const*>(args.data()));
        _exit(EXIT_FAILURE);  // Only reached if exec fails
    }
    return (pid > 0);  // Parent process or fork failed
}

// Check if a process with the given name is running
bool LinuxAPI::isProcessRunning(const std::string& name) {
    std::string lowerName = name;
    std::transform(lowerName.begin(), lowerName.end(), lowerName.begin(), ::tolower);

    for (const auto& entry : std::filesystem::directory_iterator("/proc")) {
        if (entry.is_directory()) {
            std::string statusFile = entry.path().string() + "/status";
            std::ifstream status(statusFile);
            if (status.is_open()) {
                std::string line;
                while (std::getline(status, line)) {
                    if (line.find("Name:") == 0) {
                        std::string procName = line.substr(6);
                        std::transform(procName.begin(), procName.end(), procName.begin(), ::tolower);
                        if (procName == lowerName) {
                            return true;
                        }
                    }
                }
            }
        }
    }
    return false;
}

// Stop a process with the given name
bool LinuxAPI::stopProcess(const std::string& name) {
    std::string lowerName = name;
    std::transform(lowerName.begin(), lowerName.end(), lowerName.begin(), ::tolower);

    for (const auto& entry : std::filesystem::directory_iterator("/proc")) {
        if (entry.is_directory()) {
            std::string statusFile = entry.path().string() + "/status";
            std::ifstream status(statusFile);
            if (status.is_open()) {
                std::string line;
                pid_t pid = std::stoi(entry.path().filename().string());
                while (std::getline(status, line)) {
                    if (line.find("Name:") == 0) {
                        std::string procName = line.substr(6);
                        std::transform(procName.begin(), procName.end(), procName.begin(), ::tolower);
                        if (procName == lowerName) {
                            if (kill(pid, SIGTERM) == 0) {
                                return true;
                            }
                        }
                    }
                }
            }
        }
    }
    return false;
}

// Bring a process window to the foreground
bool LinuxAPI::bringToForeground(const std::string& processName) {
    std::vector<pid_t> pids = findProcessIdsByName(processName);

    for (pid_t targetPid : pids) {
        Window targetWindow = findWindowByProcessId(targetPid);

        if (targetWindow != 0) {
            Display* display = XOpenDisplay(NULL);
            if (display) {
                XRaiseWindow(display, targetWindow);
                XSetInputFocus(display, targetWindow, RevertToPointerRoot, CurrentTime);
                XFlush(display);
                XCloseDisplay(display);
                return true;
            }
        }
    }

    return false;
}

// Get the name of the process currently in the foreground
std::string LinuxAPI::getForegroundProcessName() {
    Display* display = XOpenDisplay(NULL);
    if (!display) {
        return "";
    }

    Window root = DefaultRootWindow(display);
    Window focusedWindow;
    int revert;

    XGetInputFocus(display, &focusedWindow, &revert);

    pid_t processId = 0;
    Atom pidAtom = XInternAtom(display, "_NET_WM_PID", True);
    if (pidAtom != None) {
        unsigned char* prop;
        Atom actualType;
        int actualFormat;
        unsigned long nitems;
        unsigned long bytesAfter;

        if (XGetWindowProperty(display, focusedWindow, pidAtom, 0, 1, False, XA_CARDINAL,
            &actualType, &actualFormat, &nitems, &bytesAfter, &prop) == Success) {
            if (prop) {
                processId = *(pid_t*)prop;
                XFree(prop);
            }
        }
    }

    XCloseDisplay(display);

    if (processId != 0) {
        std::ifstream cmdline("/proc/" + std::to_string(processId) + "/cmdline");
        std::string processName;
        if (cmdline.is_open()) {
            std::getline(cmdline, processName, '\0');
            cmdline.close();
        }
        return processName;
    }

    return "";
}

// Find all process IDs by process name
std::vector<pid_t> LinuxAPI::findProcessIdsByName(const std::string& processName) {
    std::vector<pid_t> pids;
    std::string lowerName = processName;
    std::transform(lowerName.begin(), lowerName.end(), lowerName.begin(), ::tolower);

    for (const auto& entry : std::filesystem::directory_iterator("/proc")) {
        if (entry.is_directory()) {
            std::string statusFile = entry.path().string() + "/status";
            std::ifstream status(statusFile);
            if (status.is_open()) {
                std::string line;
                while (std::getline(status, line)) {
                    if (line.find("Name:") == 0) {
                        std::string procName = line.substr(6);
                        std::transform(procName.begin(), procName.end(), procName.begin(), ::tolower);
                        if (procName == lowerName) {
                            pids.push_back(std::stoi(entry.path().filename().string()));
                        }
                    }
                }
            }
        }
    }

    return pids;
}

// Find a window by process ID
Window LinuxAPI::findWindowByProcessId(pid_t pid) {
    EnumData data = { pid, 0 };

    Display* display = XOpenDisplay(NULL);
    if (!display) {
        return 0;
    }

    Window root = DefaultRootWindow(display);
    enumerateWindows(display, root, &data);

    XCloseDisplay(display);
    return data.window;
}

// Helper function to recursively enumerate windows
void LinuxAPI::enumerateWindows(Display* display, Window root, EnumData* data) {
    Window root_return, parent_return;
    Window* children;
    unsigned int nchildren;

    if (XQueryTree(display, root, &root_return, &parent_return, &children, &nchildren)) {
        for (unsigned int i = 0; i < nchildren; i++) {
            pid_t windowPid = 0;
            Atom pidAtom = XInternAtom(display, "_NET_WM_PID", True);
            if (pidAtom != None) {
                unsigned char* prop;
                Atom actualType;
                int actualFormat;
                unsigned long nitems;
                unsigned long bytesAfter;

                if (XGetWindowProperty(display, children[i], pidAtom, 0, 1, False, XA_CARDINAL,
                    &actualType, &actualFormat, &nitems, &bytesAfter, &prop) == Success) {
                    if (prop) {
                        windowPid = *(pid_t*)prop;
                        XFree(prop);
                    }
                }
            }

            if (windowPid == data->pid) {
                data->window = children[i];
                break;
            }

            enumerateWindows(display, children[i], data);
        }

        if (children) {
            XFree(children);
        }
    }
}

#endif