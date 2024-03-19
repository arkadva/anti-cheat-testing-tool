#include "process.h"

Process::Process(const std::wstring& path) : _pid(0) {
  STARTUPINFOW startup_info;
  PROCESS_INFORMATION process_info;

  ZeroMemory(&startup_info, sizeof(startup_info));
  startup_info.cb = sizeof(startup_info);
  ZeroMemory(&process_info, sizeof(process_info));

  // lpCommandLine is LPWSTR (wchar_t*) while std::wstring::data() is const
  std::vector<wchar_t> writable_path(path.begin(), path.end());
  writable_path.push_back(0);

  if (!CreateProcessW(
    NULL,                 // lpApplicationName
    writable_path.data(), // lpCommandLine
    NULL,                 // lpProcessAttributes
    NULL,                 // lpThreadAttributes
    FALSE,                // bInheritHandles
    CREATE_NEW_CONSOLE,   // dwCreationFlags
    NULL,                 // lpEnvironment
    NULL,                 // lpCurrentDirectory
    &startup_info,        // lpStartupInfo
    &process_info         // lpProcessInformation
  )) {
    // exception/error handling
  }
  else {
    _pid = static_cast<unsigned long>(process_info.dwProcessId);
    CloseHandle(process_info.hProcess);
    CloseHandle(process_info.hThread);
  }
}
Process::Process(const DWORD pid) : _pid(pid) { }

DWORD Process::GetPid() const {
  return _pid;
}