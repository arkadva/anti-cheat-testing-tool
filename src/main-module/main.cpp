#include "./Base/cheatmanager.h"
#include "./Utils/implementation_imports.h"
#include "./Utils/logger.h"
#include "./Utils/pipeclient.h"
#include "./Utils/process.h"
#include "./Utils/debug.h"
#include "./Utils/utilities.h"
#include "./Utils/loadconfig.h"
/*
const std::wstring process_name = L"ac_client.exe";
Process* process = new Process(process_name, utilities::GetPIDByName(process_name));
*/

void Demo1(CheatManager* cheatManager);
void Demo2(CheatManager* cheatManager);
void Demo3(CheatManager* cheatManager);
void Demo4(CheatManager* cheatManager);
int Demo5(CheatManager* cheatManager);

int main() {
  YAML::Node config = InitializeConfig();

  LogTypeInfo lti = GetLogSettings(config);
  Logger& logger = Logger::getInstance(lti.display_mask, lti.save_mask);

  ProcessStartupInfo psi = GetProcessStartupInfo(config);
  Process* process = new Process(psi.name, psi.args, psi.working_directory);
  
  LPVOID base_address = reinterpret_cast<LPVOID>(
    utilities::GetModuleBaseAddress(process->GetPid(),
    utilities::GetExecutableName(process->GetPath()).c_str())
  );

  std::pair<void*, int> base_address_variable = CheatManager::NewVariable("base_address", "uint32_t");
  *reinterpret_cast<PDWORD>(base_address_variable.first) = reinterpret_cast<DWORD>(base_address);

  CheatManager* cheatManager = new CheatManager(process);

  std::vector<Module*> modules = ParseAttacks(config);
  cheatManager->AddAttacks(&modules);

  LOG_CONFIRMATION("Start 1");
  cheatManager->execute();
  /*
  Logger& logger = Logger::getInstance();

  const std::wstring process_name = L"D:\\AssaultCube 1.3.0.2\\bin_win32\\ac_client.exe";
  const std::wstring working_directory = L"D:\\AssaultCube 1.3.0.2";
  std::vector<std::wstring> args = {
    L"\"--home=?MYDOCUMENTS?\\My Games\\AssaultCube\\v1.3\"",
    L"--init %1 %2 %3 %4 %5"
  };

  Process* process = new Process(process_name, args, working_directory);
  CheatManager* cheatManager = new CheatManager(process);

  LOG_CONFIRMATION("Start 1");
  Demo2(cheatManager);
  LOG_CONFIRMATION("Execution finished. Process restart required.");
  process->RestartProcess();

  LOG_CONFIRMATION("Start 2");
  Demo2(cheatManager);
  LOG_CONFIRMATION("Execution finished. Process restart required.");
  process->RestartProcess();

  LOG_CONFIRMATION("Start 3");
  Demo3(cheatManager);
  LOG_CONFIRMATION("Execution finished. Process restart required.");
  process->RestartProcess();
  return 0;
  */
}
