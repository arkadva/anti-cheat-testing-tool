#ifndef MANUALMAPPING_H_
#define MANUALMAPPING_H_

#include "../../Base/module.h"
#include "../../Utils/utilities.h"

int ManualMappingInjection(DWORD pid, const std::wstring& path);

class ManualMapping : public Module {
public:
	ManualMapping(Variable* path)
	: path_(path) {
		module_name = "Manual Mapping DLL injection";
	}

	virtual bool execute(const Process* process) const override {
		BOOL result = ManualMappingInjection(process->GetPid(), path_->as<wchar_t*>());

		return result;
	}
private:
	Variable* path_;
};
#endif
