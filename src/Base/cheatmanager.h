#ifndef CHEATMANAGER_H_
#define CHEATMANAGER_H_

#include "process.h"
#include "../Base/Modules/module.h"

class CheatManager {
public:
  CheatManager(const Process* process);
  CheatManager(const Process* process, const std::vector<Module*>* attacks);
  void execute() const;

private:
  const Process* _process;
  std::vector<Module*>* _attacks;
};

#endif
