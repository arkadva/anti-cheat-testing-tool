#include "./Base/cheatmanager.h"
#include "./Utils/implementation_imports.h"
#include "./Utils/logger.h"
#include "./Utils/pipeclient.h"
#include "./Utils/process.h"
#include "./Utils/debug.h"
#include "./Utils/utilities.h"
#include "./Utils/loadconfig.h"
#include "./Base/variable.h"
#include <cstdint>

int main() {
  ////// SETUP START
  YAML::Node config = InitializeConfig();
  // extract startup info
  ProcessStartupInfo psi = GetProcessStartupInfo(config);
  // extract the log settings
  LogTypeInfo lti = GetLogSettings(config);

  Logger& logger = Logger::getInstance(lti.display_mask, lti.save_mask);
  Process* process = new Process(psi.name, psi.args, psi.working_directory);
  CheatManager* cheatManager = new CheatManager(process);

  Sleep(1000); // no callback for process creation? :(

  LPVOID base_address_p = reinterpret_cast<LPVOID>(
    utilities::GetModuleBaseAddress(process->GetPid(),
    utilities::GetExecutableName(process->GetPath()).c_str())
  );

  Variable::NewVariable<LPVOID>("base_address", base_address_p);
  ////// SETUP END
  
  std::vector<Module*> modules = GetModules(config);

  for (Module* module : modules) {
    if (module != nullptr) {
      LOG_CONFIRMATION("Start");
      cheatManager->execute(module);
      LOG_CONFIRMATION("End");
    }
  }
  
  Variable::PrintVariables();
}
