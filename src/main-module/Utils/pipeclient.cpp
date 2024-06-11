#include <iostream>
#include <thread>
#include <chrono>
#include "PipeClient.h"

PipeClient::PipeClient(const std::string& pipeName) : pipe_(INVALID_HANDLE_VALUE), pipe_name_(pipeName) {}

PipeClient::~PipeClient() {
  close();
}

bool PipeClient::connect() {
  const int max_attempts = 3;
  const int timeout_seconds = 3;

  for (int attempt = 1; attempt <= max_attempts; ++attempt) {
    pipe_ = CreateFileA(pipe_name_.c_str(),
      GENERIC_READ | GENERIC_WRITE,
      0,
      NULL,
      OPEN_EXISTING,
      0,
      NULL);

    if (pipe_ != INVALID_HANDLE_VALUE) {
      std::cout << "Successfully connected to pipe on attempt " << attempt << "." << std::endl;
      return true;
    }

    std::cerr << "Attempt " << attempt << " failed to connect to pipe. Error: " << GetLastError() << std::endl;

    if (attempt < max_attempts) {
      std::this_thread::sleep_for(std::chrono::seconds(timeout_seconds));
    }
  }

  std::cerr << "Failed to connect to pipe after " << max_attempts << " attempts." << std::endl;
  return false;
}

bool PipeClient::write(BYTE* message, int size) {
  DWORD dwWritten;
  if (!WriteFile(pipe_, message, size, &dwWritten, NULL)) {
    std::cerr << "Write failed with error: " << GetLastError() << std::endl;
    return false;
  }
  return true;
}

std::string PipeClient::read() {
  std::string output;

  DWORD dwRead;
  if (ReadFile(pipe_, &output, sizeof(output), &dwRead, NULL)) {
    return output;
  }
  else {
    std::cerr << "Read failed with error: " << GetLastError() << std::endl;
  }


  return output;
}

void PipeClient::close() {
  if (pipe_ != INVALID_HANDLE_VALUE) {
    CloseHandle(pipe_);
    pipe_ = INVALID_HANDLE_VALUE;
  }
}
