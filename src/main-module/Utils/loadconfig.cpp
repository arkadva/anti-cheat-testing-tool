#include "loadconfig.h"
#include "../Base/module.h"
#include "../Base/cheatmanager.h"
#include "../Utils/implementation_imports.h"

YAML::Node InitializeConfig(std::string config_name) {
  YAML::Node config;
  std::cout << "Attempting to load config file " << config_name << std::endl;

  try {
    config = YAML::LoadFile(config_name);
  }
  catch (...) {
    std::cout << "Failed to load the provided file. Please make sure the specified file exists and is a valid YAML format" << std::endl;
    getchar();
    exit(1);
  }

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
  Logger& logger = Logger::getInstance();

  if (!attack["name"]) { 
    LOG_ERROR("Module entry missing name.");
    return nullptr;
  }

  if (!attack["variables"]) {
    LOG_ERROR("Module entry missing variables.");
    return nullptr;
  }


  std::string attack_name = attack["name"].as<std::string>();
  YAML::Node variables = attack["variables"];

  //
  // ===============> MEMORY MODULES START
  //
  if (attack_name == "ReadOffset") {
    Variable* address = nullptr;
    Variable* buffer = nullptr;
    Variable* method = new Variable(kReadProcessMemory);
    std::vector<Variable*> offsets;

    if (variables["address"]) {
      address = InterpretVariable<PVOID>(variables["address"].as<std::string>());
    }

    else {
      LOG_ERROR("ReadOffset module is missing the `address` argument. Aborting.");
      return nullptr;
    }

    if (variables["save_to"]) {
      buffer = InterpretVariable<PVOID>(variables["save_to"].as<std::string>());
    }
    else {
      LOG_ERROR("ReadOffset module is missing the `save_to` argument. Aborting.");
      return nullptr;
    }

    if (variables["offsets"]) {
      offsets = InterpretList<uintptr_t>(variables["offsets"]);
    }
    else {
      LOG_ERROR("ReadOffset module is missing the `offsets` argument. Aborting.");
      return nullptr;
    }

    if (variables["method"]) {
      std::string methodStr = variables["method"].as<std::string>();

      if (methodStr == "ReadProcessMemory") {
        method->Set<int>(kReadProcessMemory);
      }
      else if (methodStr == "NtReadVirtualMemory") {
        method->Set<int>(kNtReadVirtualMemory);
      }
      else {
        LOG_WARN("`%s` is not a valid method for the ReadOffset module. Defaulting to `ReadProcessMemory`.", methodStr);
      }
    }
    else {
      LOG_WARN("ReadOffset module is missing the `method` argument. Defaulting to `ReadProcessMemory`.");
    }

    return new ReadOffset(address, buffer, new Variable(kReadProcessMemory), offsets);
  }

  if (attack_name == "ReadMemory") {
    Variable* address = nullptr;
    Variable* buffer = nullptr;
    size_t num_bytes = 4;
    Variable* method = new Variable(kReadProcessMemory);

    if (variables["address"]) {
      address = InterpretVariable<PVOID>(variables["address"].as<std::string>());
    }
    else {
      LOG_ERROR("ReadMemory module is missing the `address` argument. Aborting.");
      return nullptr;
    }

    if (variables["save_to"]) {
      buffer = InterpretVariable<PVOID>(variables["save_to"].as<std::string>());
    }
    else {
      LOG_ERROR("ReadMemory module is missing the `save_to` argument. Aborting.");
      return nullptr;
    }

    if (variables["type"]) {
      num_bytes = Variable::GetSizeOfType(variables["type"].as<std::string>());
    }
    else {
      LOG_WARN("ReadMemory module is missing the `type` argument. Defaulting to 4 bytes.");
    }

    if (variables["method"]) {
      std::string methodStr = variables["method"].as<std::string>();

      if (methodStr == "ReadProcessMemory") {
        method->Set<int>(kReadProcessMemory);
      }
      else if (methodStr == "NtReadVirtualMemory") {
        method->Set<int>(kNtReadVirtualMemory);
      }
      else {
        LOG_WARN("`%s` is not a valid method for the ReadMemory module. Defaulting to `ReadProcessMemory`.", methodStr);
      }
    }
    else {
      LOG_WARN("ReadMemory module is missing the `method` argument. Defaulting to `ReadProcessMemory`.");
    }

    return new ReadMemory(address, buffer, new Variable(num_bytes), method);
  }

  if (attack_name == "WriteMemory") {
    Variable* address = nullptr;
    Variable* value = nullptr;
    size_t num_bytes = 4;
    Variable* method = new Variable(kReadProcessMemory);

    if (variables["address"]) {
      address = InterpretVariable<PVOID>(variables["address"].as<std::string>());
    }
    else {
      LOG_ERROR("WriteMemory module is missing the `address` argument. Aborting.");
      return nullptr;
    }

    if (variables["value"]) {
      value = InterpretVariable<PVOID>(variables["value"].as<std::string>());
    }
    else {
      LOG_ERROR("WriteMemory module is missing the `value` argument. Aborting.");
      return nullptr;
    }

    if (variables["type"]) {
      num_bytes = Variable::GetSizeOfType(variables["type"].as<std::string>());
    }
    else {
      LOG_WARN("WriteMemory module is missing the `type` argument. Defaulting to 4 bytes.");
    }

    if (variables["method"]) {
      std::string methodStr = variables["method"].as<std::string>();

      if (methodStr == "WriteProcessMemory") {
        method->Set<int>(kWriteProcessMemory);
      }
      else if (methodStr == "NtWriteVirtualMemory") {
        method->Set<int>(kNtWriteVirtualMemory);
      }
      else {
        LOG_WARN("`%s` is not a valid method for the WriteMemory module. Defaulting to `WriteProcessMemory`.", methodStr);
      }
    }
    else {
      LOG_WARN("WriteMemory module is missing the `method` argument. Defaulting to `WriteProcessMemory`.");
    }

    return new WriteMemory(address, value, new Variable(num_bytes), method);
  }

  if (attack_name == "ExecHeap") {
    Variable* shellcode = new Variable(0);
    size_t size = 0;

    if (variables["shellcode"]) {
      std::string shellcodeStr = variables["shellcode"].as<std::string>();
      size = shellcodeStr.length() / 4;

      try {
        shellcode->Set<char*>(utilities::hexStringToBytes(shellcodeStr.c_str(), size));
      }
      catch (const std::exception& e) {
        LOG_ERROR("Failed to convert shellcode string to bytes: %s", e.what());
        return nullptr;
      }
    }
    else {
      LOG_ERROR("ExecHeap module is missing the `shellcode` argument. Aborting.");
      return nullptr;
    }

    return new ExecHeap(shellcode, size);
  }

  //
  // ===============> MEMORY MODULES END
  //
  
  //
  // ===============> DLL MODULES START
  //

  if (attack_name == "CreateRemoteThreadInjection") {
    std::wstring path;
    Variable* method = new Variable(kCreateRemoteThread);

    if (variables["path"]) {
      path = utilities::string_to_wstring(variables["path"].as<std::string>());
    }
    else {
      LOG_ERROR("CreateRemoteThreadInjection module is missing the `path` argument. Aborting.");
      return nullptr;
    }

    if (variables["method"]) {
      std::string methodStr = variables["method"].as<std::string>();

      if (methodStr == "NtCreateThreadEx") {
        method->Set<int>(kNtCreateThreadEx);
      }
      else if (methodStr == "RtlCreateUserThread") {
        method->Set<int>(kRtlCreateUserThread);
      }
      else if (methodStr == "CreateRemoteThread") {
        method->Set<int>(kCreateRemoteThread);
      }
      else {
        LOG_WARN("`%s` is not a valid method for the CreateRemoteThreadInjection module. Defaulting to `CreateRemoteThread`.", methodStr.c_str());
      }
    }
    else {
      LOG_WARN("CreateRemoteThreadInjection module is missing the `method` argument. Defaulting to `CreateRemoteThread`.");
    }

    wchar_t* wstr = static_cast<wchar_t*>(malloc(sizeof(wchar_t) * (path.length() + 1)));
    if (wstr == nullptr) {
      LOG_ERROR("Memory allocation for `wstr` failed. Aborting.");
      return nullptr;
    }

    wcscpy_s(wstr, path.length() + 1, path.c_str());

    return new CreateRemoteThreadInjection(new Variable(wstr), method);
  }

  if (attack_name == "ManualMapping") {
    std::wstring path;

    if (variables["path"]) {
      path = utilities::string_to_wstring(variables["path"].as<std::string>());
    }
    else {
      LOG_ERROR("ManualMapping module is missing the `path` argument. Aborting.");
      return nullptr;
    }

    wchar_t* wstr = static_cast<wchar_t*>(malloc(sizeof(wchar_t) * (path.length() + 1)));
    if (wstr == nullptr) {
      LOG_ERROR("Memory allocation for `wstr` failed. Aborting.");
      return nullptr;
    }

    wcscpy_s(wstr, path.length() + 1, path.c_str());

    return new ManualMapping(new Variable(wstr));
  }

  if (attack_name == "WindowsHooksInjection") {
    std::wstring path;

    if (variables["path"]) {
      path = utilities::string_to_wstring(variables["path"].as<std::string>());
    }
    else {
      LOG_ERROR("WindowsHooksInjection module is missing the `path` argument. Aborting.");
      return nullptr;
    }

    wchar_t* wstr = static_cast<wchar_t*>(malloc(sizeof(wchar_t) * (path.length() + 1)));
    if (wstr == nullptr) {
      LOG_ERROR("Memory allocation for `wstr` failed. Aborting.");
      return nullptr;
    }

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

    int breakpointType = kPageGuard;

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

    if (variables["type"]) {
      std::string typeStr = variables["type"].as<std::string>();

      if (typeStr == "PageGuard") {
        breakpointType = kPageGuard;
      }
      else if (typeStr == "HardwareBreakpoint") {
        breakpointType = kHardwareBreakpoint;
      }
      else {
        LOG_WARN("`%s` is not a valid type for the BreakpointHook module. Defaulting to `PageGuard`.", typeStr.c_str());
      }
    }
    else {
      LOG_WARN("BreakpointHook module is missing the `type` argument. Defaulting to `PageGuard`.");
    }

    entry.entriesSize = entryVec.size();
    _ContextChangeEntry* entryArr = static_cast<_ContextChangeEntry*>(malloc(sizeof(_ContextChangeEntry) * entryVec.size()));
    std::copy(entryVec.begin(), entryVec.end(), entryArr);

    return new BreakpointHook(CheatManager::getClient(), entry, entryArr, static_cast<HookType>(breakpointType));
  }

  //
  // ===============> HOOKS MODULES END
  //

  //
  // ===============> INPUT MODULES START
  //

  if (attack_name == "SetCursorPos") {
    Variable* cursor_x = new Variable(0);
    Variable* cursor_y = new Variable(0);

    if (variables["cursor_x"]) {
      cursor_x = InterpretVariable<int>(variables["cursor_x"].as<std::string>());
    }
    else {
      LOG_WARN("SetCursorPos module is missing the `cursor_x` argument. Defaulting to 0.");
    }

    if (variables["cursor_y"]) {
      cursor_y = InterpretVariable<int>(variables["cursor_y"].as<std::string>());
    }
    else {
      LOG_WARN("SetCursorPos module is missing the `cursor_y` argument. Defaulting to 0.");
    }

    InputSynthesizerInformation* isi = new InputSynthesizerInformation();
    isi->x = cursor_x;
    isi->y = cursor_y;

    InputSynthesizerType ist = kSetCursor;

    return new InputSynthesizer(isi, ist);
  }

  if (attack_name == "MouseEvent") {
    int mouse_event = 0;

    if (variables["event"]) {
      std::string eventStr = variables["event"].as<std::string>();

      mouse_event = utilities::GetMouseEvent(eventStr);

      if (mouse_event == -1) {
        LOG_ERROR("Invalid value for `event` argument in MouseEvent. Aborting.");
        return nullptr;
      }
    }
    else {
      LOG_ERROR("MouseEvent module is missing the `event` argument. Aborting.");
      return nullptr;
    }

    INPUT* in = new INPUT();
    in->mi.dwFlags = mouse_event;

    InputSynthesizerInformation* isi = new InputSynthesizerInformation();
    isi->input = in;

    InputSynthesizerType ist = kMouseEvent;

    return new InputSynthesizer(isi, ist);
  }

  if (attack_name == "KeyboardEvent") {
    int keyboard_event = 0;
    int key = 0;

    if (variables["event"]) {
      std::string eventStr = variables["event"].as<std::string>();

      if (eventStr == "KEYEVENTF_KEYUP") {
        keyboard_event = KEYEVENTF_KEYUP;
      }
      else if (eventStr != "KEYEVENTF_KEYDOWN") {
        LOG_ERROR("%s is not a valid `event` argument for KeyboardEvent. Aborting.", eventStr);
        return nullptr;
      }
    }
    else {
      LOG_WARN("KeyboardEvent module is missing the `event` argument. Defaulting to KEYEVENTF_KEYDOWN.");
    }

    if (variables["key"]) {
      key = InterpretVariable<int>(variables["key"].as<std::string>())->as<int>();
    } else {
      LOG_WARN("KeyboardEvent module is missing the `key` argument. Aborting.");
      return nullptr;
    }

    INPUT* in = new INPUT();
    in->ki.dwFlags = keyboard_event;
    in->ki.wVk = key;

    InputSynthesizerInformation* isi = new InputSynthesizerInformation();
    isi->input = in;

    InputSynthesizerType ist = kKeyboardEvent;

    return new InputSynthesizer(isi, ist);
  }

  if (attack_name == "SendInput") {
    std::string input_type;

    if (variables["type"]) {
      input_type = variables["type"].as<std::string>();
    }
    else {
      LOG_ERROR("SendInput module is missing the `type` argument. Aborting.");
      return nullptr;
    }

    if (input_type == "INPUT_KEYBOARD") {
      int keyboard_event = 0;
      int key = 0;

      if (variables["event"]) {
        std::string eventStr = variables["event"].as<std::string>();

        if (eventStr == "KEYEVENTF_KEYUP") {
          keyboard_event = KEYEVENTF_KEYUP;
        }
        else if (eventStr != "KEYEVENTF_KEYDOWN") {
          LOG_ERROR("%s is not a valid `event` argument for KeyboardEvent. Aborting.", eventStr);
          return nullptr;
        }
      }
      else {
        LOG_WARN("SendInput module is missing the `event` argument for keyboard input. Defaulting to KEYEVENTF_KEYDOWN.");
      }

      if (variables["key"]) {
        key = InterpretVariable<int>(variables["key"].as<std::string>())->as<int>();
      }
      else {
        LOG_ERROR("SendInput module is missing the `key` argument for keyboard input. Aborting.");
        return nullptr;
      }

      INPUT* in = new INPUT();
      in->type = INPUT_KEYBOARD;
      in->ki.dwFlags = keyboard_event;
      in->ki.wVk = key;

      InputSynthesizerInformation* isi = new InputSynthesizerInformation();
      isi->input = in;

      InputSynthesizerType ist = kSendInput;

      return new InputSynthesizer(isi, ist);
    }
    else if (input_type == "INPUT_MOUSE") {
      int mouse_event = 0;

      if (variables["event"]) {
        std::string eventStr = variables["event"].as<std::string>();

        mouse_event = utilities::GetMouseEvent(eventStr);

        if (mouse_event == -1) {
          LOG_ERROR("Invalid value for `event` argument in MouseEvent. Aborting.");
          return nullptr;
        }
      }
      else {
        LOG_ERROR("SendInput module is missing the `event` argument for mouse input. Aborting.");
        return nullptr;
      }

      INPUT* in = new INPUT();
      in->type = INPUT_MOUSE;
      in->mi.dwFlags = mouse_event;

      InputSynthesizerInformation* isi = new InputSynthesizerInformation();
      isi->input = in;

      InputSynthesizerType ist = kSendInput;

      return new InputSynthesizer(isi, ist);
    }
    else {
      LOG_ERROR("Invalid `type` argument for SendInput. Aborting.");
      return nullptr;
    }
  }


  //
  // ===============> INPUT MODULES END
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
