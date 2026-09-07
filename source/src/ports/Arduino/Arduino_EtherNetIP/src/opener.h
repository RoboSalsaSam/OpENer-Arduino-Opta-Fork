//opener.h changes the original STM32/lwIP startup interface into the public Arduino Opta API used by the .ino example.

#ifndef OPTA_OPENER_H_
#define OPTA_OPENER_H_
#include <stdbool.h>
#ifdef __cplusplus
extern "C" {
#endif
void opener_init(void);
const char *opener_process(void);
const char *opener_last_status(void);
bool opener_running(void);
#ifdef __cplusplus
}
#endif
#endif
