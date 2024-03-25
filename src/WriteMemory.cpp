#include "WriteMemory.h"
#include <Windows.h>

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

BOOL NTWVM(HANDLE process, PVOID address, PVOID buffer, ULONG size, PULONG written) {
  HMODULE hNtdll = LoadLibraryA("ntdll.dll");
  if (!hNtdll) {
    return FALSE;
  }

  PFN_NtWriteVirtualMemory pNtWriteVirtualMemory = (PFN_NtWriteVirtualMemory)GetProcAddress(hNtdll, "NtWriteVirtualMemory");
  if (!pNtWriteVirtualMemory) {
    FreeLibrary(hNtdll);
    return FALSE;
  }

  NTSTATUS status = pNtWriteVirtualMemory(process, address, buffer, size, written);
  FreeLibrary(hNtdll);
  return NT_SUCCESS(status);
}
