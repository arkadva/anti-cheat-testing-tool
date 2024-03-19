#ifndef PROCESS_H
#define PROCESS_H

#include <windows.h>
#include <string>
#include <vector>

class Process {
public:
  Process(const std::wstring& path);
  Process(const DWORD pid);
  DWORD GetPid() const;

private:
  DWORD _pid;
};

#endif
