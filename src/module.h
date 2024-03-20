#ifndef MODULE_H_
#define MODULE_H_

#include <Windows.h>
#include "process.h"

class Module {
public:
  virtual ~Module() = default;
  virtual bool execute(const Process* process) const = 0;
};

#endif
