// https://www.codereversing.com/archives/595
// https://github.com/codereversing/functionhooks/blob/main/SoftwareBreakpoint/Source.cpp
#include "PageGuardHook.h"


namespace PageGuardHookNs {
  static void* functionAddress;
  ContextChangeEntry* entries;
  int length = 0;
};

LONG WINAPI PageGuardExceptionHandler(EXCEPTION_POINTERS* const exceptionInfo) {
  if (exceptionInfo->ExceptionRecord->ExceptionCode == STATUS_GUARD_PAGE_VIOLATION) {

    if (exceptionInfo->ExceptionRecord->ExceptionAddress == PageGuardHookNs::functionAddress) {
      for (int i = 0; i < PageGuardHookNs::length; i++) {
        *reinterpret_cast<PDWORD>(reinterpret_cast<char*>(exceptionInfo->ContextRecord) + PageGuardHookNs::entries[i].offset) = reinterpret_cast<DWORD>(PageGuardHookNs::entries[i].value);
      }
    }

    // set single step flag so that memory breakpoints are re-enabled
    // on the next instruction execution.
    exceptionInfo->ContextRecord->EFlags |= 0x100;

    return EXCEPTION_CONTINUE_EXECUTION;
  }

  if (exceptionInfo->ExceptionRecord->ExceptionCode == EXCEPTION_SINGLE_STEP) {

    // re-enable memory breakpoint since a different address might
    // have caused the guard page violation.
    SetPageGuardHook(PageGuardHookNs::functionAddress);

    return EXCEPTION_CONTINUE_EXECUTION;
  }

  return EXCEPTION_CONTINUE_SEARCH;
}

void SetPageGuardHook(void* const targetAddress) {
  DWORD oldProtections{};
  MEMORY_BASIC_INFORMATION memoryInfo{};
  auto result{ VirtualQuery(targetAddress, &memoryInfo, sizeof(MEMORY_BASIC_INFORMATION)) };

  if (result == 0) {
    std::cout << "VirtualQuery" << std::endl;
  }

  result = VirtualProtect(targetAddress, sizeof(void*), memoryInfo.Protect | PAGE_GUARD, &oldProtections);
  if (!result) {
    std::cout << "VirtualProtect" << std::endl;
  }
}

void PageGuardHook(BreakpointHookData* hookData, ContextChangeEntry* entryList) {
  PageGuardHookNs::functionAddress = hookData->address;
  PageGuardHookNs::length = hookData->entriesSize;

  // pointer to stack of a different thread - hard copy is required
  PageGuardHookNs::entries = reinterpret_cast<ContextChangeEntry*>(malloc(sizeof(ContextChangeEntry) * PageGuardHookNs::length));
  if (PageGuardHookNs::entries != NULL)
    memcpy(PageGuardHookNs::entries, entryList, sizeof(ContextChangeEntry) * PageGuardHookNs::length);

  AddVectoredExceptionHandler(true, PageGuardExceptionHandler);
  SetPageGuardHook(PageGuardHookNs::functionAddress);
}
