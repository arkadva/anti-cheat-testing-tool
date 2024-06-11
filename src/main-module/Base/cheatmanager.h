#ifndef CHEATMANAGER_H_
#define CHEATMANAGER_H_

#include "process.h"
#include "../Utils/pipeclient.h"
#include "../Base/Modules/module.h"

class CheatManager {
public:
  CheatManager(const Process* process);
  CheatManager(const Process* process, const std::vector<Module*>* attacks);
  void execute() const;
  void AddAttacks(const std::vector<Module*>* attacks);
public:
  const Process* process_;
  PipeClient* client_;
  std::vector<Module*>* attacks_;
};

#endif
