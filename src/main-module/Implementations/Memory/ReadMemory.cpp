#include <Windows.h>
#include "../../Utils/logger.h"
#include "../../Utils/utilities.h"
#include "ReadMemory.h"

// prototype for NtReadVirtualMemory
typedef NTSTATUS(NTAPI* PFN_NtReadVirtualMemory)(
  HANDLE ProcessHandle,
  PVOID BaseAddress,
  PVOID Buffer,
  ULONG NumberOfBytesToRead,
  PULONG NumberOfBytesRead);

BOOL RPM(HANDLE process, LPCVOID address, LPVOID buffer, SIZE_T size, SIZE_T* read) {
  return ReadProcessMemory(process, address, buffer, size, read);
}

// NOTE passing the bytesRead variable to NtReadVirtualMemory will cause a stack corruption when compiled in Debug mode
BOOL NTRVM(HANDLE process, PVOID address, PVOID buffer, ULONG size, PULONG read) {
  HMODULE hNtdll = LoadLibraryA("ntdll.dll");
  if (!hNtdll) {
    return FALSE;
  }

  PFN_NtReadVirtualMemory pNtReadVirtualMemory = (PFN_NtReadVirtualMemory) GetProcAddress(hNtdll, "NtReadVirtualMemory");
  if (!pNtReadVirtualMemory) {
    FreeLibrary(hNtdll);
    return FALSE;
  }

  NTSTATUS status = pNtReadVirtualMemory(process, address, buffer, size, read);
  FreeLibrary(hNtdll);
  return NT_SUCCESS(status);
}

BOOL Read(DWORD pid, PVOID address, PVOID buffer, ULONG size, BYTE type) {
  Logger& logger = Logger::getInstance();

#ifdef DEBUG
  LOG_CALL("pid = %lu, address = %p, buffer = %p, size = %lu, type = %u", pid, address, buffer, size, type);
#endif

  SIZE_T bytesRead = 0;
  ULONG bytesReadNt = 0;

  HANDLE processHandle = OpenProcess(PROCESS_VM_READ, FALSE, pid);

  if (processHandle == NULL) {
    LOG_ERROR("Failed to open handle to process.");
    return FALSE;
  }

#ifdef DEBUG
  LOG_INFO("Opened handle %lu to process.", processHandle);
#endif

  BOOL success = TRUE;

  if (type & kReadProcessMemory) {
    BOOL status = RPM(processHandle, (LPCVOID)address, buffer, size, &bytesRead);
    success &= status;

    if (status) {
      LOG_INFO("ReadProcessMemory call succeeded. Bytes read: %d.", bytesRead);
    }
    else {
      LOG_ERROR("ReadProcessMemory returned false.");
    }
  }

  if (type & kNtReadVirtualMemory) {
    BOOL status = NTRVM(processHandle, (PVOID)address, buffer, (ULONG)size, &bytesReadNt);
    success &= status;

    if (status) {
      LOG_INFO("NtReadVirtualMemory call succeeded. Bytes read: %d.", bytesReadNt);
    }
    else {
      LOG_ERROR("NtReadVirtualMemory returned false.");
    }
  }

  CloseHandle(processHandle);
  return success;
}