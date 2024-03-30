#ifndef WRITEMEMORY_H_
#define WRITEMEMORY_H_

#include "memoryaccess.h"
#include <Windows.h>
#include <iostream>

BOOL WPM(HANDLE process, LPVOID address, LPCVOID buffer, SIZE_T size, SIZE_T* written);
BOOL NTWVM(HANDLE process, PVOID address, PVOID buffer, ULONG size, PULONG written);

// bitflags for write memory type
enum WriteMemoryType {
  kWriteProcessMemory = 0,
  kNtWriteVirtualMemory = 1 << 0
};

class WriteMemory : public MemoryAccess {
public:
  WriteMemory(LPVOID address, void* buffer, size_t size, BYTE type)
    : MemoryAccess(address), buffer_(buffer), size_(size), type_(type) { }

  virtual bool execute(const Process* process) const override {
    SIZE_T bytesWritten = 0;
    ULONG bytesWrittenNt = 0;

    HANDLE processHandle = OpenProcess(PROCESS_VM_WRITE | PROCESS_VM_OPERATION, FALSE, process->GetPid());
    if (processHandle == NULL) {
      std::cerr << "Failed to open process for PID: " << process->GetPid() << std::endl;
      return false;
    }

    bool success = false;

    if (type_ & kWriteProcessMemory) {
      success = WPM(processHandle, address_, buffer_, size_, &bytesWritten);
    }

    if (type_ & kNtWriteVirtualMemory) {
      success = NTWVM(processHandle, address_, buffer_, (ULONG)size_, &bytesWrittenNt);
    }

    std::cout << "Bytes written: " << bytesWritten << " Bytes written NT: " << bytesWrittenNt << std::endl;
    CloseHandle(processHandle);
    return success;
  }

private:
  void* buffer_;
  const size_t size_;
  const BYTE type_;
};
#endif
