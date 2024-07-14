#ifndef UTILITIES_H_
#define UTILITIES_H_

#include <Windows.h>
#include <string>
#include <stdexcept>
#include <cstdarg>
#include <vector>
#include <iostream>
#include <string>
#include <locale>
#include <codecvt>

namespace utilities {
  DWORD GetThreadIDFromWindow(HWND hwnd);

  HWND FindWindowByPID(DWORD pid);

  DWORD GetMainThreadId(DWORD dwProcessId);

  DWORD GetPIDByName(const std::wstring& name);

  HANDLE GetHandleByName(const std::wstring& name, BOOL inherit_handle, DWORD desired_access);

  uintptr_t GetModuleBaseAddress(DWORD pid, const wchar_t* moduleName);

  std::wstring string_to_wstring(const std::string& str);

  std::wstring GetExecutableName(const std::wstring& full_path);

  template<typename ... Args>
  std::string string_format(const std::string& format, Args ... args) {
    size_t size = std::snprintf(nullptr, 0, format.c_str(), args ...) + 1; // Extra space for '\0'
    if (size <= 0) { throw std::runtime_error("Error during formatting."); }
    std::unique_ptr<char[]> buf(new char[size]);
    std::snprintf(buf.get(), size, format.c_str(), args...);
    return std::string(buf.get(), buf.get() + size - 1); // Exclude the '\0'
  }
}

#endif
