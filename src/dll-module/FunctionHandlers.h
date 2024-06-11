#ifndef FUNCTIONHANDLERS_H_
#define FUNCTIONHANDLERS_H_

#include <Windows.h>
#include <string>
#include <vector>
#include <map>
#include <functional>
#include "PageGuardHook.h"
#include "HardwareHook.h"
#include "IATHook.h"

extern std::map<std::string, std::function<BOOL(BYTE* buffer)>> functionMap;

void InitializeFunctionMap();

BOOL HandleRequest(const std::string& intent, BYTE* buffer);

// hooks
BOOL PageGuardHook_wrapper(BYTE* buffer);
BOOL HardwareHook_wrapper(BYTE* buffer);
BOOL IATHook_wrapper(BYTE* buffer);
#endif
