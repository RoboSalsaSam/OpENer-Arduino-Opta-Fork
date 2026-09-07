#include <stdio.h>
#include <errno.h>
extern "C" {
#include "opener_error.h"
}

extern "C" int GetSocketErrorNumber(void) { return errno; }
extern "C" char *GetErrorMessage(int error_number) {
  static char text[64];
  snprintf(text, sizeof(text), "Mbed socket error %d", error_number);
  return text;
}
extern "C" void FreeErrorMessage(char *) {}
