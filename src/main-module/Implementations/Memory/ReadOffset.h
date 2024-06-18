#ifndef READOFFSET_H_
#define READOFFSET_H_

#include <Windows.h>
#include <iostream>
#include "../../Base/Modules/memoryaccess.h"

BOOL ReadOffsetImp(DWORD pid, PVOID address, PVOID buffer, BYTE type, std::vector<uintptr_t> offsets);

class ReadOffset : public MemoryAccess {
public:
  ReadOffset(LPVOID address, void* buffer, ReadMemoryType type, std::vector<uintptr_t> offsets)
    : MemoryAccess(address), buffer_(buffer), type_(type), offsets_(offsets) {
    module_name = "Read Memory Offset";
  }

  virtual bool execute(const Process* process) const override {
    BOOL status = ReadOffsetImp(process->GetPid(), address_, buffer_, type_, offsets_);
    return status;
  }

private:
  void* buffer_;
  const ReadMemoryType type_;
  std::vector<uintptr_t> offsets_;
};

#endif
