#ifndef WRITEMEMORY_H_
#define WRITEMEMORY_H_

#include <Windows.h>
#include <iostream>
#include "../../Base/Modules/memoryaccess.h"

BOOL Write(DWORD pid, PVOID address, PVOID buffer, ULONG size, BYTE type);

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
