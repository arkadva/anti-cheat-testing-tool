#include "./Base/cheatmanager.h"
#include "./Utils/implementation_imports.h"
#include "./Utils/pipeclient.h"
#include "./Utils/process.h"
#include "./Utils/debug.h"
#include "./Utils/utilities.h"

int main() {
  const std::wstring process_name = L"ac_client.exe";
  const std::wstring actt_dll_name = L"D:\\Programming\\anti-cheat-testing-tool\\Release\\actt-dll.dll"; 
  Process* process = new Process(utilities::GetPIDByName(process_name));
  CheatManager* cheatManager = new CheatManager(process);

  Sleep(2000);

  BreakpointHookData hookData = { reinterpret_cast<void*>(0x41C223), 1 };
  ContextChangeEntry entryData = { reinterpret_cast<void*>(-5), offsetof(CONTEXT, Esi) };

  std::vector<Module*> attacks = {
    new CreateRemoteThreadInjection(actt_dll_name, RemoteThreadType::kRtlCreateUserThread),
    new BreakpointHook(cheatManager->client_, &hookData, &entryData, kPageGuard),
  };

  cheatManager->AddAttacks(&attacks);
  cheatManager->execute();

  Sleep(5000);

  return 0;
}
