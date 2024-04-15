#ifndef FUNCHOOK_H_
#define FUNCHOOK_H_

#include <string>
#include "module.h"

class FuncHook : public Module {
public:
  FuncHook(LPVOID address) : address_(address) { }

  virtual bool execute(const Process* process) const = 0;

protected:
  const LPVOID address_;
};

#endif
