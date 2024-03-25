#include "ReadMemory.h"
#include <Windows.h>

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

BOOL NTRVM(HANDLE process, PVOID address, PVOID buffer, ULONG size, PULONG read) {
  HMODULE hNtdll = LoadLibraryA("ntdll.dll");
  if (!hNtdll) {
    return FALSE;
  }

  PFN_NtReadVirtualMemory pNtReadVirtualMemory = (PFN_NtReadVirtualMemory)GetProcAddress(hNtdll, "NtReadVirtualMemory");
  if (!pNtReadVirtualMemory) {
    FreeLibrary(hNtdll);
    return FALSE;
  }

  NTSTATUS status = pNtReadVirtualMemory(process, address, buffer, size, read);
  FreeLibrary(hNtdll);
  return NT_SUCCESS(status);
}
