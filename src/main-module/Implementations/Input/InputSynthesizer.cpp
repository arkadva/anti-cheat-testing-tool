 #include "InputSynthesizer.h"

BOOL KeyboardEvent(INPUT* input) {
  keybd_event(input->ki.wVk, input->ki.wScan, input->ki.dwFlags, input->ki.time);
  return TRUE;
}

BOOL MouseEvent(INPUT* input) {
  mouse_event(input->mi.dwFlags, input->mi.dx, input->mi.dy, input->mi.mouseData, input->mi.time);
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
    status &= SetCursorPos(isi_->x->as<LONG>(), isi_->y->as<LONG>());
  }

  if (type_ & kSendInput) {
    status &= (SendInput(1, isi_->input, sizeof(INPUT) == 1));
  }

  if (type_ & kMouseEvent) {
    status &= MouseEvent(isi_->input);
  }

  if (type_ & kKeyboardEvent) {
    status &= KeyboardEvent(isi_->input);
  }

  return status;
}