#include "../Base/module.h"
#include "../Utils/pipeclient.h"
#include "../Utils/logger.h"
#include "cheatmanager.h"

PipeClient* CheatManager::client_ = nullptr;

CheatManager::CheatManager(const Process* process) : process_(process) {
  client_ = new PipeClient("\\\\.\\pipe\\actt-pipe");
}

void CheatManager::execute(Module* module) const {
  Logger& logger = Logger::getInstance();

  if (module != nullptr) {
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

PipeClient* CheatManager::getClient() {
  return client_;
}
