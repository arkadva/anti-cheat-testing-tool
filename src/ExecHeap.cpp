#include <windows.h>
#include "ExecHeap.h"
#include "logger.h"

// temporary shellcode
unsigned char shellcode[] = { 
  0xc3  // x86 ret instruction
};

bool ExecHeap::execute(const Process* process) const {
  Logger& logger = Logger::getInstance();

  memcpy(address_, shellcode, sizeof(shellcode));
  DWORD oldProtect;

  if (!VirtualProtect(address_, 1, PAGE_EXECUTE_READ, &oldProtect)) {
    VirtualFree(address_, 0, MEM_RELEASE);
    logger.error("Proteciton change failed.");
    return false;
  }
  HANDLE processHandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, process->GetPid());

  if (processHandle == NULL) {
    VirtualFree(address_, 0, MEM_RELEASE);
    logger.error("Opening a handle failed.");
    return false;
  }

  LPTHREAD_START_ROUTINE threadStart = (LPTHREAD_START_ROUTINE) address_;
  HANDLE threadHandle = CreateRemoteThread(processHandle, NULL, 0, threadStart, NULL, 0, NULL);

  if (threadHandle == NULL) {
    CloseHandle(processHandle);
    VirtualFree(address_, 0, MEM_RELEASE);
    logger.error("Thread creation failed.");
    return false;
  }

  CloseHandle(threadHandle);
  CloseHandle(processHandle);

  return true;
}
