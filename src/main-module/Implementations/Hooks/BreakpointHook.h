#ifndef BREAKPOINTHOOK_H_
#define BREAKPOINTHOOK_H_

#include "../../Base/module.h"
#include "../../Utils/pipeclient.h"

enum HookType {
	kHardwareBreakpoint = 1 << 0,
	kPageGuard = 1 << 1
};

BOOL BreakpointHookImp(PipeClient* client, BreakpointHookData* hookData, ContextChangeEntry* entryData, HookType type);

class BreakpointHook : public Module {
public:
	BreakpointHook(PipeClient* client, _BreakpointHookData entry, _ContextChangeEntry* entryData, HookType type)
	: client_(client), entry_(entry), entryData_(entryData), type_(type) {
		module_name = type == kHardwareBreakpoint ? "Hardware Breakpoint Hook" : "Page Guard Hook";
	}

	virtual bool execute(const Process* process) const override {
		BreakpointHookData* entry = new BreakpointHookData();
		entry->address = entry_.address->as<void*>();
		entry->entriesSize = entry_.entriesSize;

		ContextChangeEntry* entryData = static_cast<ContextChangeEntry*>(malloc(entry->entriesSize * sizeof(ContextChangeEntry)));

		if (!entryData) return FALSE;

		for (int i = 0; i < entry->entriesSize; i++) {
			ContextChangeEntry t = { entryData_[i].value->as<void*>(), entryData_[i].offset };
			entryData[i] = t;
		}

		BOOL result = BreakpointHookImp(client_, entry, entryData, type_);

		return result;
	}

public:
	PipeClient* client_;
	_BreakpointHookData entry_;
	_ContextChangeEntry* entryData_;
	const HookType type_;
};
#endif
