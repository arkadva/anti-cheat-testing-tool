#ifndef EXECHEAP_H_
#define EXECHEAP_H_

#include "../../Base/module.h"
#include "../../Base/memoryaccess.h"

BOOL WriteAndExec(DWORD pid, LPVOID address, LPCSTR shellcode, SIZE_T size);

class ExecHeap : public Module {
public:
	ExecHeap(LPVOID address, const std::vector<char>& shellcode) {
		module_name = "Execute Heap";
	}

	virtual bool execute(const Process* process) const override {
		/*
		BOOL result = WriteAndExec(process->GetPid(), address_, shellcode_.data(), shellcode_.size());
		*/
		BOOL result = TRUE;
		return result;
	}

private:
/*
	const std::vector<char>& shellcode_;
*/
};
#endif
