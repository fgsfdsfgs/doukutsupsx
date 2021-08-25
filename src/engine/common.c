#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <sys/types.h>
#include <libetc.h>
#include <libgte.h>
#include <libgpu.h>

#include "engine/common.h"
#include "spu.h"

// error message buffer
char error_msg[MAX_ERROR];

void do_panic(void) {
  // spew to TTY
  printf("ERROR: %s\n", error_msg);

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
  long stream = FntOpen(8, 16, VID_WIDTH - 8, VID_HEIGHT - 16, 0, MAX_ERROR);

  // draw
  FntPrint(stream, "ERROR:\n%s", error_msg);
  FntFlush(stream);
  DrawSync(0);
  VSync(0);
  SetDispMask(1);

  while (1) VSync(0);
}
