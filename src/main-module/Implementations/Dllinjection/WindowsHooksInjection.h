#ifndef WINDOWSHOOKSINJECTION_H_
#define WINDOWSHOOKSINJECTION_H_

#include "../../Base/Modules/dllinjection.h"
#include "../../Utils/utilities.h"

int WindowsHooksInjectionImp(DWORD pid, const std::wstring& dll_name);

class WindowsHooksInjection : public DLLInjection {
public:
	WindowsHooksInjection(const std::wstring& path) : DLLInjection(path) {
		module_name = "SetWindowsHookEx Injection";
	}

	virtual bool execute(const Process* process) const override {
		BOOL result = WindowsHooksInjectionImp(process->GetPid(), path);
		return TRUE;
	}
};
#endif

