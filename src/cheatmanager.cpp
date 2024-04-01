#include "module.h"
#include "cheatmanager.h"
#include "logger.h"

CheatManager::CheatManager(const Process* process)
  : _process(process),
    _attacks(new std::vector<Module*>()) {
}

CheatManager::CheatManager(const Process* process, const std::vector<Module*>* attacks)
  : _process(process),
    _attacks(new std::vector<Module*>(*attacks)) {
}

void CheatManager::execute() const {
  Logger& logger = Logger::getInstance();

  for (Module* module : *_attacks) {
    LOG_INFO("Executing %s.", module->GetName());
    bool result = module->execute(_process);

    if (result) {
      LOG_INFO("Executing %s succeeded.", module->GetName());
    }
    else {
      LOG_ERROR("Executing %s failed.", module->GetName());
    }
  }
}