#include <Windows.h>
#include "../../Utils/logger.h"
#include "../../Utils/utilities.h"
#include "ReadMemory.h"

BOOL Read(DWORD pid, PVOID address, PVOID buffer, ULONG size, BYTE type) {
  Logger& logger = Logger::getInstance();

#ifdef DEBUG
  LOG_CALL("pid = %lu, address = %p, buffer = %p, size = %lu, type = %u", pid, address, buffer, size, type);
#endif

  SIZE_T bytesRead = 0;
  ULONG bytesReadNt = 0;

  HANDLE processHandle = OpenProcess(PROCESS_VM_READ, FALSE, pid);

  if (processHandle == NULL) {
    LOG_ERROR("Failed to open handle to process.");
    return FALSE;
  }

  BOOL success = TRUE;

  if (type & kReadProcessMemory) {
    BOOL status = RPM(processHandle, (LPCVOID)address, buffer, size, &bytesRead);
    success &= status;

    if (status) {
      LOG_INFO("ReadProcessMemory call succeeded. Bytes read: %d.", bytesRead);
    }
    else {
      LOG_ERROR("ReadProcessMemory returned false.");
    }
  }

  if (type & kNtReadVirtualMemory) {
    BOOL status = NTRVM(processHandle, (PVOID)address, buffer, (ULONG)size, &bytesReadNt);
    success &= status;

    if (status) {
      LOG_INFO("NtReadVirtualMemory call succeeded. Bytes read: %d.", bytesReadNt);
    }
    else {
      LOG_ERROR("NtReadVirtualMemory returned false.");
    }
  }

  CloseHandle(processHandle);
  return success;
}