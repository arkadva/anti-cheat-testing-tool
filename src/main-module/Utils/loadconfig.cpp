#include "loadconfig.h"
#include "../Base/module.h"
#include "../Base/cheatmanager.h"
#include "../Utils/implementation_imports.h"

YAML::Node InitializeConfig() {
  YAML::Node config = YAML::LoadFile("D:\\Programming\\anti-cheat-testing-tool\\Release\\config.yaml");
  std::cout << "Loaded config file" << std::endl;
  return config;
}

ProcessStartupInfo GetProcessStartupInfo(YAML::Node config) {
  ProcessStartupInfo psi;

  if (config["process"]) {
    YAML::Node process = config["process"];

    if (process["name"]) {
      psi.name = utilities::string_to_wstring(process["name"].as<std::string>());
    }

    if (process["working_directory"]) {
      psi.working_directory = utilities::string_to_wstring(process["working_directory"].as<std::string>());
    }

    if (process["args"]) {
      YAML::Node args_node = process["args"];
      BYTE args_index = 0;

      std::string args;
      while (args_index < UCHAR_MAX) {
        if (args_node[std::to_string(args_index)]) {
          args += " " + args_node[std::to_string(args_index)].as<std::string>();
        }

        args_index += 1;
      }

      psi.args = utilities::string_to_wstring(args);
    }
  }

  return psi;
}

LogTypeInfo GetLogSettings(YAML::Node config) {
  LogTypeInfo lti;

  if (config["log_settings"]) {
    if (config["log_settings"]["display"]) {
      bool is_display = config["log_settings"]["display"].as<bool>();

      if (is_display && config["log_settings"]["display_settings"]) {
        YAML::Node display_settings = config["log_settings"]["display_settings"];
        lti.display_mask = CalculateLogSettings(display_settings);
      }
    }

    if (config["log_settings"]["save"]) {
      bool is_save = config["log_settings"]["save"].as<bool>();

      if (is_save && config["log_settings"]["save_settings"]) {
        YAML::Node save_settings = config["log_settings"]["save_settings"];
        lti.save_mask = CalculateLogSettings(save_settings);
      }
    }
  }

  return lti;
}

UINT CalculateLogSettings(YAML::Node config) {
  BYTE settings_index = 0;
  UINT logmask = 0;

  while (settings_index < UCHAR_MAX) {
    if (config[std::to_string(settings_index)]) {
      std::string setting = config[std::to_string(settings_index)].as<std::string>();

      std::unordered_map<std::string, LogType> map = {
        {"info", kInfo},
        {"warning", kWarning},
        {"error", kError},
        {"confirmation", kConfirmation}
      };

      try {
        logmask |= map.at(setting);
      }
      catch (const std::out_of_range&) { }

      settings_index += 1;
    }
    else {
      break;
    }
  }

  return logmask;
}

UINT GetModuleCount(YAML::Node config) {
  BYTE attack_index = 0;

  if (config["attacks"]) {
    YAML::Node attacks = config["attacks"];

    while (attack_index < UCHAR_MAX) {
      if (attacks[std::to_string(attack_index)]) {
        attack_index += 1;
      }
      else {
        break;
      }
    }
  }

  return attack_index;
}

template<typename T>
static std::vector<Variable*> InterpretList(const YAML::Node& node) {
  std::vector<Variable*> values;

  for (const auto& value : node) {
    values.push_back(InterpretVariable<T>(value.as<std::string>()));
  }

  return values;
}

template<typename T>
Variable* InterpretVariable(std::string variable) {
  if (variable.empty()) {
    return nullptr;
  }

  bool isNumber = true;
  for (char c : variable) {
    if (!isdigit(c)) {
      isNumber = false;
      break;
    }
  }
  if (isNumber) {
    long value = std::stol(variable);
    return new Variable(value);
  }

  if (variable.length() > 2 && variable[0] == '0' && (variable[1] == 'x' || variable[1] == 'X')) {
    std::istringstream iss(variable);
    T value;
    iss >> std::hex >> value;

    if (!iss.fail()) {
      return new Variable(value);
    }
  }

  if (isalpha(variable[0])) {
    return Variable::GetVariable<T>(variable);
  }

  return nullptr;
}

Module* InterpretModule(YAML::Node attack) {
  if (!attack["name"]) return nullptr;
  if (!attack["variables"]) return nullptr;


  std::string attack_name = attack["name"].as<std::string>();
  YAML::Node variables = attack["variables"];

  //
  // ===============> MEMORY MODULES START
  //
  if (attack_name == "ReadOffset") {
    Variable* address = nullptr; Variable* buffer = nullptr; std::vector<Variable*> offsets;

    if (variables["address"]) {
      address = InterpretVariable<PVOID>(variables["address"].as<std::string>());
    }

    if (variables["save_to"]) {
      buffer = InterpretVariable<PVOID>(variables["save_to"].as<std::string>());
    }

    if (variables["offsets"]) {
      offsets = InterpretList<uintptr_t>(variables["offsets"]);
    }

    return new ReadOffset(address, buffer, new Variable(kReadProcessMemory), offsets);
  }

  if (attack_name == "ReadMemory") {
    Variable* address = nullptr; Variable* buffer = nullptr; size_t num_byters = 4;

    if (variables["address"]) {
      address = InterpretVariable<PVOID>(variables["address"].as<std::string>());
    }

    if (variables["save_to"]) {
      buffer = InterpretVariable<PVOID>(variables["save_to"].as<std::string>());
    }

    if (variables["type"]) {
      num_byters = Variable::GetSizeOfType(variables["type"].as<std::string>());
    }

    return new ReadMemory(address, buffer, new Variable(num_byters), new Variable(kReadProcessMemory));
  }

  if (attack_name == "WriteMemory") {
    Variable* address = nullptr; Variable* value = nullptr; size_t num_byters = 4;

    if (variables["address"]) {
      address = InterpretVariable<PVOID>(variables["address"].as<std::string>());
    }

    if (variables["value"]) {
      value = InterpretVariable<PVOID>(variables["value"].as<std::string>());
    }

    if (variables["type"]) {
      num_byters = Variable::GetSizeOfType(variables["type"].as<std::string>());
    }

    return new WriteMemory(address, value, new Variable(num_byters), new Variable(kReadProcessMemory));
  }
  //
  // ===============> MEMORY MODULES END
  //
  
  //
  // ===============> DLL MODULES START
  //

  if (attack_name == "CreateRemoteThreadInjection") {
    std::wstring path;
    
    if (variables["path"]) {
      path = utilities::string_to_wstring((variables["path"].as<std::string>()));
    }

    wchar_t* wstr = static_cast<wchar_t*>(malloc(sizeof(wchar_t) * (wcslen(path.c_str() + 1))));
    wcscpy_s(wstr, path.length() + 1, path.c_str());

    return new CreateRemoteThreadInjection(new Variable(wstr), new Variable(kCreateRemoteThread));
  }

  if (attack_name == "ManualMapping") {
    std::wstring path;

    if (variables["path"]) {
      path = utilities::string_to_wstring((variables["path"].as<std::string>()));
    }

    wchar_t* wstr = static_cast<wchar_t*>(malloc(sizeof(wchar_t) * (wcslen(path.c_str() + 1))));
    wcscpy_s(wstr, path.length() + 1, path.c_str());

    return new ManualMapping(new Variable(wstr));
  }

  if (attack_name == "WindowsHooksInjection") {
    std::wstring path;

    if (variables["path"]) {
      path = utilities::string_to_wstring((variables["path"].as<std::string>()));
    }

    wchar_t* wstr = static_cast<wchar_t*>(malloc(sizeof(wchar_t) * (wcslen(path.c_str() + 1))));
    wcscpy_s(wstr, path.length() + 1, path.c_str());

    return new WindowsHooksInjection(new Variable(wstr));
  }

  //
  // ===============> DLL MODULES END
  //

  //
  // ===============> HOOKS MODULES START
  //

  if (attack_name == "BreakpointHook") {
    Variable* address;

    _BreakpointHookData entry;
    std::vector<_ContextChangeEntry> entryVec;

    if (variables["address"]) {
      address = InterpretVariable<PVOID>(variables["address"].as<std::string>());
      entry.address = address;
    }

    if (variables["context"]) {
      UINT contextIndex = 0;

      while (contextIndex <= UCHAR_MAX) {
        YAML::Node contextNode = variables["context"];

        if (variables["context"][std::to_string(contextIndex)]) {
          YAML::Node contextEntry = contextNode[std::to_string(contextIndex)];

          std::string reg; int value = -1;

          if (contextEntry["register"]) {
            reg = contextEntry["register"].as<std::string>();
          }

          if (contextEntry["value"]) {
            value = contextEntry["value"].as<int>();
          }

          UINT offset = utilities::GetContextOffset(reg);

          _ContextChangeEntry* newEntryData = new _ContextChangeEntry(); //{ reinterpret_cast<void*>(value), offset };
          newEntryData->value = new Variable(value);
          newEntryData->offset = offset;

          entryVec.push_back(*newEntryData);
        }
        else {
          break;
        }

        contextIndex += 1;
      }
      
    }

    entry.entriesSize = entryVec.size();
    _ContextChangeEntry* entryArr = static_cast<_ContextChangeEntry*>(malloc(sizeof(_ContextChangeEntry) * entryVec.size()));
    std::copy(entryVec.begin(), entryVec.end(), entryArr);

    return new BreakpointHook(CheatManager::getClient(), entry, entryArr, kPageGuard);
  }

  //
  // ===============> HOOKS MODULES END
  //

  return nullptr;
}

std::vector<Module*> GetModules(YAML::Node config) {
  std::vector<Module*> modules;

  BYTE attack_index = 0;
  UINT attack_count = GetModuleCount(config);

  YAML::Node attacks = config["attacks"];

  for (UINT i = 0; i < attack_count; i++) {
    YAML::Node attack = attacks[std::to_string(i)];
    modules.push_back(InterpretModule(attack));
  }

  return modules;
}
