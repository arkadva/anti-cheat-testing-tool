#include "module.h"
#include "cheatmanager.h"

CheatManager::CheatManager(const Process* process)
  : _process(process),
    _attacks(new std::vector<Module*>()) {
}

CheatManager::CheatManager(const Process* process, const std::vector<Module*>* attacks)
  : _process(process),
    _attacks(new std::vector<Module*>(*attacks)) {
}

void CheatManager::execute() const {
  for (Module* module : *_attacks) {
    module->execute(_process);
  }
}