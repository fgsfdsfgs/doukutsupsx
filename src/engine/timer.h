#pragma once

#include "engine/common.h"

#define TIMER_RATE 100

typedef void (*timer_func_t)(void);

// global timer; ticks at 100hz
extern volatile u32 timer_ticks;
// next music tick
extern volatile u32 timer_next_orgtick;

void timer_cb_ticker(void);
void timer_cb_music(void);

void timer_init(void);
void timer_set_callback(timer_func_t cb);
