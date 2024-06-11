#include <Windows.h>
#include <sstream>
#include <iostream>
#include "FunctionHandlers.h"

std::map<std::string, std::function<BOOL(BYTE* buffer)>> functionMap;

void InitializeFunctionMap() {
  functionMap["PageGuardHook"] = PageGuardHook_wrapper;
  functionMap["HardwareHook"] = HardwareHook_wrapper;
  functionMap["IATHook"] = IATHook_wrapper;
}

BOOL HandleRequest(const std::string& intent, BYTE* buffer) {
  BOOL result = FALSE;
  if (functionMap.find(intent) != functionMap.end()) {
    result = functionMap[intent](buffer);
  }

  return result;
}

BOOL PageGuardHook_wrapper(BYTE* buffer) {
  BreakpointHookData* hookData = reinterpret_cast<BreakpointHookData*>(buffer);
  ContextChangeEntry* entryList = reinterpret_cast<ContextChangeEntry*>(buffer + sizeof(BreakpointHookData));

  PageGuardHook(hookData, entryList);
  return TRUE;
}

BOOL HardwareHook_wrapper(BYTE* buffer) {
  BreakpointHookData* hookData = reinterpret_cast<BreakpointHookData*>(buffer);
  ContextChangeEntry* entryList = reinterpret_cast<ContextChangeEntry*>(buffer + sizeof(BreakpointHookData));

  HardwareHook(hookData, entryList);
  return TRUE;
}

BOOL IATHook_wrapper(BYTE* buffer) {
  //IATHook();
  return TRUE;
}
