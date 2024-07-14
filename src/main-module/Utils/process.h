#ifndef PROCESS_H_
#define PROCESS_H_

#include <windows.h>
#include <string>
#include <vector>

class Process {
public:
  Process(const std::wstring& path, const std::wstring& args, const std::wstring& working_directory);
  Process(const std::wstring& path, const DWORD pid);
  DWORD GetPid() const;
  const std::wstring& GetPath() const;

  BOOL StartProcess();
  BOOL KillProcess();
  BOOL RestartProcess();
private:
  const std::wstring path_;
  const std::wstring args_;
  const std::wstring working_directory_;
  DWORD pid_;
};

#endif
