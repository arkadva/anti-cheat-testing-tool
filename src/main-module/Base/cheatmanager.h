#ifndef CHEATMANAGER_H_
#define CHEATMANAGER_H_

#include <utility>
#include <map>
#include "process.h"
#include "../Utils/pipeclient.h"
#include "../Base/Modules/module.h"

enum Variable {
  kByte = 1 << 0,   // uint8_t
  kWord = 1 << 1,   // uint16_t
  kDword = 1 << 2,  // uint32_t
  kQword = 1 << 3,  // uint64_t
  kSigned = 1 << 4  // int_x_t
};

class CheatManager {
public:
  CheatManager(const Process* process);
  CheatManager(const Process* process, const std::vector<Module*>* attacks);
  void execute() const;
  void AddAttacks(const std::vector<Module*>* attacks);
  void ClearAttacks();

  // variables map
  static std::pair<void*, unsigned int> NewVariable(const std::string& name, const std::string& variable);
  static std::pair<void*, unsigned int> ReadVariable(const std::string& name);

public: // for now
  const Process* process_;
  PipeClient* client_;
  std::vector<Module*>* attacks_;
  static std::map<std::string, std::pair<void*, int>> variables_;
};

#endif
