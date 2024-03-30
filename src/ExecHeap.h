#ifndef EXECHEAP_H_
#define EXECHEAP_H_

#include "memoryaccess.h"

class ExecHeap : public MemoryAccess {
public:
	ExecHeap(LPVOID address) : MemoryAccess(address) {}

	virtual bool execute(const Process* process) const override;
};
#endif

