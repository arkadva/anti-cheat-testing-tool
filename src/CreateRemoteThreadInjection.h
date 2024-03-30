#ifndef CREATEREMOTETHREADINJECTION_H_
#define CREATEREMOTETHREADINJECTION_H_

#include <iostream>
#include "dllinjection.h"
#include "utilities.h"

int injectIntoPID(int process, int method, const wchar_t* dll);

// bitflgs for injection type
enum RemoteThreadType {
	kNtCreateThreadEx = 0,
	kRtlCreateUserThread = 1 << 0,
	kCreateRemoteThread = 1 << 1
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
