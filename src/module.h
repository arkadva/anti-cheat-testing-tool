#ifndef MODULE_H_
#define MODULE_H_

#include <Windows.h>
#include "process.h"

class Module {
public:
  virtual ~Module() = default;
  virtual bool execute(const Process* process) const = 0;
  const std::string GetName() { return module_name; }
protected:
  std::string module_name = "Module";
};

#endif
