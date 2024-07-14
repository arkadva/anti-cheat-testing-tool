#include "../Base/Modules/module.h"
#include "../Utils/pipeclient.h"
#include "../Utils/logger.h"
#include "cheatmanager.h"

std::map<std::string, std::pair<void*, int>> CheatManager::variables_;

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
}

void CheatManager::AddAttacks(const std::vector<Module*>* attacks) {
  attacks_ = new std::vector<Module*>(*attacks);
}

void CheatManager::ClearAttacks() {
  delete attacks_;
}

std::pair<void*, unsigned int> CheatManager::NewVariable(const std::string& name, const std::string& variable) {
  void* ptr = nullptr;
  unsigned int type_flag = 0;

  if (variable == "uint8_t") {
    ptr = new uint8_t;
    type_flag = Variable::kByte;
  }
  else if (variable == "uint16_t") {
    ptr = new uint16_t;
    type_flag = Variable::kWord;
  }
  else if (variable == "uint32_t") {
    ptr = new uint32_t;
    type_flag = Variable::kDword;
  }
  else if (variable == "uint64_t") {
    ptr = new uint64_t;
    type_flag = Variable::kQword;
  }
  else if (variable == "int8_t") {
    ptr = new int8_t;
    type_flag = Variable::kByte | Variable::kSigned;
  }
  else if (variable == "int16_t") {
    ptr = new int16_t;
    type_flag = Variable::kWord | Variable::kSigned;
  }
  else if (variable == "int32_t") {
    ptr = new int32_t;
    type_flag = Variable::kDword | Variable::kSigned;
  }
  else if (variable == "int64_t") {
    ptr = new int64_t;
    type_flag = Variable::kQword | Variable::kSigned;
  }

  variables_[name] = std::make_pair(ptr, type_flag);

  return variables_[name];
}

std::pair<void*, unsigned int> CheatManager::ReadVariable(const std::string& name) {
  if (variables_.find(name) != variables_.end()) {
    return variables_[name];
  }
  else {
    return std::make_pair(nullptr, 0);
  }
}