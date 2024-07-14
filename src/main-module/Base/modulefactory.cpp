#include "modulefactory.h"
#include "../Utils/implementation_imports.h"
#include "../Base/cheatmanager.h"
#include <regex>

Module* ModuleFactory::CreateAttack(const std::string& type, const YAML::Node& variables) {
  if (type == "ReadOffset") {
    std::string save_to = variables["save_to"].as<std::string>();
    std::string str_address = variables["address"].as<std::string>();

    std::vector<uintptr_t> offsets = InterpretList<uintptr_t>(variables["offsets"]);
    DWORD address = reinterpret_cast<DWORD>(InterpretValue(str_address));

    std::pair<void*, unsigned int> variable = CheatManager::NewVariable(type, "uint32_t");
    ULONG size = variable.second & (~kSigned);

    return new ReadOffset(reinterpret_cast<LPVOID>(*reinterpret_cast<PDWORD>(address)), variable.first, kReadProcessMemory, offsets);
  }

  else if (type == "ReadMemory") {
    std::string str_address = variables["address"].as<std::string>();
    std::string save_to = variables["save_to"].as<std::string>();
    std::string type = variables["type"].as<std::string>();
    
    DWORD address = reinterpret_cast<DWORD>(InterpretValue(str_address));

    std::pair<void*, int> variable = CheatManager::NewVariable(save_to, type);
    ULONG size = variable.second & (~kSigned);

    return new ReadMemory(reinterpret_cast<LPVOID>(address), variable.first, size, kReadProcessMemory);
  }

  else if (type == "WriteMemory") {
    std::string str_address = variables["address"].as<std::string>();
    std::string save_to = variables["save_to"].as<std::string>();
    std::string type = variables["type"].as<std::string>();

    DWORD address = reinterpret_cast<DWORD>(InterpretValue(str_address));

    std::pair<void*, int> variable = CheatManager::NewVariable(save_to, type);
    ULONG size = variable.second & (~kSigned);

    return new WriteMemory(reinterpret_cast<LPVOID>(address), variable.first, size, kReadProcessMemory);
  }

  return nullptr;
}

void* ModuleFactory::InterpretValue(const std::string& str)
{
  std::regex hex_regex("^0[xX][0-9a-fA-F]+$");

  DWORD address;
  if (std::regex_match(str, hex_regex)) {
    address = std::stoul(str, nullptr, 16);
  }
  else {
    address = reinterpret_cast<DWORD>(CheatManager::ReadVariable(str).first);
  }

  return reinterpret_cast<void*>(address);
}

template<typename T>
static std::vector<T> ModuleFactory::InterpretList(const YAML::Node& node) {
  std::vector<T> values;
  for (const auto& value : node) {
    values.push_back(InterpretValue<T>(value));
  }

  return values;
}
