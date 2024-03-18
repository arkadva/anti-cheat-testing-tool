#ifndef CREATEREMOTETHREADINJECTION_H_
#define CREATEREMOTETHREADINJECTION_H_

#include <iostream>
#include "dllinjection.h"
#include "utilities.h"

// ========== Implementation Definitions Start ==========
HANDLE RtlCreateUserThread(
	HANDLE hProcess,
	LPVOID lpBaseAddress,
	LPVOID lpSpace
);

HANDLE NtCreateThreadEx(
	HANDLE hProcess,
	LPVOID lpBaseAddress,
	LPVOID lpSpace
);

int injectIntoPID(int process, int method, const wchar_t* dll);
// ========== Implementation Definitions End ==========


// bitflgs for injection type
enum RemoteThreadType {
	kNtCreateThreadEx = 0x1,
	kRtlCreateUserThread = 0x2,
	kCreateRemoteThread = 0x4
};


class CreateRemoteThreadInjection : public DLLInjection {
public:
  CreateRemoteThreadInjection(const std::wstring& path, BYTE attacks) : DLLInjection(path), _attacks(attacks) { }

	// TODO: missing dll unload mechanism
	// TODO: add logging after each injection (success/failure)
  virtual bool execute(const Process* process) const override {
		DWORD pid = process->GetPid();

		int result;

		if (_attacks & kNtCreateThreadEx) {
			result = injectIntoPID(pid, 1, path.c_str());
		}

		if (_attacks & kRtlCreateUserThread) {
			result = injectIntoPID(pid, 2, path.c_str());
		}

		if (_attacks & kCreateRemoteThread) {
			result = injectIntoPID(pid, 3, path.c_str());
		}

    return true;
  }
private:
	const BYTE _attacks;
};
#endif