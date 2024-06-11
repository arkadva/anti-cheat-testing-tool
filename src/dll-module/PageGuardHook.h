#ifndef PAGEGUARDHOOK_H_
#define PAGEGUARDHOOK_H_
#define _CRT_SECURE_NO_WARNINGS

#include <Windows.h>

#include <chrono>
#include <ctime>
#include <format>
#include <iostream>
#include <string>
#include <thread>
#include "struct_definitions.h"

LONG WINAPI PageGuardExceptionHandler(EXCEPTION_POINTERS* const exceptionInfo);
void SetPageGuardHook(void* const targetAddress);
void PageGuardHook(BreakpointHookData* hookData, ContextChangeEntry* entryList);

#endif
