#define _WIN32_WINNT 0x0A00
#include <windows.h>
#include <tlhelp32.h>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>

// Safe fallback for older SDKs
#ifndef PROCESS_POWER_THROTTLING_STATE_VERSION
#define PROCESS_POWER_THROTTLING_STATE_VERSION 1
typedef struct _MY_PROCESS_POWER_THROTTLING_STATE {
    DWORD Version;
    DWORD ControlMask;
    DWORD StateMask;
} MY_PROCESS_POWER_THROTTLING_STATE, * PMY_PROCESS_POWER_THROTTLING_STATE;
#define PROCESS_POWER_THROTTLING_STATE MY_PROCESS_POWER_THROTTLING_STATE
#define PPROCESS_POWER_THROTTLING_STATE PMY_PROCESS_POWER_THROTTLING_STATE
#endif

bool EnableDebugPrivilege() {
    HANDLE hToken;
    TOKEN_PRIVILEGES tkp;

    if (!OpenProcessToken(GetCurrentProcess(),
        TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken))
        return false;

    if (!LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &tkp.Privileges[0].Luid)) {
        CloseHandle(hToken);
        return false;
    }

    tkp.PrivilegeCount = 1;
    tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

    if (!AdjustTokenPrivileges(hToken, FALSE, &tkp, sizeof(tkp), NULL, NULL)) {
        CloseHandle(hToken);
        return false;
    }

    bool success = GetLastError() == ERROR_SUCCESS;
    CloseHandle(hToken);
    return success;
}

std::wstring GetExeFolder() {
    wchar_t path[MAX_PATH];
    GetModuleFileNameW(NULL, path, MAX_PATH);
    std::wstring fullPath(path);
    size_t pos = fullPath.find_last_of(L"\\/");
    return fullPath.substr(0, pos + 1);
}

std::vector<std::wstring> LoadConfig(const std::wstring& filePath) {
    std::vector<std::wstring> processes;
    std::wifstream file(filePath);
    if (!file.is_open()) return processes;

    std::wstring line;
    while (std::getline(file, line)) {
        line.erase(std::remove_if(line.begin(), line.end(), iswspace), line.end());
        if (!line.empty()) processes.push_back(line);
    }
    return processes;
}

int wmain(int argc, wchar_t* argv[]) {
    bool noConsole = false;
    for (int i = 1; i < argc; ++i)
        if (_wcsicmp(argv[i], L"--no-console") == 0) noConsole = true;

    if (!EnableDebugPrivilege()) {
        if (!noConsole) std::wcout << L"Failed to enable SeDebugPrivilege.\n";
        return 1;
    }

    std::wstring exeFolder = GetExeFolder();
    std::wstring configFile = exeFolder + L"config.txt";
    auto configProcesses = LoadConfig(configFile);

    // Original message if config can't be opened or is empty
    if (configProcesses.empty()) {
        if (!noConsole)
            std::wcout << L"Config missing or empty: " << configFile << std::endl;
        return 1;
    }

    // Loop over config entries first
    for (auto& target : configProcesses) {
        bool found = false;

        HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
        if (hSnap != INVALID_HANDLE_VALUE) {
            PROCESSENTRY32W pe;
            pe.dwSize = sizeof(PROCESSENTRY32W);

            if (Process32FirstW(hSnap, &pe)) {
                do {
                    if (_wcsicmp(pe.szExeFile, target.c_str()) == 0) {
                        found = true;

                        DWORD access = PROCESS_SET_LIMITED_INFORMATION | PROCESS_QUERY_LIMITED_INFORMATION;
                        HANDLE hProcess = OpenProcess(access, FALSE, pe.th32ProcessID);
                        if (hProcess) {
                            PROCESS_POWER_THROTTLING_STATE state{};
                            state.Version = PROCESS_POWER_THROTTLING_STATE_VERSION;
                            state.ControlMask = PROCESS_POWER_THROTTLING_EXECUTION_SPEED;
                            state.StateMask = PROCESS_POWER_THROTTLING_EXECUTION_SPEED;

                            if (SetProcessInformation(hProcess, ProcessPowerThrottling, &state, sizeof(state))) {
                                if (!noConsole) std::wcout << L"Efficiency Mode applied: " << target
                                    << L" (PID " << pe.th32ProcessID << L")\n";
                            }
                            else {
                                if (!noConsole) std::wcout << L"Efficiency Mode failed to set: " << target
                                    << L" (PID " << pe.th32ProcessID << L")\n";
                            }
                            CloseHandle(hProcess);
                        }
                        else {
                            if (!noConsole) std::wcout << L"Failed to open process: " << target
                                << L" (PID " << pe.th32ProcessID << L")\n";
                        }
                    }
                } while (Process32NextW(hSnap, &pe));
            }
            CloseHandle(hSnap);
        }

        // If no process matched at all
        if (!found && !noConsole) {
            std::wcout << L"Efficiency Mode failed to set: " << target
                << L" (Process not running)\n";
        }
    }

    return 0;
}
