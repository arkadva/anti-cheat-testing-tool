#include "PipeServer.h"
#include "FunctionHandlers.h"
#include <windows.h>
#include <iostream>
#include <map>
#include <sstream>


bool CreatePipeServer() {
  HANDLE hPipe;
  BYTE buffer[4096];
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

  //MessageBoxA(NULL, "Pipe server started.", "Info", MB_OK);

  InitializeFunctionMap();

  while (ReadFile(hPipe, buffer, sizeof(buffer) - 1, &dwRead, NULL) != FALSE) {
    buffer[dwRead] = '\0';

    char* intent = reinterpret_cast<char*>(buffer);
    int len = strlen(intent) + 1;
    
    HandleRequest(intent, buffer + len);
    WriteFile(hPipe, intent, len, &dwWritten, NULL);
  }

  DisconnectNamedPipe(hPipe);
  CloseHandle(hPipe);
  return true;
}
