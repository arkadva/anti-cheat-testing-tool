#pragma once
/*
/////////
char str[] = "PageGuardHook";
BreakpointHookData hookData = { reinterpret_cast<void*>(0x41C223), 1 };
ContextChangeEntry entryData = { reinterpret_cast<void*>(-5), offsetof(CONTEXT, Esi)};
size_t size = strlen(str) + 1 + sizeof(BreakpointHookData) + sizeof(ContextChangeEntry);

BYTE* buffer = reinterpret_cast<BYTE*>(malloc(size));

if (buffer == NULL)
  exit(1);

memcpy(buffer, str, strlen(str) + 1);
memcpy(buffer + strlen(str) + 1, &hookData, sizeof(BreakpointHookData));
memcpy(buffer + strlen(str) + 1 + sizeof(ContextChangeEntry), &entryData, sizeof(ContextChangeEntry));
/////////
*/

/*
PipeClient* pc = cheatManager->client_;
pc->connect();
pc->write(buffer, size);

std::cout << pc->read() << std::endl;
*/

/*
int instruction_address = 0x41C223; //utilities::GetModuleBaseAddress(process->GetPid(), process_name.c_str()) + 0x1C223;

std::string message = "HardwareHook|0x41C223";

PipeClient* pc = cheatManager->client_;
pc->connect();
pc->write(message);
std::cout << pc->read() << std::endl;
*/


/*
int player_health_pointer = 0;
int desired_health = 300;

std::vector<uintptr_t> offsets = { 0x17E0A8, 0xEC };

std::vector<Module*> attacks = {
  //new CreateRemoteThreadInjection(actt_dll_name, RemoteThreadType::kRtlCreateUserThread),
  new ReadOffset(reinterpret_cast<LPVOID>(utilities::GetModuleBaseAddress(process->GetPid(), process_name.c_str())),
                 &player_health_pointer,
                 4,
                 kReadProcessMemory,
                 offsets)
};


CheatManager* cheatManager = new CheatManager(process, &attacks);
cheatManager->execute();

Sleep(1000);

std::vector<Module*> attacks2 = {
  new WriteMemory(reinterpret_cast<LPVOID>(player_health_pointer), &desired_health, 4, kWriteProcessMemory)
};

CheatManager* cheatManager2 = new CheatManager(process, &attacks2);
cheatManager2->execute();


getchar();
*/