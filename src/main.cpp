#include "process.h"
#include "CreateRemoteThreadInjection.h"
#include "cheatmanager.h"

int main() {
  const std::wstring process_name = L"proc1-dbg.exe";
  const std::wstring dll_name = L"dll1-dbg.dll";

  Process* process = new Process(process_name);
  Module* crtj = new CreateRemoteThreadInjection(dll_name, RemoteThreadType::kRtlCreateUserThread);

  std::vector<Module*> attacks;
  attacks.push_back(crtj);

  CheatManager* cheatManager = new CheatManager(process, &attacks);
  cheatManager->execute();

  delete process;
  delete crtj;
  delete cheatManager;

  return 0;
}