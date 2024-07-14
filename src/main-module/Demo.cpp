#include "./Base/cheatmanager.h"
#include "./Utils/implementation_imports.h"
#include "./Utils/pipeclient.h"
#include "./Utils/process.h"
#include "./Utils/debug.h"
#include "./Utils/utilities.h"

void Demo1(CheatManager* cheatManager) {
  const std::wstring actt_dll_name = L"D:\\Programming\\anti-cheat-testing-tool\\Release\\actt-dll.dll";

  BreakpointHookData hookData = { reinterpret_cast<void*>(0x41C223), 1 };
  ContextChangeEntry entryData = { reinterpret_cast<void*>(-5), offsetof(CONTEXT, Esi) };

  std::vector<Module*> attacks = {
      new CreateRemoteThreadInjection(actt_dll_name, RemoteThreadType::kRtlCreateUserThread),
      new BreakpointHook(cheatManager->client_, &hookData, &entryData, kPageGuard),
  };

  cheatManager->AddAttacks(&attacks);
  cheatManager->execute();
}

void Demo2(CheatManager* cheatManager) {
  std::vector<uintptr_t> player_health_offsets = { 0x17E0A8, 0xEC };
  std::vector<uintptr_t> player_ammo_offsets = { 0x17E0A8, 0x368, 0x14, 0x0 };
  std::vector<uintptr_t> player_coords_offsets = { 0x195404, 0x28 };

  struct Coordinates { float x; float y; float z; };

  int* player_health_pointer = 0;
  int* player_ammo_pointer = 0;
  Coordinates* player_coords_pointer = 0;

  LPVOID base_address = reinterpret_cast<LPVOID>(
    utilities::GetModuleBaseAddress(cheatManager->process_->GetPid(), 
    utilities::GetExecutableName(cheatManager->process_->GetPath()).c_str())
  );

  std::vector<Module*> attacks = {
    new ReadOffset(base_address, &player_health_pointer, kNtReadVirtualMemory, player_health_offsets),
    new ReadOffset(base_address, &player_ammo_pointer, kReadProcessMemory, player_ammo_offsets),
    new ReadOffset(base_address, &player_coords_pointer, kReadProcessMemory, player_coords_offsets)
  };

  cheatManager->AddAttacks(&attacks);
  cheatManager->execute();
  cheatManager->ClearAttacks();


  int player_health = -1;
  int player_ammo = -1;
  Coordinates player_coords = { -1, -1, -1 };

  attacks = {
    new ReadMemory(player_health_pointer, &player_health, sizeof(int), kNtReadVirtualMemory),
    new ReadMemory(player_ammo_pointer, &player_ammo, sizeof(int), kNtReadVirtualMemory),
    new ReadMemory(player_coords_pointer, &player_coords, sizeof(Coordinates), kNtReadVirtualMemory),
  };

  cheatManager->AddAttacks(&attacks);
  cheatManager->execute();
  
  std::cout << std::endl;
  std::cout << "Player health: " << player_health << std::endl;
  std::cout << "Weapon ammo: " << player_ammo << std::endl;
  std::cout << "Player position:" << " X: " << player_coords.x << " Y: " << player_coords.y << " Z: " << player_coords.z << std::endl;
}

void Demo3(CheatManager* cheatManager){
  std::vector<uintptr_t> player_health_offsets = { 0x17E0A8, 0xEC };
  std::vector<uintptr_t> player_ammo_offsets = { 0x17E0A8, 0x368, 0x14, 0x0 };

  int* player_health_pointer = 0;
  int* player_ammo_pointer = 0;

  LPVOID base_address = reinterpret_cast<LPVOID>(utilities::GetModuleBaseAddress(cheatManager->process_->GetPid(), cheatManager->process_->GetPath().c_str()));

  std::wcout << cheatManager->process_->GetPath() << std::endl;

  std::vector<Module*> attacks = {
    new ReadOffset(base_address, &player_health_pointer, ReadMemoryType::kReadProcessMemory, player_health_offsets),
    new ReadOffset(base_address, &player_ammo_pointer, ReadMemoryType::kReadProcessMemory, player_ammo_offsets),
  };

  cheatManager->AddAttacks(&attacks);
  cheatManager->execute();
  cheatManager->ClearAttacks();


  int modified_value = 999;

  attacks = {
    new WriteMemory(player_health_pointer, &modified_value, sizeof(int), WriteMemoryType::kNtWriteVirtualMemory),
    new WriteMemory(player_ammo_pointer, &modified_value, sizeof(int), WriteMemoryType::kWriteProcessMemory),
  };

  cheatManager->AddAttacks(&attacks);
  cheatManager->execute();
}

void Demo4(CheatManager* cheatManager) {
  const std::wstring actt_dll_name = L"D:\\Programming\\anti-cheat-testing-tool\\Release\\actt-dll.dll";

  std::vector<Module*> attacks = {
      //new ManualMapping(actt_dll_name)
      new CreateRemoteThreadInjection(actt_dll_name, RemoteThreadType::kRtlCreateUserThread)
  };

  cheatManager->AddAttacks(&attacks);
  cheatManager->execute();
  
  Sleep(3000);

  char attack[] = "IATHook";
  cheatManager->client_->connect();
  cheatManager->client_->write(reinterpret_cast<BYTE*>(attack), strlen(attack) + 1);
}

int Demo5(CheatManager* cheatManager) {
  const std::wstring actt_dll_name = L"D:\\Programming\\anti-cheat-testing-tool\\Release\\actt-dll.dll";

  std::vector<Module*> attacks = {
    new WindowsHooksInjection(actt_dll_name)
  };

  cheatManager->AddAttacks(&attacks);
  cheatManager->execute();

  Sleep(2000);
}