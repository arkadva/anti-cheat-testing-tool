#include "BreakpointHook.h"

BOOL BreakpointHookImp(PipeClient* client, BreakpointHookData* hookData, ContextChangeEntry* entryData, HookType type) {
  const char* str = (type == kHardwareBreakpoint) ? "HardwareHook" : "PageGuardHook";
  size_t size = strlen(str) + 1 + sizeof(BreakpointHookData) + sizeof(ContextChangeEntry);
  BYTE* buffer = reinterpret_cast<BYTE*>(malloc(size));

  memcpy(buffer, str, strlen(str) + 1);
  memcpy(buffer + strlen(str) + 1, hookData, sizeof(BreakpointHookData));
  memcpy(buffer + strlen(str) + 1 + sizeof(ContextChangeEntry), entryData, sizeof(ContextChangeEntry));

  client->connect();
  client->write(buffer, size);

  return TRUE;
}
