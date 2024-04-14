#include "PipeServer.h"
#include "FunctionHandlers.h"
#include <windows.h>
#include <iostream>
#include <map>
#include <sstream>

bool CreatePipeServer() {
  HANDLE hPipe;
  char buffer[1024];
  DWORD dwRead, dwWritten;

  hPipe = CreateNamedPipeA("\\\\.\\pipe\\actt-pipe",
    PIPE_ACCESS_DUPLEX,
    PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT,
    PIPE_UNLIMITED_INSTANCES,
    1024,
    1024,
    NMPWAIT_USE_DEFAULT_WAIT,
    NULL);
  if (hPipe == INVALID_HANDLE_VALUE) {
    std::cout << "Failed to create pipe." << std::endl;
    return false;
  }

  if (ConnectNamedPipe(hPipe, NULL) == FALSE) {
    std::cout << "Failed to connect pipe." << std::endl;
    CloseHandle(hPipe);
    return false;
  }

  MessageBoxA(NULL, "DLL loaded and pipe server started.", "Info", MB_OK);

  InitializeFunctionMap();

  while (ReadFile(hPipe, buffer, sizeof(buffer) - 1, &dwRead, NULL) != FALSE) {
    buffer[dwRead] = '\0';
    std::string input(buffer);

    BOOL val = HandleRequest(input);

    std::string result = std::to_string(val);
    WriteFile(hPipe, buffer, strlen(buffer) + 1, &dwWritten, NULL);
  }

  DisconnectNamedPipe(hPipe);
  CloseHandle(hPipe);
  return true;
}
