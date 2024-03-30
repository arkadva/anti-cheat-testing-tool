#ifndef MEMORYACCESS_H_
#define MEMORYACCESS_H_

#include <string>
#include "module.h"

#define NT_SUCCESS(Status) (((NTSTATUS)(Status)) >= 0)

class MemoryAccess : public Module {
public:
  MemoryAccess(LPVOID address) : address_(address) { }
  MemoryAccess() : address_(NULL) { }

  virtual bool execute(const Process* process) const = 0;

protected:
  LPVOID address_;
};

#endif
