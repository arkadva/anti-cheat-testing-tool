#ifndef READMEMORY_H_
#define READMEMORY_H_

#include <Windows.h>
#include <iostream>
#include "../../Base/Modules/memoryaccess.h"

BOOL Read(DWORD pid, PVOID address, PVOID buffer, ULONG size, BYTE type);

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
