#ifndef READMEMORY_H_
#define READMEMORY_H_

#include "memoryaccess.h"
#include <Windows.h>
#include <iostream>

BOOL RPM(HANDLE process, LPCVOID address, LPVOID buffer, SIZE_T size, SIZE_T* read);
BOOL NTRVM(HANDLE process, PVOID address, PVOID buffer, ULONG size, PULONG read);

// bitflags for read memory type
enum ReadMemoryType {
  kReadProcessMemory = 0x1,
  kNtReadVirtualMemory = 0x2
};

class ReadMemory : public MemoryAccess {
public:
  ReadMemory(uintptr_t address, void* buffer, size_t size, BYTE type)
    : MemoryAccess(address), buffer_(buffer), size_(size), type_(type) { }

  virtual bool execute(const Process* process) const override {
    SIZE_T bytesRead = 0;
    ULONG bytesReadNt = 0;

    HANDLE processHandle = OpenProcess(PROCESS_VM_READ, FALSE, process->GetPid());
    if (processHandle == NULL) {
      std::cerr << "Failed to open process for PID: " << process->GetPid() << std::endl;
      return false;
    }

    bool success = false;

    if (type_ & kReadProcessMemory) {
      success = RPM(processHandle, (LPCVOID)address_, buffer_, size_, &bytesRead);
    }

    if (type_ & kNtReadVirtualMemory) {
      success = NTRVM(processHandle, (PVOID)address_, buffer_, (ULONG)size_, &bytesReadNt);
    }

    std::cout << "Bytes read: " << bytesRead << " Bytes read NT: " << bytesReadNt << std::endl;
    CloseHandle(processHandle);
    return success;
  }

private:
  void* buffer_;
  const size_t size_;
  const BYTE type_;
};
#endif
