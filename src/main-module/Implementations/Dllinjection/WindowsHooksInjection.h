#ifndef WINDOWSHOOKSINJECTION_H_
#define WINDOWSHOOKSINJECTION_H_

#include "../../Base/module.h"
#include "../../Utils/utilities.h"

int WindowsHooksInjectionImp(DWORD pid, const std::wstring& dll_name);

class WindowsHooksInjection : public Module {
public:
	WindowsHooksInjection(Variable* path) 
	: path_(path) {
		module_name = "SetWindowsHookEx Injection";
	}

	virtual bool execute(const Process* process) const override {
		BOOL result = WindowsHooksInjectionImp(process->GetPid(), path_->as<wchar_t*>());

		return TRUE;
	}
private:
	Variable* path_;
};
#endif
