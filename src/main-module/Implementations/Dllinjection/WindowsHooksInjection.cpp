#include "WindowsHooksInjection.h"
#include "../../Utils/logger.h"
#include "../../Utils/utilities.h"

int WindowsHooksInjectionImp(DWORD pid, const std::wstring& dll_name) {
  Logger& logger = Logger::getInstance();
#ifdef DEBUG
  LOG_CALL("pid = %p, dll = %ws", pid, dll_name);
#endif

  HWND hwnd = utilities::FindWindowByPID(pid);

  if (hwnd == NULL) {
    LOG_ERROR("Could not find target window.");
    return EXIT_FAILURE;
  }

  // Getting the thread of the window and the PID
  //DWORD pid = NULL;
  DWORD tid = GetWindowThreadProcessId(hwnd, &pid);

  if (tid == NULL) {
    LOG_ERROR("Could not get thread ID of the target window.");
    return EXIT_FAILURE;
  }

  // Loading DLL
  HMODULE dll = LoadLibraryEx(dll_name.c_str(), NULL, DONT_RESOLVE_DLL_REFERENCES);
  if (dll == NULL) {
    LOG_ERROR("The DLL could not be found.");
    return EXIT_FAILURE;
  }

  // Getting exported function address
  HOOKPROC addr = (HOOKPROC)GetProcAddress(dll, "NextHook");
  if (addr == NULL) {
    LOG_ERROR("The function was not found.");
    return EXIT_FAILURE;
  }

  // Setting the hook in the hook chain
  HHOOK handle = SetWindowsHookEx(WH_GETMESSAGE, addr, dll, tid);
  if (handle == NULL) {
    LOG_ERROR("Couldn't set the hook with SetWindowsHookEx.");
    return EXIT_FAILURE;
  }

  // Triggering the hook
  PostThreadMessage(tid, WM_NULL, NULL, NULL);

  /*
  // Waiting for user input to remove the hook
  std::cout << "[ OK ] Hook set and triggered." << std::endl;
  std::cout << "[ >> ] Press any key to unhook (This will unload the DLL)." << std::endl;
  system("pause > nul");

  // Unhooking
  BOOL unhook = UnhookWindowsHookEx(handle);
  if (unhook == FALSE) {
    std::cout << "[ FAILED ] Could not remove the hook." << std::endl;
    system("pause");
    return EXIT_FAILURE;
  }

  std::cout << "[ OK ] Done. Press any key to exit." << std::endl;
  system("pause > nul");
  return EXIT_SUCCESS;
  */
}