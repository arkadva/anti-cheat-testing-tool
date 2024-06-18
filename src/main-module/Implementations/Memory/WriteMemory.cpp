#include <Windows.h>
#include "../../Utils/logger.h"
#include "../../Utils/utilities.h"
#include "WriteMemory.h"

// prototype for NtWriteVirtualMemory
typedef NTSTATUS(NTAPI* PFN_NtWriteVirtualMemory)(
  HANDLE ProcessHandle,
  PVOID BaseAddress,
  PVOID Buffer,
  ULONG NumberOfBytesToWrite,
  PULONG NumberOfBytesWritten);

BOOL WPM(HANDLE process, LPVOID address, LPCVOID buffer, SIZE_T size, SIZE_T* written) {
  return WriteProcessMemory(process, address, buffer, size, written);
}
// NOTE passing the bytesWritten variable to NtWriteVirtualMemory will cause a stack corruption when compiled in Debug mode
BOOL NTWVM(HANDLE process, PVOID address, PVOID buffer, ULONG size, PULONG written) {
  HMODULE hNtdll = LoadLibraryA("ntdll.dll");
  if (!hNtdll) {
    return FALSE;
  }

  PFN_NtWriteVirtualMemory pNtWriteVirtualMemory = (PFN_NtWriteVirtualMemory) GetProcAddress(hNtdll, "NtWriteVirtualMemory");
  if (!pNtWriteVirtualMemory) {
    FreeLibrary(hNtdll);
    return FALSE;
  }

  NTSTATUS status = pNtWriteVirtualMemory(process, address, buffer, size, written);
  FreeLibrary(hNtdll);
  return NT_SUCCESS(status);
}

BOOL Write(DWORD pid, PVOID address, PVOID buffer, ULONG size, BYTE type) {
  Logger& logger = Logger::getInstance();

#ifdef DEBUG
  LOG_CALL("pid = %lu, address = %p, buffer = %p, size = %lu, type = %u", pid, address, buffer, size, type);
#endif

  SIZE_T bytesWritten = 0;
  ULONG bytesWrittenNt = 0;

  HANDLE processHandle = OpenProcess(PROCESS_VM_OPERATION | PROCESS_VM_WRITE, FALSE, pid);

  if (processHandle == NULL) {
    LOG_ERROR("Failed to open handle to process.");
    return FALSE;
  }

  BOOL success = TRUE;

  if (type & kWriteProcessMemory) {
    BOOL status = WPM(processHandle, (LPVOID) address, buffer, size, &bytesWritten);
    success &= status;

    if (status) {
      LOG_INFO("WriteProcessMemory call succeeded. Bytes written: %d.", bytesWritten);
    }
    else {
      LOG_ERROR("WriteProcessMemory returned false.");
    }
  }

  if (type & kNtWriteVirtualMemory) {
    BOOL status = NTWVM(processHandle, (PVOID) address, buffer, (ULONG) size, &bytesWrittenNt);
    success &= status;

    if (status) {
      LOG_INFO("NtWriteVirtualMemory call succeeded. Bytes written: %lu.", bytesWrittenNt);
    }
    else {
      LOG_ERROR("NtWriteVirtualMemory call failed.");
    }
  }

  CloseHandle(processHandle);
  return success;
}