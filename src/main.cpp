#include "process.h"
#include "CreateRemoteThreadInjection.h"
#include "ManualMapping.h"
#include "cheatmanager.h"

int main() {
  const std::wstring process_name = L"proc1-dbg.exe";
  Process* process = new Process(process_name);

  const std::wstring dll_name = L"dll1-dbg.dll";
  Module* crtj = new CreateRemoteThreadInjection(dll_name, RemoteThreadType::kRtlCreateUserThread);
  Module* manual_mapping = new ManualMapping(dll_name);

  std::vector<Module*> attacks = { 
    manual_mapping,
    //crtj,
  };

  CheatManager* cheatManager = new CheatManager(process, &attacks);
  cheatManager->execute();

  delete process;
  delete crtj;
  delete manual_mapping;
  delete cheatManager;

  return 0;
}
