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

LONG WINAPI ExceptionHandler(EXCEPTION_POINTERS* const exceptionInfo);
void SetMemoryBreakpoint(void* const targetAddress);
void PageGuardHook(void* address);

#endif
