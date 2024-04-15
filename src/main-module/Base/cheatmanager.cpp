#include "../Base/Modules/module.h"
#include "../Utils/pipeclient.h"
#include "../Utils/logger.h"
#include "cheatmanager.h"

CheatManager::CheatManager(const Process* process)
  : process_(process),
    attacks_(new std::vector<Module*>()),
    client_(new PipeClient("\\\\.\\pipe\\actt-pipe")) {
}

CheatManager::CheatManager(const Process* process, const std::vector<Module*>* attacks)
  : process_(process),
    attacks_(new std::vector<Module*>(*attacks)),
    client_(new PipeClient("\\\\.\\pipe\\actt-pipe")) {
}

void CheatManager::execute() const {
  Logger& logger = Logger::getInstance();

  for (Module* module : *attacks_) {
    LOG_INFO("Executing module %s.", module->GetName().c_str());
    bool result = module->execute(process_);

    if (result) {
      LOG_INFO("Executing module %s succeeded.", module->GetName().c_str());
    }
    else {
      LOG_WARN("Executing module %s failed.", module->GetName().c_str());
    }
  }
}