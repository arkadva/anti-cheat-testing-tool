#include "./Base/cheatmanager.h"
#include "./Utils/implementation_imports.h"
#include "./Utils/process.h"
#include "./Utils/debug.h"

int main() {
  const std::wstring dll_name = L"dll1-dbg.dll";
  const std::wstring process_name = L"proc1-dbg.exe";

  void* address = reinterpret_cast<void*>(0x000000000014FED0);
  ULONG write_buffer = 1000;
  ULONG read_buffer = -1;

  Process* process = new Process(process_name);
  Sleep(1000);

  std::vector<Module*> attacks = { 
    //new ManualMapping(dll_name),
    new CreateRemoteThreadInjection(dll_name, RemoteThreadType::kCreateRemoteThread),
    //new WriteMemory(address, &write_buffer, sizeof(ULONG), WriteMemoryType::kNtWriteVirtualMemory),
    //new ReadMemory(address, &read_buffer, sizeof(int), ReadMemoryType::kNtReadVirtualMemory)
  };

  CheatManager* cheatManager = new CheatManager(process, &attacks);
  cheatManager->execute();

  return 0;
}
