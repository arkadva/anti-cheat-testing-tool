#include "process.h"
#include <stdexcept>
#include <vector>
#include <string>
#include <iostream>

Process::Process(const std::wstring& path, const std::wstring& args, const std::wstring& working_directory)
  : pid_(0), path_(path), args_(args), working_directory_(working_directory) {
  StartProcess();
}

Process::Process(const std::wstring& path, const DWORD pid) : pid_(pid), path_(path), args_(), working_directory_() { }

DWORD Process::GetPid() const {
  return pid_;
}

const std::wstring& Process::GetPath() const {
  return path_;
}

BOOL Process::StartProcess() {
  STARTUPINFOW startup_info;
  PROCESS_INFORMATION process_info;

  ZeroMemory(&startup_info, sizeof(startup_info));
  startup_info.cb = sizeof(startup_info);
  ZeroMemory(&process_info, sizeof(process_info));

  std::wstring command_line = path_;
  
  if (!args_.empty()) {
    command_line += args_;
  }

  // Convert std::wstring to LPWSTR
  std::vector<wchar_t> writable_command_line(command_line.begin(), command_line.end());
  writable_command_line.push_back(0);

  if (!CreateProcessW(
    NULL,                      // lpApplicationName
    writable_command_line.data(), // lpCommandLine
    NULL,                      // lpProcessAttributes
    NULL,                      // lpThreadAttributes
    FALSE,                     // bInheritHandles
    CREATE_NEW_CONSOLE,        // dwCreationFlags
    NULL,                      // lpEnvironment
    working_directory_.empty() ? NULL : working_directory_.c_str(), // lpCurrentDirectory
    &startup_info,             // lpStartupInfo
    &process_info              // lpProcessInformation
  )) {
    return FALSE;
    //throw std::runtime_error("Failed to create process");
  }
  else {
    pid_ = static_cast<unsigned long>(process_info.dwProcessId);
    CloseHandle(process_info.hProcess);
    CloseHandle(process_info.hThread);
    return TRUE;
  }
}

BOOL Process::KillProcess() {
  HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, FALSE, pid_);

  if (hProcess == NULL) {
    return FALSE;
    // throw std::runtime_error("Failed to open process for termination");
  }

  if (!TerminateProcess(hProcess, 0)) {
    CloseHandle(hProcess);
    return FALSE;
    // throw std::runtime_error("Failed to terminate process");
  }

  CloseHandle(hProcess);
  pid_ = 0;

  return TRUE;
}

BOOL Process::RestartProcess() {
  if (!KillProcess())
    return FALSE;

  if (!StartProcess())
    return FALSE;

  return TRUE;
}
