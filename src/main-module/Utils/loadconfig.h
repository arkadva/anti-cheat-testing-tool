#ifndef LOADCONFIG_H_
#define LOADCONFIG_H_

#define YAML_CPP_STATIC_DEFINE

#include <yaml-cpp/yaml.h>
#include <Windows.h>
#include <iostream>
#include "../Utils/logger.h"
#include "../Base/module.h"

struct ProcessStartupInfo {
  std::wstring name;
  std::wstring working_directory;
  std::wstring args;
};

struct LogTypeInfo {
  UINT save_mask = 0;
  UINT display_mask = 0;
};

YAML::Node InitializeConfig(std::string);

ProcessStartupInfo GetProcessStartupInfo(YAML::Node config);

template<typename T>
Variable* InterpretVariable(std::string variable);

template<typename T>
static std::vector<Variable*> InterpretList(const YAML::Node& node);

template<typename T>
Module* InterpretModule(YAML::Node config);
std::vector<Module*> GetModules(YAML::Node config);

// log settings
LogTypeInfo GetLogSettings(YAML::Node config);
UINT CalculateLogSettings(YAML::Node config);
UINT GetModuleCount(YAML::Node config);

#endif
