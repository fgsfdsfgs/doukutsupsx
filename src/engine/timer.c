#include <sys/types.h>
#include <libapi.h>
#include <libetc.h>

#include "engine/common.h"
#include "engine/org.h"
#include "engine/timer.h"

#define RATE_HBLANK 15625
#define RATE_SYSCLK8 4233600 // ~(1sec / 0.23620559334us)

// these are undeclared in psyq
extern void *InterruptCallback(int irq, void (*func)(void));
extern void ChangeClearRCnt(int t, int m);

// global timer; ticks at 100hz
volatile u32 timer_ticks;
// next music tick
volatile u32 timer_org_nexttick;
// music tickrate
volatile u32 timer_org_delay;

static void timer_cb(void) {
  const u32 now = ++timer_ticks;
  if (now >= timer_org_nexttick && timer_org_delay) {
    org_tick();
    timer_org_nexttick = now + timer_org_delay;
  }
}

void timer_init(void) {
  timer_ticks = 0;
  timer_org_nexttick = 0;

  const u32 dt = RATE_SYSCLK8 / TIMER_RATE;

  EnterCriticalSection();
  SetRCnt(RCntCNT2, dt, RCntMdINTR);
  InterruptCallback(6, timer_cb); // IRQ6 is RCNT2
  StartRCnt(RCntCNT2);
  ChangeClearRCnt(2, 0);
  ExitCriticalSection();
}
