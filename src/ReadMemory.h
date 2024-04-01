#ifndef READMEMORY_H_
#define READMEMORY_H_

#include "memoryaccess.h"
#include <Windows.h>
#include <iostream>

BOOL Read(DWORD pid, PVOID address, PVOID buffer, ULONG size, BYTE type);

// bitflags for read memory type
enum ReadMemoryType {
  kReadProcessMemory = 0,
  kNtReadVirtualMemory = 1 << 0
};

class ReadMemory : public MemoryAccess {
public:
  ReadMemory(LPVOID address, void* buffer, ULONG size, BYTE type)
    : MemoryAccess(address), buffer_(buffer), size_(size), type_(type) { 
    module_name = "Read Memory";
  }

  virtual bool execute(const Process* process) const override {
    BOOL status = Read(process->GetPid(), address_, buffer_, size_, type_);
    return status;
  }
  
private:
  void* buffer_;
  const ULONG size_;
  const BYTE type_;
};
#endif
