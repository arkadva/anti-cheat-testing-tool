#ifndef MANUALMAPPING_H_
#define MANUALMAPPING_H_

#include "dllinjection.h"

int ManualMappingInjection(DWORD pid, const std::wstring& path);

class ManualMapping : public DLLInjection {
public:
	ManualMapping(const std::wstring& path) : DLLInjection(path) {
		module_name = "Manual Mapping DLL injection";
	}

	virtual bool execute(const Process* process) const override {
		BOOL result = ManualMappingInjection(process->GetPid(), path);
		return result;
	}
};
#endif

