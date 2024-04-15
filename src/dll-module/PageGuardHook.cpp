// https://www.codereversing.com/archives/595
// https://github.com/codereversing/functionhooks/blob/main/SoftwareBreakpoint/Source.cpp
#include "PageGuardHook.h"

static void* functionAddress;

void SetMemoryBreakpoint(void* const targetAddress) {
  DWORD oldProtections{};
  MEMORY_BASIC_INFORMATION memoryInfo{};
  auto result { VirtualQuery(targetAddress, &memoryInfo, sizeof(MEMORY_BASIC_INFORMATION)) };

  if (result == 0) {
    std::cout << "VirtualQuery" << std::endl;
  }

  result = VirtualProtect(targetAddress, sizeof(void*), memoryInfo.Protect | PAGE_GUARD, &oldProtections);
  if (!result) {
    std::cout << "VirtualProtect" << std::endl;
  }
}

LONG WINAPI ExceptionHandler(EXCEPTION_POINTERS* const exceptionInfo) {
  if (exceptionInfo->ExceptionRecord->ExceptionCode == STATUS_GUARD_PAGE_VIOLATION) {

    if (exceptionInfo->ExceptionRecord->ExceptionAddress == functionAddress) {
      exceptionInfo->ContextRecord->Rdx = 10;
    }

    // set single step flag so that memory breakpoints are re-enabled
    // on the next instruction execution.
    exceptionInfo->ContextRecord->EFlags |= 0x100;

    return EXCEPTION_CONTINUE_EXECUTION;
  }

  if (exceptionInfo->ExceptionRecord->ExceptionCode == EXCEPTION_SINGLE_STEP) {

    // re-enable memory breakpoint since a different address might
    // have caused the guard page violation.
    SetMemoryBreakpoint(functionAddress);

    return EXCEPTION_CONTINUE_EXECUTION;
  }

  return EXCEPTION_CONTINUE_SEARCH;
}

void PageGuardHook(void* address) {
  functionAddress = address;
  AddVectoredExceptionHandler(true, ExceptionHandler);
  SetMemoryBreakpoint(functionAddress);
}
