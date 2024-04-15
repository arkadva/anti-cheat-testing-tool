#include "./Base/cheatmanager.h"
#include "./Utils/implementation_imports.h"
#include "./Utils/pipeclient.h"
#include "./Utils/process.h"
#include "./Utils/debug.h"

int main() {
  const std::wstring dll_name = L"dll1-dbg.dll";
  const std::wstring actt_dll_name = L"actt-dll.dll";
  const std::wstring process_name = L"proc1-dbg.exe";

  void* address = reinterpret_cast<void*>(0x000000000014FED0);
  ULONG write_buffer = 1000;
  ULONG read_buffer = -1;

  Process* process = new Process(process_name);
  Sleep(1000);

  std::vector<Module*> attacks = { 
    new CreateRemoteThreadInjection(actt_dll_name, RemoteThreadType::kRtlCreateUserThread),
  };

  CheatManager* cheatManager = new CheatManager(process, &attacks);
  cheatManager->execute();
  
  Sleep(1000);

  PipeClient* pc = cheatManager->client_;
  pc->connect();
  pc->write("PageGuardHook|0x0000000140001079");
  std::cout << pc->read() << std::endl;

  getchar();
  return 0;
}
