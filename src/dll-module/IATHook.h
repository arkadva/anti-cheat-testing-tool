#ifndef IATHOOK_H_
#define IATHOOK_H_

#include <Windows.h>

#include <chrono>
#include <ctime>
#include <format>
#include <iostream>
#include <string>
#include <thread>

void IATHook();

PIMAGE_THUNK_DATA GetIATEntry(const std::string& EntryName);
ULONGLONG CreateHook(const std::string& IATName, ULONG_PTR HookedFunction);

using FnExitProcess = void(__stdcall*)(UINT uExitCode);

static FnExitProcess OriginalExitProcess = nullptr;

inline void HookedExitProcess(UINT uExitCode)
{
  MessageBoxA(NULL, "hello", "hello", 0);
  //OriginalExitProcess(uExitCode);
}

#endif
