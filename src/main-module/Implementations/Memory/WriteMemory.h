#ifndef WRITEMEMORY_H_
#define WRITEMEMORY_H_

#include <Windows.h>
#include <iostream>
#include "../../Base/module.h"
#include "../../Base/memoryaccess.h"

BOOL WriteImp(DWORD pid, PVOID address, PVOID buffer, ULONG size, BYTE type);

class WriteMemory : public Module {
public:
  WriteMemory(Variable* address, Variable* buffer, Variable* size, Variable* type) 
  : address_(address), buffer_(buffer), size_(size), type_(type) {
    module_name = "Write Memory";
  }

  virtual bool execute(const Process* process) const override {
    BOOL status = WriteImp(
      process->GetPid(),
      address_->as<PVOID>(),
      buffer_->variable_,
      size_->as<ULONG>(),
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
