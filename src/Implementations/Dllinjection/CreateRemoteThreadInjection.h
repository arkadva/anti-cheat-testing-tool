#ifndef CREATEREMOTETHREADINJECTION_H_
#define CREATEREMOTETHREADINJECTION_H_

#include <iostream>
#include "../../Base/Modules/dllinjection.h"
#include "../../Utils/utilities.h"

int injectIntoPID(DWORD pid, BYTE method, const wchar_t* dll);

// bitflgs for injection type
enum RemoteThreadType {
	kNtCreateThreadEx = 1 << 0,
	kRtlCreateUserThread = 1 << 1,
	kCreateRemoteThread = 1 << 2
};

class CreateRemoteThreadInjection : public DLLInjection {
public:
  CreateRemoteThreadInjection(const std::wstring& path, BYTE attacks) : DLLInjection(path), _attacks(attacks) {
		module_name = "CreateRemoteThread DLL injection";
	}

	// TODO: missing dll unload mechanism
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
