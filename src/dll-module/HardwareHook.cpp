// Disable warning for usage of ctime
#define _CRT_SECURE_NO_WARNINGS

#include <Windows.h>
#include <ProcessSnapshot.h>
#include <chrono>
#include <ctime>
#include <future>
#include <iostream>
#include <memory>
#include <string>
#include <thread>
#include "HardwareHook.h"

namespace HardwareHookNs {
  ContextChangeEntry* entries;
  int length = 0;
};

LONG WINAPI HardwareExceptionHandler(EXCEPTION_POINTERS* const exceptionInfo) {

  if (exceptionInfo->ExceptionRecord->ExceptionCode == EXCEPTION_SINGLE_STEP) {

    if (exceptionInfo->ContextRecord->Dr6 & 0x1) {

      for (int i = 0; i < HardwareHookNs::length; i++) {
        *reinterpret_cast<PDWORD>(reinterpret_cast<char*>(exceptionInfo->ContextRecord) + HardwareHookNs::entries[i].offset) = reinterpret_cast<DWORD>(HardwareHookNs::entries[i].value);
      }

      // Set the resume flag before continuing execution
      exceptionInfo->ContextRecord->EFlags |= 0x10000;
    }

    return EXCEPTION_CONTINUE_EXECUTION;
  }

  return EXCEPTION_CONTINUE_SEARCH;
}

DWORD GetMainThreadId(const HANDLE processHandle) {

  std::shared_ptr<HPSS> snapshot(new HPSS{}, [&](HPSS* snapshotPtr) {
    PssFreeSnapshot(processHandle, *snapshotPtr);
    });

  auto result{ PssCaptureSnapshot(processHandle,
      PSS_CAPTURE_THREADS, 0, snapshot.get()) };

  std::shared_ptr<HPSSWALK> walker(new HPSSWALK{}, [&](HPSSWALK* walkerPtr) {
    PssWalkMarkerFree(*walkerPtr);
    });

  result = PssWalkMarkerCreate(nullptr, walker.get());

  DWORD mainThreadId{};
  FILETIME lowestCreateTime{ MAXDWORD, MAXDWORD };

  PSS_THREAD_ENTRY thread{};

  // Iterate through the threads and keep track of the one
  // with the lowest creation time.
  while (PssWalkSnapshot(*snapshot, PSS_WALK_THREADS,
    *walker, &thread, sizeof(thread)) == ERROR_SUCCESS) {
    if (CompareFileTime(&lowestCreateTime, &thread.CreateTime) == 1) {
      lowestCreateTime = thread.CreateTime;
      mainThreadId = thread.ThreadId;
    }
  }

  return mainThreadId;
}

bool SetHardwareBreakpoint(const HANDLE& mainThreadHandle, const void* const targetAddress) {

  CONTEXT mainThreadContext;
  memset(&mainThreadContext, 0, sizeof(CONTEXT));
  mainThreadContext.ContextFlags = CONTEXT_DEBUG_REGISTERS;

  mainThreadContext.Dr0 = reinterpret_cast<DWORD64>(targetAddress);

  mainThreadContext.Dr7 = (1 << 0);

  // Suspend the thread before setting its context
  SuspendThread(mainThreadHandle);

  // Set the main threads context
  auto result{ SetThreadContext(mainThreadHandle, &mainThreadContext) };

  // Resume the thread after setting its context
  ResumeThread(mainThreadHandle);

  return result != 0;
}

void HardwareHook(BreakpointHookData* hookData, ContextChangeEntry* entryList) {
  const auto mainThreadId{ GetMainThreadId(GetCurrentProcess()) };
  const auto mainThreadHandle{ OpenThread(THREAD_SET_CONTEXT | THREAD_SUSPEND_RESUME, false, mainThreadId) };

  HardwareHookNs::length = hookData->entriesSize;
  // pointer to stack of a different thread - hard copy is required
  HardwareHookNs::entries = reinterpret_cast<ContextChangeEntry*>(malloc(sizeof(ContextChangeEntry) * HardwareHookNs::length));
  if (HardwareHookNs::entries != NULL)
    memcpy(HardwareHookNs::entries, entryList, sizeof(ContextChangeEntry) * HardwareHookNs::length);


  AddVectoredExceptionHandler(true, HardwareExceptionHandler);
  SetHardwareBreakpoint(mainThreadHandle, hookData->address);

  /* debug
  std::string message = "hookData->address: " + std::to_string(reinterpret_cast<DWORD>(hookData->address)) + "\n";
  message += "hookData->entriesSize: " + std::to_string(static_cast<int>(hookData->entriesSize)) + "\n";
  message += "entryList->value: " + std::to_string(reinterpret_cast<int>(entryList->value)) + "\n";
  message += "entryList->offset: " + std::to_string(static_cast<int>(entryList->offset));
  LPCSTR messageLPCSTR = message.c_str();
  MessageBoxA(NULL, messageLPCSTR, "Hook Data Information", MB_OK | MB_ICONINFORMATION);
  */

  CloseHandle(mainThreadHandle);
}
