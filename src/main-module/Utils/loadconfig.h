#ifndef LOADCONFIG_H_
#define LOADCONFIG_H_

#define YAML_CPP_STATIC_DEFINE

#include <yaml-cpp/yaml.h>
#include <Windows.h>
#include <iostream>
#include "../Base/modulefactory.h"
#include "../Utils/logger.h"

struct ProcessStartupInfo {
  std::wstring name;
  std::wstring working_directory;
  std::wstring args;
};

struct LogTypeInfo {
  UINT save_mask = 0;
  UINT display_mask = 0;
};

YAML::Node InitializeConfig();

ProcessStartupInfo GetProcessStartupInfo(YAML::Node config);

// log settings
LogTypeInfo GetLogSettings(YAML::Node config);
UINT CalculateLogSettings(YAML::Node config);
std::vector<Module*> ParseAttacks(YAML::Node config);

#endif
