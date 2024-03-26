#ifndef READMEMORY_H_
#define READMEMORY_H_

#include "memoryaccess.h"
#include <Windows.h>
#include <iostream>

// NOTE passing the bytesRead variable to NtReadVirtualMemory will cause
// a stack corruption when compiled in Debug mode
BOOL NTRVM(HANDLE process, PVOID address, PVOID buffer, ULONG size, PULONG read);
BOOL RPM(HANDLE process, LPCVOID address, LPVOID buffer, SIZE_T size, SIZE_T* read);

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

    bool success = true;

    /*
    * return false if either failed
    * a good logging class could come in handy
    */
    if (type_ & kReadProcessMemory) {
      BOOL status = RPM(processHandle, (LPCVOID)address_, buffer_, size_, &bytesRead);
      success &= status;
    }

    if (type_ & kNtReadVirtualMemory) {
      BOOL status = NTRVM(processHandle, (PVOID)address_, buffer_, (ULONG)size_, &bytesReadNt);
      success &= status;
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
