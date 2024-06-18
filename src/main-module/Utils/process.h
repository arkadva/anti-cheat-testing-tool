#ifndef PROCESS_H_
#define PROCESS_H_

#include <windows.h>
#include <string>
#include <vector>

class Process {
public:
  Process(const std::wstring& path, const std::vector<std::wstring>& args, const std::wstring& working_directory_);
  Process(const std::wstring& path, const DWORD pid);
  DWORD GetPid() const;
  const std::wstring& GetPath() const;

  BOOL StartProcess();
  BOOL KillProcess();
  BOOL RestartProcess();
private:
  const std::vector<std::wstring> args_;
  const std::wstring working_directory_;
  const std::wstring path_;
  DWORD pid_;
};

#endif
