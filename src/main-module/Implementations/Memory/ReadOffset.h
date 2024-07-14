#ifndef READOFFSET_H_
#define READOFFSET_H_

#include <Windows.h>
#include <iostream>
#include "../../Base/module.h"
#include "../../Base/memoryaccess.h"

BOOL ReadOffsetImp(DWORD pid, PVOID address, PVOID buffer, BYTE type, std::vector<uintptr_t> offsets);

class ReadOffset : public Module {
public:
  ReadOffset(Variable* address, Variable* buffer, Variable* type, std::vector<Variable*> offsets)
  : address_(address), buffer_(buffer), type_(type), offsets_(offsets) {
    module_name = "Read Memory Offset";
  }

  virtual bool execute(const Process* process) const override {
    std::vector<uintptr_t> interpreted_offsets;

    for (const auto& offset : offsets_) {
      interpreted_offsets.push_back(offset->as<uintptr_t>());
    }

    BOOL status = ReadOffsetImp(
      process->GetPid(),
      address_->as<PVOID>(),
      buffer_->variable_,
      type_->as<BYTE>(),
      interpreted_offsets
    );

    return status;
  }

private:
  Variable* address_;
  Variable* buffer_;
  Variable* type_;
  std::vector<Variable*> offsets_;
};

#endif
