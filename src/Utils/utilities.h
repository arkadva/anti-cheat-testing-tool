#ifndef UTILITIES_H_
#define UTILITIES_H_

#include <Windows.h>
#include <string>
#include <stdexcept>
#include <cstdarg>
#include <vector>
#include <iostream>

namespace utilities {
  DWORD GetPIDByName(const std::wstring& name);
  HANDLE GetHandleByName(const std::wstring& name, BOOL inherit_handle, DWORD desired_access);

  // pe utilities
  PIMAGE_EXPORT_DIRECTORY GetExportDirectory(HMODULE module);
  PIMAGE_IMPORT_DESCRIPTOR GetImportDescriptor(HMODULE module);
  PIMAGE_NT_HEADERS GetNTHeaders(HMODULE module);

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
