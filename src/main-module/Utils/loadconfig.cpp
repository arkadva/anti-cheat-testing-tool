#include "loadconfig.h"
#include "../Base/Modules/module.h"

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

std::vector<Module*> ParseAttacks(YAML::Node config) {
  std::vector<Module*> modules;

  if (config["attacks"]) {
    YAML::Node attacks = config["attacks"];

    BYTE attack_index = 0;

    while (attack_index < UCHAR_MAX) {
      if (attacks[std::to_string(attack_index)]) {
        YAML::Node attack = attacks[std::to_string(attack_index)];
        modules.push_back(ModuleFactory::CreateAttack(attack["name"].as<std::string>(), attack["variables"]));
      }

      attack_index += 1;
    }
  }

  return modules;
}
