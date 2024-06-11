#ifndef HARDWAREHOOK_H_
#define HARDWAREHOOK_H_

#include <Windows.h>

#include <chrono>
#include <ctime>
#include <format>
#include <iostream>
#include <string>
#include <thread>
#include "struct_definitions.h"


LONG WINAPI HardwareExceptionHandler(EXCEPTION_POINTERS* const exceptionInfo);
bool SetHardwareBreakpoint(const HANDLE& mainThreadHandle, const void* const targetAddress);
void HardwareHook(BreakpointHookData* hookData, ContextChangeEntry* entryList);

#endif
