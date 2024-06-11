#ifndef BREAKPOINTHOOK_H_
#define BREAKPOINTHOOK_H_

#include "../../Base/Modules/module.h"
#include "../../Utils/pipeclient.h"

enum HookType {
	kHardwareBreakpoint = 1 << 0,
	kPageGuard = 1 << 1
};

BOOL BreakpointHookImp(PipeClient* client, BreakpointHookData* hookData, ContextChangeEntry* entryData, HookType type);

class BreakpointHook : public Module {
public:
	BreakpointHook(PipeClient* client, BreakpointHookData* entry, ContextChangeEntry* entryData, HookType type)
	: client_(client), entry_(entry), entryData_(entryData), type_(type) {
		module_name = type == kHardwareBreakpoint ? "Hardware Breakpoint Hook" : "Page Guard Hook";
	}

	virtual bool execute(const Process* process) const override {
		BOOL result = BreakpointHookImp(client_, entry_, entryData_, type_);
		return result;
	}

public:
	PipeClient* client_;
	BreakpointHookData* entry_;
	ContextChangeEntry* entryData_;
	const HookType type_;
};
#endif
