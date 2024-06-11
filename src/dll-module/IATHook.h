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

using FnCreateFileW = HANDLE(__stdcall*)(LPCWSTR lpFileName, DWORD dwDesiredAccess, DWORD dwShareMode, LPSECURITY_ATTRIBUTES lpSecurityAttributes, DWORD dwCreationDisposition, DWORD dwFlagsAndAttributes, HANDLE hTemplateFile);
using FnReadFile = BOOL(__stdcall*)(HANDLE hFile, LPVOID lpBuffer, DWORD nNumberOfBytesToRead, LPDWORD lpNumberOfBytesRead, LPOVERLAPPED lpOverlapped);


static FnCreateFileW OriginalCreateFileW = nullptr;
static FnReadFile OriginalReadFile = nullptr;

inline HANDLE HookedCreateFileW(LPCWSTR lpFileName, DWORD dwDesiredAccess, DWORD dwShareMode, LPSECURITY_ATTRIBUTES lpSecurityAttributes, DWORD dwCreationDisposition, DWORD dwFlagsAndAttributes, HANDLE hTemplateFile)
{
  MessageBoxA(NULL, "hello", "hello", 0);
  return OriginalCreateFileW(lpFileName, dwDesiredAccess, dwShareMode, lpSecurityAttributes, dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile);
}

inline BOOL HookedReadFile(HANDLE hFile, LPVOID lpBuffer, DWORD nNumberOfBytesToRead, LPDWORD lpNumberOfBytesRead, LPOVERLAPPED lpOverlapped)
{
  MessageBoxA(NULL, "hello", "hello", 0);
  return OriginalReadFile(hFile, lpBuffer, nNumberOfBytesToRead, lpNumberOfBytesRead, lpOverlapped);
}

#endif
