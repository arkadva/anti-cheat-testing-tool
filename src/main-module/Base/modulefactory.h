#ifndef MODULEFACTORY_H_
#define MODULEFACTORY_H_

#include "./Modules/module.h"
#include "../Utils/loadconfig.h"

class ModuleFactory {
public:
  static Module* CreateAttack(const std::string& type, const YAML::Node& variables);
  static void* InterpretValue(const std::string& str);

  template <typename T>
  static std::vector<T> InterpretList(const YAML::Node& node);
};


#endif
