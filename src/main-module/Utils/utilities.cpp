#include <Windows.h>
#include <tlhelp32.h>
#include "utilities.h"

struct EnumWindowsParams {
  DWORD pid;
  HWND hwnd;
};

namespace utilities {
  BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam) {
    DWORD pid;
    GetWindowThreadProcessId(hwnd, &pid);
    EnumWindowsParams* params = reinterpret_cast<EnumWindowsParams*>(lParam);
    if (pid == params->pid) {
      params->hwnd = hwnd;
      return FALSE;
    }
    return TRUE;
  }

  HWND FindWindowByPID(DWORD pid) {
    EnumWindowsParams params = { pid, NULL };
    EnumWindows(EnumWindowsProc, reinterpret_cast<LPARAM>(&params));
    return params.hwnd;
  }

  DWORD GetThreadIDFromWindow(HWND hwnd) {
    DWORD processID;
    DWORD threadID = GetWindowThreadProcessId(hwnd, &processID);
    return threadID;
  }

  DWORD GetMainThreadId(DWORD dwProcessId)
  {
    HANDLE hThreadSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
    if (hThreadSnapshot == INVALID_HANDLE_VALUE)
    {
      return 0;
    }

    THREADENTRY32 te32;
    te32.dwSize = sizeof(THREADENTRY32);
    DWORD mainThreadId = 0;
    FILETIME earliestCreationTime = { MAXDWORD, MAXDWORD };

    if (Thread32First(hThreadSnapshot, &te32))
    {
      do
      {
        if (te32.th32OwnerProcessID == dwProcessId)
        {
          HANDLE hThread = OpenThread(THREAD_QUERY_INFORMATION, FALSE, te32.th32ThreadID);
          if (hThread != NULL)
          {
            FILETIME createTime, exitTime, kernelTime, userTime;
            if (GetThreadTimes(hThread, &createTime, &exitTime, &kernelTime, &userTime))
            {
              if (CompareFileTime(&createTime, &earliestCreationTime) < 0)
              {
                earliestCreationTime = createTime;
                mainThreadId = te32.th32ThreadID;
              }
            }
            CloseHandle(hThread);
          }
        }
      } while (Thread32Next(hThreadSnapshot, &te32));
    }

    CloseHandle(hThreadSnapshot);
    return mainThreadId;
  }

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

  uintptr_t GetModuleBaseAddress(DWORD pid, const wchar_t* moduleName) {
    uintptr_t baseAddress = 0;
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, pid);
    if (hSnapshot != INVALID_HANDLE_VALUE) {
      MODULEENTRY32 moduleEntry;
      moduleEntry.dwSize = sizeof(MODULEENTRY32);
      if (Module32First(hSnapshot, &moduleEntry)) {
        do {
          if (_wcsicmp(moduleEntry.szModule, moduleName) == 0) {
            baseAddress = reinterpret_cast<uintptr_t>(moduleEntry.modBaseAddr);
            break;
          }
        } while (Module32Next(hSnapshot, &moduleEntry));
      }
      CloseHandle(hSnapshot);
    }
    return baseAddress;
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
}
