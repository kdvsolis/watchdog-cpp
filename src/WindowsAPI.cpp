#include "WindowsAPI.h"


struct EnumData {
    DWORD pid;
    HWND hwnd;
};

bool WindowsAPI::startProcess(const std::string& name, const std::string& path, const std::vector<std::string>& params) {
    std::string command = "\"" + path + "\"";
    for (const auto& param : params) {
        command += " " + param;
    }

    STARTUPINFO si = { sizeof(STARTUPINFO) };
    PROCESS_INFORMATION pi;

    return CreateProcess(nullptr, command.data(), nullptr, nullptr, FALSE, 0, nullptr, nullptr, &si, &pi);
}

bool WindowsAPI::isProcessRunning(const std::string& name) {
    PROCESSENTRY32 entry;
    entry.dwSize = sizeof(PROCESSENTRY32);
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

    if (snapshot == INVALID_HANDLE_VALUE) {
        return false;
    }

    if (Process32First(snapshot, &entry)) {
        do {
            std::string exeFile(entry.szExeFile);
            std::transform(exeFile.begin(), exeFile.end(), exeFile.begin(), ::tolower);
            std::string lowerName = name;
            std::transform(lowerName.begin(), lowerName.end(), lowerName.begin(), ::tolower);

            if (lowerName == exeFile) {
                CloseHandle(snapshot);
                return true;
            }
        } while (Process32Next(snapshot, &entry));
    }

    CloseHandle(snapshot);
    return false;
}

bool WindowsAPI::stopProcess(const std::string& name) {
    PROCESSENTRY32 entry;
    entry.dwSize = sizeof(PROCESSENTRY32);
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

    if (Process32First(snapshot, &entry)) {
        do {
            std::string exeFile(entry.szExeFile);
            std::transform(exeFile.begin(), exeFile.end(), exeFile.begin(), ::tolower);
            std::string lowerName = name;
            std::transform(lowerName.begin(), lowerName.end(), lowerName.begin(), ::tolower);

            if (lowerName == exeFile) {
                HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, FALSE, entry.th32ProcessID);
                if (hProcess != nullptr) {
                    TerminateProcess(hProcess, 0);
                    CloseHandle(hProcess);
                    CloseHandle(snapshot);
                    return true;
                }
            }
        } while (Process32Next(snapshot, &entry));
    }

    CloseHandle(snapshot);
    return false;
}

bool WindowsAPI::bringToForeground(const std::string& processName) {
    std::vector<DWORD> pids = findProcessIdsByName(processName);

    for (DWORD targetPid : pids) {
        HWND targetHwnd = findWindowByProcessId(targetPid);

        if (targetHwnd) {
            // Attach input threads to allow window focus changes
            DWORD foregroundThreadId = GetWindowThreadProcessId(GetForegroundWindow(), NULL);
            DWORD targetThreadId = GetWindowThreadProcessId(targetHwnd, NULL);

            if (foregroundThreadId != targetThreadId) {
                AttachThreadInput(foregroundThreadId, targetThreadId, TRUE);
            }

            // Restore and bring the window to the foreground
            if (IsIconic(targetHwnd)) {
                ShowWindow(targetHwnd, SW_RESTORE);
            }
            SetForegroundWindow(targetHwnd);
            SetFocus(targetHwnd);
            SwitchToThisWindow(targetHwnd, TRUE);

            if (foregroundThreadId != targetThreadId) {
                AttachThreadInput(foregroundThreadId, targetThreadId, FALSE);
            }

            return true;
        }
    }

    return false;
}

std::string WindowsAPI::getForegroundProcessName() {
    HWND foregroundHwnd = ::GetForegroundWindow();
    DWORD processId;
    ::GetWindowThreadProcessId(foregroundHwnd, &processId);

    HANDLE processHandle = ::OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, processId);
    if (processHandle) {
        char processName[MAX_PATH];
        if (::GetModuleFileNameEx(processHandle, NULL, processName, MAX_PATH)) {
            ::CloseHandle(processHandle);
            std::string name(processName);
            std::transform(name.begin(), name.end(), name.begin(), ::tolower);
            return name;
        }
        ::CloseHandle(processHandle);
    }
    return "";
}



std::vector<DWORD> WindowsAPI::findProcessIdsByName(const std::string& processName) {
    std::vector<DWORD> pids;
    PROCESSENTRY32 entry;
    entry.dwSize = sizeof(PROCESSENTRY32);
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

    if (snapshot == INVALID_HANDLE_VALUE) {
        return pids;
    }

    if (Process32First(snapshot, &entry)) {
        do {
            std::string exeFile(entry.szExeFile);
            std::transform(exeFile.begin(), exeFile.end(), exeFile.begin(), ::tolower);
            std::string lowerName = processName;
            std::transform(lowerName.begin(), lowerName.end(), lowerName.begin(), ::tolower);

            if (lowerName == exeFile) {
                pids.push_back(entry.th32ProcessID);
            }
        } while (Process32Next(snapshot, &entry));
    }
    CloseHandle(snapshot);

    return pids;
}

HWND WindowsAPI::findWindowByProcessId(DWORD pid) {
    EnumData data = { pid, nullptr };

    EnumWindows([](HWND hWnd, LPARAM lParam) -> BOOL {
        EnumData& data = *reinterpret_cast<EnumData*>(lParam);
        DWORD windowPid = 0;
        GetWindowThreadProcessId(hWnd, &windowPid);
        std::cout << "Found Window PID: " << windowPid << std::endl;

        if (windowPid == data.pid && IsWindowVisible(hWnd) && GetWindow(hWnd, GW_OWNER) == NULL) {
            data.hwnd = hWnd;
            return FALSE;  // Stop enumeration
        }
        return TRUE;  // Continue enumeration
     }, reinterpret_cast<LPARAM>(&data));

    return data.hwnd;
}