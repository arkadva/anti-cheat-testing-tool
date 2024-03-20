#include <Windows.h>
#include <tlhelp32.h>
#include "utilities.h"

namespace utilities {
  DWORD GetPIDByName(const std::wstring& name) {
    LPCWSTR process_name = name.c_str();
    HANDLE snapshot = INVALID_HANDLE_VALUE;

    snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0); // create snapshot

    PROCESSENTRY32W pe32w;
    pe32w.dwSize = sizeof(PROCESSENTRY32W);

    if (snapshot == INVALID_HANDLE_VALUE) {
      return 0;
    }

    if (!Process32FirstW(snapshot, &pe32w)) { // try getting the first process from the snapshot
      CloseHandle(snapshot);
      return 0;
    }

    DWORD process_id = 0;
    do // loop through the snapshot until we find the desiered process
    {
      if (wcscmp(pe32w.szExeFile, process_name) == 0) {
        process_id = pe32w.th32ProcessID;
        break;
      }
    } while (Process32NextW(snapshot, &pe32w));

    CloseHandle(snapshot);
    return process_id;
  }

  HANDLE GetHandleByName(const std::wstring& name, BOOL inherit_handle, DWORD desired_access) {
    return OpenProcess(GetPIDByName(name), inherit_handle, desired_access);
  }

  PIMAGE_NT_HEADERS GetNTHeaders(HMODULE module) {
    PIMAGE_DOS_HEADER dos_header = reinterpret_cast<PIMAGE_DOS_HEADER>(module);
    PIMAGE_NT_HEADERS nt_headers = reinterpret_cast<PIMAGE_NT_HEADERS>((LPBYTE)module + dos_header->e_lfanew);
    return nt_headers;
  }

  PIMAGE_IMPORT_DESCRIPTOR GetImportDescriptor(HMODULE module) {
    PIMAGE_NT_HEADERS nt_headers = GetNTHeaders(module);
    PIMAGE_IMPORT_DESCRIPTOR import_descriptor = PIMAGE_IMPORT_DESCRIPTOR((LPBYTE)module + nt_headers->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress);
    return import_descriptor;
  }

  PIMAGE_EXPORT_DIRECTORY GetExportDirectory(HMODULE module) {
    PIMAGE_NT_HEADERS nt_headers = GetNTHeaders(module);
    PIMAGE_EXPORT_DIRECTORY export_directory = PIMAGE_EXPORT_DIRECTORY((LPBYTE)module + nt_headers->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress);
    return export_directory;
  }

  std::vector<void*> GetVMTFunctions(void** vtable) {
    std::vector<void*> functions;

    for (int i = 0; vtable[i] != nullptr; ++i) {
      functions.push_back(vtable[i]);
    }

    return functions;
  }
}
