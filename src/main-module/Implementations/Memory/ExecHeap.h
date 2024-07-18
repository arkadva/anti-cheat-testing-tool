#ifndef EXECHEAP_H_
#define EXECHEAP_H_

#include "../../Base/module.h"
#include "../../Base/memoryaccess.h"

BOOL WriteAndExec(DWORD pid, LPCSTR shellcode, SIZE_T size);

class ExecHeap : public Module {
public:
	ExecHeap(Variable* shellcode, size_t size) 
	: shellcode_(shellcode), size_(size) {
		module_name = "Execute Heap";
	}

	virtual bool execute(const Process* process) const override {
		char* byte_array = shellcode_->as<char*>();
		int size = strlen(byte_array);

		std::vector<char> shellcode;
		shellcode.assign(byte_array, byte_array + size);

		BOOL result = WriteAndExec(process->GetPid(), shellcode.data(), shellcode.size());

		return TRUE;
	}

private:
	Variable* shellcode_;
	size_t size_;
};
#endif
