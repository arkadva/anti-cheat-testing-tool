#ifndef EVENTINPUTSYNTHESIZER_H_
#define EVENTINPUTSYNTHESIZER_H_

#include <Windows.h>
#include "../../Base/Modules/module.h"
#include "../../Utils/utilities.h"

BOOL KeyboardEvent(INPUT* input, UINT count);
BOOL MouseEvent(INPUT* input, UINT count);

class EventInputSynthesizer : public Module {
public:
  EventInputSynthesizer(INPUT* input, UINT keyboard_count, INPUT* mouse_event, UINT mouse_count)
    :input_(input), keyboard_count_(keyboard_count), mouse_event_(mouse_event), mouse_count_(mouse_count) {
    module_name = "Event Input Synthesizer";
  }

  virtual bool execute(const Process* process) const override;

private:
  INPUT* input_;
  const UINT keyboard_count_;
  INPUT* mouse_event_;
  const UINT mouse_count_;
};

#endif
