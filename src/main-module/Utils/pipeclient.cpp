#include <iostream>
#include "PipeClient.h"

PipeClient::PipeClient(const std::string& pipeName) : pipe_(INVALID_HANDLE_VALUE), pipe_name_(pipeName) {}

PipeClient::~PipeClient() {
  close();
}

bool PipeClient::connect() {
  pipe_ = CreateFileA(pipe_name_.c_str(),
    GENERIC_READ | GENERIC_WRITE,
    0,
    NULL,
    OPEN_EXISTING,
    0,
    NULL);
  if (pipe_ == INVALID_HANDLE_VALUE) {
    std::cerr << "Failed to connect to pipe. Error: " << GetLastError() << std::endl;
    return false;
  }
  return true;
}

bool PipeClient::write(const std::string& message) {
  DWORD dwWritten;
  if (!WriteFile(pipe_, message.c_str(), message.size() + 1, &dwWritten, NULL)) {
    std::cerr << "Write failed with error: " << GetLastError() << std::endl;
    return false;
  }
  return true;
}

std::string PipeClient::read() {
  char buffer[1024];
  DWORD dwRead;
  if (ReadFile(pipe_, buffer, sizeof(buffer), &dwRead, NULL)) {
    return std::string(buffer, dwRead);
  }
  else {
    std::cerr << "Read failed with error: " << GetLastError() << std::endl;
    return "";
  }
}

void PipeClient::close() {
  if (pipe_ != INVALID_HANDLE_VALUE) {
    CloseHandle(pipe_);
    pipe_ = INVALID_HANDLE_VALUE;
  }
}
