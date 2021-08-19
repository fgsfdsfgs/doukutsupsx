#include <sys/types.h>
#include <libapi.h>
#include <libetc.h>

#include "engine/common.h"
#include "engine/org.h"
#include "engine/timer.h"

// these are undeclared in psyq
extern void *InterruptCallback(int irq, void (*func)(void));
extern void ChangeClearRCnt(int t, int m);

timer_func_t timer_callback = NULL;

volatile u32 timer_ticks;
volatile u32 timer_next_orgtick;

static volatile u32 timer_orgwait = 0;

void timer_cb_ticker(void) {
  ++timer_ticks;
}

void timer_cb_music(void) {
  const u32 now = ++timer_ticks;
  if (now >= timer_next_orgtick && timer_orgwait) {
    org_tick();
    timer_next_orgtick = now + timer_orgwait;
  }
}

void timer_init(void) {
  timer_ticks = 0;
  EnterCriticalSection();
  const u32 dt = 15625 / TIMER_RATE;
  SetRCnt(RCntCNT1, dt, RCntMdINTR);
  InterruptCallback(5, timer_cb_ticker); // IRQ5 is RCNT1
  StartRCnt(RCntCNT1);
  ChangeClearRCnt(1, 0);
  ExitCriticalSection();
}

void timer_set_callback(timer_func_t cb) {
  EnterCriticalSection();
  timer_orgwait = org_get_wait() / 10;
  InterruptCallback(5, cb); // IRQ5 is RCNT1
  ExitCriticalSection();
  timer_callback = cb;
}
