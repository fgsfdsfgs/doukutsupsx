#pragma once

#include "engine/common.h"

#define TIMER_RATE 100

typedef void (*timer_func_t)(void);
extern timer_func_t timer_callback;

// global timer; ticks at 100hz
extern volatile u32 timer_ticks;
// next music tick
extern volatile u32 timer_org_nexttick;
// music tickrate
extern volatile u32 timer_org_delay;

void timer_init(void);
