#include "EventInputSynthesizer.h"

bool EventInputSynthesizer::execute(const Process* process) const {
  HWND hwnd = utilities::FindWindowByPID(process->GetPid());

  if (hwnd == NULL) {
    return FALSE;
  }

  SetForegroundWindow(hwnd);
  SetFocus(hwnd);
  ShowWindow(hwnd, SW_SHOWNORMAL);
  BringWindowToTop(hwnd);

  BOOL status = TRUE;

  if (input_) {
    status &= KeyboardEvent(input_, keyboard_count_);
  }

  if (mouse_event_) {
    status &= MouseEvent(mouse_event_, mouse_count_);
  }

  return status;
}

BOOL KeyboardEvent(INPUT* input, UINT count) {
  for (UINT i = 0; i < count; i++) {
    keybd_event(input[i].ki.wVk, input[i].ki.wScan, input[i].ki.dwFlags, input[i].ki.time);
  }
  return TRUE;
}

BOOL MouseEvent(INPUT* input, UINT count) {
  for (UINT i = 0; i < count; ++i) {
    mouse_event(input[i].mi.dwFlags, input[i].mi.dx, input[i].mi.dy, input[i].mi.mouseData, input[i].mi.time);
  }
  return TRUE;
}
