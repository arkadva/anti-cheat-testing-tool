#ifndef SENDINPUTSYNTHESIZER_H_
#define SENDINPUTSYNTHESIZER_H_

#include <Windows.h>
#include "../../Base/Modules/module.h"
#include "../../Utils/utilities.h"

BOOL SynthesizeInput(INPUT* input, UINT count);
BOOL SetCursor(LONG x, LONG y);

class SendInputSynthesizer : public Module {
public:
  SendInputSynthesizer(INPUT* input, UINT count, LONG x, LONG y)
    : input_(input), count_(count), x_(x), y_(y) {
    module_name = "Send Input Synthesizer";
  }
  virtual bool execute(const Process* process) const override;

private:
  INPUT* input_;
  const UINT count_;
  const LONG x_;
  const LONG y_;
};

#endif
