#ifndef PIPECLIENT_H
#define PIPECLIENT_H

#include <windows.h>
#include <string>

class PipeClient {
public:
  PipeClient(const std::string& pipeName);
  ~PipeClient();

  bool connect();
  bool write(const std::string& message);
  std::string read();
  void close();

private:
  HANDLE pipe_;
  std::string pipe_name_;
};

#endif
