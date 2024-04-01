#ifndef WRITEMEMORY_H_
#define WRITEMEMORY_H_

#include "memoryaccess.h"
#include <Windows.h>
#include <iostream>

BOOL Write(DWORD pid, PVOID address, PVOID buffer, ULONG size, BYTE type);

// bitflags for write memory type
enum WriteMemoryType {
  kWriteProcessMemory = 0,
  kNtWriteVirtualMemory = 1 << 0
};

class WriteMemory : public MemoryAccess {
public:
  WriteMemory(LPVOID address, void* buffer, ULONG size, BYTE type)
    : MemoryAccess(address), buffer_(buffer), size_(size), type_(type) {
    module_name = "Write Memory";
  }

  virtual bool execute(const Process* process) const override {
    BOOL status = Write(process->GetPid(), address_, buffer_, size_, type_);
    return status;
  }

private:
  void* buffer_;
  const ULONG size_;
  const BYTE type_;
};
#endif
