#ifndef INPUTSYNTHESIZER_H_
#define INPUTSYNTHESIZER_H_

#include <Windows.h>
#include "../../Base/module.h"
#include "../../Utils/utilities.h"

struct InputSynthesizerInformation {
  INPUT* input;
  UINT inputCount;
  LONG x;
  LONG y;
};

enum InputSynthesizerType {
  kSetCursor = 1 << 0,
  kSendInput = 1 << 1,
  kKeyboardEvent = 1 << 2,
  kMouseEvent = 1 << 3
};

class InputSynthesizer : public Module {
public:
  InputSynthesizer(InputSynthesizerInformation* isi, InputSynthesizerType type) : isi_(isi), type_(type) {
    module_name = "Input Synthesizer";
  }

  virtual bool execute(const Process* process) const override;
private:
  InputSynthesizerInformation* isi_;
  InputSynthesizerType type_;
};

#endif
