#ifndef TIMER_H
#define TIMER_H
#include <windows.h>

typedef void (* TIMER_CALL_BACK_PROC)(void *);

void timeRun(TIMER_CALL_BACK_PROC proc, void *lparam, int intervalSecs);

#endif