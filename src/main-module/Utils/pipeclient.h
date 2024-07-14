#ifndef PIPECLIENT_H
#define PIPECLIENT_H

#include <windows.h>
#include <string>
#include "../Base/variable.h"

struct BreakpointHookData {
  void* address;
  int entriesSize;
};

struct ContextChangeEntry {
  void* value;
  int offset;
};

// resolves to BreakpointHookData before execution
struct _BreakpointHookData {
  Variable* address;
  int entriesSize;
};

// resolves to ContextChangeEntry before execution
struct _ContextChangeEntry {
  Variable* value;
  int offset;
};

class PipeClient {
public:
  PipeClient(const std::string& pipeName);
  ~PipeClient();

  bool connect();
  bool write(BYTE* message, int size);
  std::string read();
  void close();

private:
  HANDLE pipe_;
  std::string pipe_name_;
};

#endif
