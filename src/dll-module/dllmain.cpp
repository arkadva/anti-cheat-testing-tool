#include <windows.h>
#include "PipeServer.h"

extern "C" __declspec(dllexport) int NextHook(int code, WPARAM wParam, LPARAM lParam) { return CallNextHookEx(NULL, code, wParam, lParam); }

HANDLE hThread;

DWORD WINAPI PipeServerThread(LPVOID lpParam) {
  CreatePipeServer();
  return 0;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
  switch (ul_reason_for_call) {
  case DLL_PROCESS_ATTACH:
    hThread = CreateThread(NULL, 0, PipeServerThread, NULL, 0, NULL);
    MessageBoxA(NULL, "DLL has been loaded!", "Notification", MB_OK | MB_ICONINFORMATION);
    DisableThreadLibraryCalls(hModule);
    break;
  case DLL_PROCESS_DETACH:
    if (hThread) {
      CloseHandle(hThread);
    }
    break;
  }

  return TRUE;
}