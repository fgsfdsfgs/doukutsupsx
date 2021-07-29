#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <psxgpu.h>
#include <psxapi.h>
#include <psxetc.h>

#include "engine/common.h"
#include "spu.h"

#define MAX_ERROR 512
#define TIMER_RATE 100

volatile u32 timer_ticks;

static void timer_cb(void) {
  ++timer_ticks;
}

void timer_start(void) {
  timer_ticks = 0;
  EnterCriticalSection();
  const u32 dt = 15625 / TIMER_RATE;
  SetRCnt(RCntCNT1, dt, RCntMdINTR);
  InterruptCallback(5, timer_cb); // IRQ5 is RCNT1
  StartRCnt(RCntCNT1);
  ChangeClearRCnt(1, 0);
  ExitCriticalSection();
}

void panic(const char *error, ...) {
  char buf[MAX_ERROR];

  va_list args;
  va_start(args, error);
  vsnprintf(buf, sizeof(buf), error, args);
  va_end(args);

  // spew to TTY
  printf("ERROR: %s\n", buf);

  // setup graphics viewport and clear screen
  SetDispMask(0);
  DISPENV disp;
  DRAWENV draw;
  SetDefDispEnv(&disp, 0, 0, VID_WIDTH, VID_HEIGHT);
  SetDefDrawEnv(&draw, 0, 0, VID_WIDTH, VID_HEIGHT);
  setRGB0(&draw, 0x40, 0x00, 0x00); draw.isbg = 1;
  PutDispEnv(&disp);
  PutDrawEnv(&draw);

  // load built in font
  FntLoad(960, 0);
  FntOpen(0, 8, 320, 224, 0, MAX_ERROR);

  // draw
  FntPrint(-1, "ERROR:\n%s", buf);
  FntFlush(-1);
  DrawSync(0);
  VSync(0);
  SetDispMask(1);

  while (1) VSync(0);
}
