#include <Windows.h>
#include "../../Utils/logger.h"
#include "../../Utils/utilities.h"
#include "ReadOffset.h"
#include <iomanip> 

BOOL ReadOffsetImp(DWORD pid, PVOID address, PVOID buffer, BYTE type, std::vector<uintptr_t> offsets) {
  Logger& logger = Logger::getInstance();

#ifdef DEBUG
  LOG_CALL("pid = %lu, address = %p, buffer = %p, type = %u, offsets = %p", pid, address, buffer, type, offsets);
#endif

  SIZE_T bytesRead = 0;
  ULONG bytesReadNt = 0;

  HANDLE processHandle = OpenProcess(PROCESS_VM_READ, FALSE, pid);

  if (processHandle == NULL) {
    LOG_ERROR("Failed to open handle to process.");
    return FALSE;
  }

  BOOL success = TRUE;
  uintptr_t pointer = (uintptr_t)address;

  for (size_t i = 0; i < offsets.size(); ++i) {
    if (i < offsets.size() - 1) {
      if (type & kReadProcessMemory) {
        RPM(processHandle, (LPCVOID)(pointer + offsets[i]), (LPVOID)&pointer, sizeof(uintptr_t), &bytesRead);
      }
      else if (type & kNtReadVirtualMemory) {
        NTRVM(processHandle, (PVOID)(pointer + offsets[i]), (LPVOID)&pointer, sizeof(uintptr_t), &bytesReadNt);
      }
    }
    else {
      pointer += offsets[i];
    }
  }

  *(uintptr_t*)buffer = pointer;

  CloseHandle(processHandle);
  return success;
}
