#ifndef MANUALMAPPING_H_
#define MANUALMAPPING_H_

#include "dllinjection.h"

bool ManualMapDll(HANDLE hProc, BYTE* pSrcData, SIZE_T FileSize, bool ClearHeader = true, bool ClearNonNeededSections = true, bool AdjustProtections = true, bool SEHExceptionSupport = true, DWORD fdwReason = DLL_PROCESS_ATTACH, LPVOID lpReserved = 0);
int ManualMappingInjection(DWORD pid, const std::wstring& path);

class ManualMapping : public DLLInjection {
public:
	ManualMapping(const std::wstring& path) : DLLInjection(path) { }

	virtual bool execute(const Process* process) const override {
		DWORD pid = process->GetPid();

		int result = ManualMappingInjection(pid, path);
		return result;
	}
};
#endif

