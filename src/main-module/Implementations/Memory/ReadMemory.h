#ifndef READMEMORY_H_
#define READMEMORY_H_

#include <Windows.h>
#include <iostream>
#include "../../Base/memoryaccess.h"
#include "../../Base/module.h"

BOOL ReadMemoryImp(DWORD pid, PVOID address, PVOID buffer, ULONG size, BYTE type);

class ReadMemory : public Module {
public:
  ReadMemory(Variable* address, Variable* buffer, Variable* size, Variable* type) 
  : address_(address), buffer_(buffer), size_(size), type_(type) {
    module_name = "Read Memory";
  }

  virtual bool execute(const Process* process) const override {
    BOOL status = ReadMemoryImp(
      process->GetPid(),
      address_->as<PVOID>(),
      buffer_->variable_,
      size_->as<UINT>(),
      type_->as<BYTE>()
    );
    
    return status;
  }
  
private:
  Variable* address_;
  Variable* buffer_;
  Variable* size_;
  Variable* type_;
};
#endif
