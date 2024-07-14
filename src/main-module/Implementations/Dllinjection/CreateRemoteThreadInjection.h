#ifndef CREATEREMOTETHREADINJECTION_H_
#define CREATEREMOTETHREADINJECTION_H_

#include <iostream>
#include "../../Base/module.h"
#include "../../Utils/utilities.h"

int injectIntoPID(DWORD pid, BYTE method, const wchar_t* dll);

// bitflgs for injection type
enum RemoteThreadType {
	kNtCreateThreadEx = 1 << 0,
	kRtlCreateUserThread = 1 << 1,
	kCreateRemoteThread = 1 << 2
};

class CreateRemoteThreadInjection : public Module {
public:
  CreateRemoteThreadInjection(Variable* path, Variable* attacks) 
	: path_(path), attacks_(attacks) {
		module_name = "CreateRemoteThread DLL injection";
	}

  virtual bool execute(const Process* process) const override {
		DWORD pid = process->GetPid();
		DWORD attacks = attacks_->as<DWORD>();
		wchar_t* path = path_->as<wchar_t*>();

		int result = true;

		if (attacks & kNtCreateThreadEx) {
			result &= injectIntoPID(pid, 1, path);
		}

		if (attacks & kRtlCreateUserThread) {
			result &= injectIntoPID(pid, 2, path);
		}

		if (attacks & kCreateRemoteThread) {
			result &= injectIntoPID(pid, 3, path);
		}

		return result;
  }
private:
	Variable* path_;
	Variable* attacks_;
};
#endif
