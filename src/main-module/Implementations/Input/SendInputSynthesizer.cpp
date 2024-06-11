#include "SendInputSynthesizer.h"

bool SendInputSynthesizer::execute(const Process* process) const {
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
    status &= SynthesizeInput(input_, count_);
  }


  if (x_ && y_) {
    status &= SetCursor(x_, y_);
  }

  return status;
}

BOOL SynthesizeInput(INPUT* input, UINT count) {
  UINT sent = SendInput(count, input, sizeof(INPUT));
  std::cout << sent << std::endl;
  return sent == count;
}

BOOL SetCursor(LONG x, LONG y) {
  return SetCursorPos(x, y);
}