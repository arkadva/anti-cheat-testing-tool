#ifndef MEMORYACCESS_H_
#define MEMORYACCESS_H_

#include <string>
#include "module.h"

#define NT_SUCCESS(Status) (((NTSTATUS)(Status)) >= 0)

enum ReadMemoryType {
  kReadProcessMemory = 1 << 0,
  kNtReadVirtualMemory = 1 << 1
};

// prototype for NtReadVirtualMemory
typedef NTSTATUS(NTAPI* PFN_NtReadVirtualMemory)(HANDLE ProcessHandle, PVOID BaseAddress, PVOID Buffer, ULONG NumberOfBytesToRead, PULONG NumberOfBytesRead);

BOOL RPM(HANDLE process, LPCVOID address, LPVOID buffer, SIZE_T size, SIZE_T* read);

// NOTE passing the bytesRead variable to NtReadVirtualMemory will cause a stack corruption when compiled in Debug mode
BOOL NTRVM(HANDLE process, PVOID address, PVOID buffer, ULONG size, PULONG read);

class MemoryAccess : public Module {
public:
  MemoryAccess(LPVOID address) : address_(address) { }

  virtual bool execute(const Process* process) const = 0;

protected:
  const LPVOID address_;
};

#endif
