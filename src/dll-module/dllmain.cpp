#include <windows.h>
#include "PipeServer.h"

// SetWindowsHookEx injection
extern "C" __declspec(dllexport) int NextHook(int code, WPARAM wParam, LPARAM lParam) { return CallNextHookEx(NULL, code, wParam, lParam); }

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
  switch (ul_reason_for_call) {
  case DLL_PROCESS_ATTACH:
    MessageBoxA(NULL, "DLL has been loaded!", "Notification", MB_OK | MB_ICONINFORMATION);
    DisableThreadLibraryCalls(hModule);
    CreatePipeServer();
    break;
  case DLL_PROCESS_DETACH:
    break;
  }

  return TRUE;
}
