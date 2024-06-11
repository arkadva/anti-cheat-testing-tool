#ifndef PIPECLIENT_H
#define PIPECLIENT_H

#include <windows.h>
#include <string>

struct BreakpointHookData {
  void* address;
  int entriesSize;
};

struct ContextChangeEntry {
  void* value;
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
