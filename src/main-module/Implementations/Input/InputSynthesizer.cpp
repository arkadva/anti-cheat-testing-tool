#include "InputSynthesizer.h"

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

bool InputSynthesizer::execute(const Process* process) const {
  HWND hwnd = utilities::FindWindowByPID(process->GetPid());

  if (hwnd == NULL) {
    return FALSE;
  }

  // ensure the input is applied to the game window
  SetForegroundWindow(hwnd);
  SetFocus(hwnd);
  ShowWindow(hwnd, SW_SHOWNORMAL);
  BringWindowToTop(hwnd);

  BOOL status = TRUE;

  if (type_ & kSetCursor) {
    status &= SetCursorPos(isi_->x, isi_->y);
  }

  if (type_ & kSendInput) {
    status &= (SendInput(isi_->inputCount, isi_->input, sizeof(INPUT) == isi_->inputCount));
  }

  if (type_ & kMouseEvent) {
    status &= MouseEvent(isi_->input, isi_->inputCount);
  }

  if (type_ & kKeyboardEvent) {
    status &= KeyboardEvent(isi_->input, isi_->inputCount);
  }

  return status;
}