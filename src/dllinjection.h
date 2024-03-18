#ifndef DLL_MODULE_H_
#define DLL_MODULE_H_

#include <string>
#include "module.h"

class DLLInjection : public Module {
public:
  DLLInjection(const std::wstring& path) : path(path) { }
  virtual bool execute(const Process* process) const = 0;

protected:
  const std::wstring& path;
};

#endif