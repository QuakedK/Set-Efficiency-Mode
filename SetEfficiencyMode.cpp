// File: SetEfficiencyMode.cpp
// Description: Sets listed processes to Efficiency Mode (EcoQoS) on Windows 11.
// Supports --no-console to run silently.

#define UNICODE
#define _UNICODE

#include <windows.h>
#include <tlhelp32.h>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>

// Trim whitespace from start and end
std::wstring trim(const std::wstring& s) {
    size_t start = s.find_first_not_of(L" \t\r\n");
    size_t end = s.find_last_not_of(L" \t\r\n");
    return (start == std::wstring::npos) ? L"" : s.substr(start, end - start + 1);
}

// Convert string to lowercase
std::wstring toLower(const std::wstring& str) {
    std::wstring copy = str;
    std::transform(copy.begin(), copy.end(), copy.begin(), ::towlower);
    return copy;
}

// Load process names from config file
std::vector<std::wstring> loadConfig(const std::wstring& filename) {
    std::vector<std::wstring> processNames;
    std::wifstream file(filename);
    if (!file.is_open()) {
        std::wcerr << L"Failed to open config file: " << filename << L"\n";
        return processNames;
    }

    std::wstring line;
    while (std::getline(file, line)) {
        line = trim(line);
        if (line.empty() || line[0] == L'#') continue; // skip empty lines and comments
        processNames.push_back(toLower(line));
    }
    return processNames;
}

// Check if exeName is in the config list
bool matchesConfig(const std::vector<std::wstring>& config, const std::wstring& exeName) {
    std::wstring lowerExe = toLower(exeName);
    return std::find(config.begin(), config.end(), lowerExe) != config.end();
}

// Enable Efficiency Mode (EcoQoS) on a process
void enableEfficiencyMode(DWORD pid, const std::wstring& exeName) {
    HANDLE hProc = OpenProcess(PROCESS_SET_INFORMATION, FALSE, pid);
    if (!hProc) {
        std::wcerr << L"Failed to open process: " << exeName << L" (PID " << pid << L")\n";
        return;
    }

    // Set process priority to IDLE
    if (!SetPriorityClass(hProc, IDLE_PRIORITY_CLASS)) {
        std::wcerr << L"Failed to set priority for: " << exeName << L"\n";
    }

    // Enable EcoQoS
    PROCESS_POWER_THROTTLING_STATE state = {};
    state.Version = PROCESS_POWER_THROTTLING_CURRENT_VERSION;
    state.ControlMask = PROCESS_POWER_THROTTLING_EXECUTION_SPEED;
    state.StateMask = PROCESS_POWER_THROTTLING_EXECUTION_SPEED;

    if (!SetProcessInformation(hProc, ProcessPowerThrottling, &state, sizeof(state))) {
        std::wcerr << L"Failed to set Efficiency Mode for: " << exeName << L"\n";
    }
    else {
        std::wcout << L"Efficiency Mode applied: " << exeName << L" (PID " << pid << L")\n";
    }

    CloseHandle(hProc);
}

// Get folder of the executable
std::wstring getExeFolder() {
    wchar_t path[MAX_PATH];
    GetModuleFileNameW(NULL, path, MAX_PATH);
    std::wstring exePath = path;
    size_t pos = exePath.find_last_of(L"\\/");
    if (pos != std::wstring::npos)
        exePath = exePath.substr(0, pos + 1);
    return exePath;
}

int wmain(int argc, wchar_t* argv[]) {
    // Check for --no-console argument
    for (int i = 1; i < argc; ++i) {
        if (std::wstring(argv[i]) == L"--no-console") {
            HWND hwnd = GetConsoleWindow();
            if (hwnd) ShowWindow(hwnd, SW_HIDE);
            break;
        }
    }

    std::wstring exeFolder = getExeFolder();
    const std::wstring configFile = exeFolder + L"config.txt";

    auto processList = loadConfig(configFile);
    if (processList.empty()) {
        std::wcerr << L"Could not read config file: " << configFile << L"\n";
        return 1;
    }

    // Take a snapshot of all processes
    HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnap == INVALID_HANDLE_VALUE) {
        std::wcerr << L"Failed to create process snapshot.\n";
        return 1;
    }

    PROCESSENTRY32W entry = {};
    entry.dwSize = sizeof(PROCESSENTRY32W);

    if (Process32FirstW(hSnap, &entry)) {
        do {
            std::wstring exeName = entry.szExeFile;
            if (matchesConfig(processList, exeName)) {
                enableEfficiencyMode(entry.th32ProcessID, exeName);
            }
        } while (Process32NextW(hSnap, &entry));
    }
    else {
        std::wcerr << L"Failed to enumerate processes.\n";
    }

    CloseHandle(hSnap);
    return 0;
}
