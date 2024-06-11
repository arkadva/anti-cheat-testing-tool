#ifndef STRUCTDEFINITIONS_H_
#define STRUCTDEFINITIONS_H_

struct BreakpointHookData {
  void* address;
  int entriesSize;
};

struct ContextChangeEntry {
  void* value;
  int offset;
};


#endif
