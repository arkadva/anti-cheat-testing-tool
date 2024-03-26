#include "process.h"
#include "CreateRemoteThreadInjection.h"
#include "ManualMapping.h"
#include "WriteMemory.h"
#include "ReadMemory.h"
#include "cheatmanager.h"

int main() {
  ULONG write_buffer = 1000;
  ULONG read_buffer = -1;

  const std::wstring dll_name = L"dll1-dbg.dll";
  const std::wstring process_name = L"proc1-dbg.exe";

  Process* process = new Process(process_name);

  Module* manual_mapping = new ManualMapping(dll_name);
  Module* crtj = new CreateRemoteThreadInjection(dll_name, RemoteThreadType::kRtlCreateUserThread);
  Module* write_memory = new WriteMemory(0x000000000014FED0, &write_buffer, sizeof(int), WriteMemoryType::kNtWriteVirtualMemory);
  Module* read_memory = new ReadMemory(0x000000000014FED0, &read_buffer, sizeof(int), ReadMemoryType::kReadProcessMemory);

  std::vector<Module*> attacks = { 
    //manual_mapping,
    //crtj,
    //write_memory,
    read_memory
  };

  getchar();

  CheatManager* cheatManager = new CheatManager(process, &attacks);
  cheatManager->execute();

  std::cout << read_buffer << std::endl;

  return 0;
}
