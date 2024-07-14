#ifndef CHEATMANAGER_H_
#define CHEATMANAGER_H_

#include <utility>
#include <map>
#include "process.h"
#include "../Utils/pipeclient.h"
#include "../Base/module.h"

class PipeClient; // Forward declaration

class CheatManager {
public:
  CheatManager(const Process* process);
  void execute(Module* module) const;

  static PipeClient* getClient();

public: // for now
  const Process* process_;
  static PipeClient* client_;
};

#endif
