#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <kernel.h>
#include <libapi.h>
#include <libetc.h>
#include <libgte.h>
#include <libgpu.h>

#include "engine/common.h"
#include "engine/exception.h"

static unsigned long exception_event;

static inline const char *exception_cause(const unsigned long cr) {
  static const char *msg[] = {
    "EXT INT",
    "?",
    "?",
    "?",
    "READ ERR",
    "WRITE ERR",
    "CMD BUS ERR",
    "DATA BUS ERR",
    "SYSCALL",
    "BREAK",
    "UNK COMMAND",
    "NO COP",
    "OVERFLOW",
  };
  return msg[(cr & 63) >> 2];
}

static long exception_cb(void) {
  // get current thread's TCB
  const struct ToT *tot = (const struct ToT *)0x100;
  const struct TCB *tcb_list = (const struct TCB *)tot[2].head;
  const unsigned long status = tcb_list[0].status;
  const unsigned long cr = GetCr();
  const unsigned long *regs = tcb_list[0].reg;

  // spew to tty
  printf("UH OH ZONE\nSTATUS=%08x\nCR=%08x\nPC=%08x\nRA=%08x\n", status, cr, regs[R_EPC], regs[R_RA]); 

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
  long stream = FntOpen(8, 16, VID_WIDTH - 8, VID_HEIGHT - 16, 0, 800);

  // the most important info
  const char *cause = exception_cause(cr);
  FntPrint(stream, "UH OH ZONE\n\n");
  FntPrint(stream, "CAUSE:  %s (%08x)\nSTATUS: %08x\n\n", cause, cr, status);
  FntPrint(stream, "PC=%08x  RA=%08x\n\n", regs[R_EPC],regs[R_RA]);

  // GPR dump
  FntPrint(stream, "AT=%08x  K0=%08x  K1=%08x\n", regs[R_AT],  regs[R_K0],  regs[R_K1]);
  FntPrint(stream, "A0=%08x  A1=%08x  A2=%08x\n", regs[R_A0],  regs[R_A1],  regs[R_A2]);
  FntPrint(stream, "A3=%08x  V0=%08x  V1=%08x\n", regs[R_A3],  regs[R_V0],  regs[R_V1]);
  FntPrint(stream, "T0=%08x  T1=%08x  T2=%08x\n", regs[R_T0],  regs[R_T1],  regs[R_T2]);
  FntPrint(stream, "T3=%08x  T4=%08x  T5=%08x\n", regs[R_T3],  regs[R_T4],  regs[R_T5]);
  FntPrint(stream, "T6=%08x  T7=%08x  T8=%08x\n", regs[R_T6],  regs[R_T7],  regs[R_T8]);
  FntPrint(stream, "T9=%08x  S0=%08x  S1=%08x\n", regs[R_T9],  regs[R_S0],  regs[R_S1]);
  FntPrint(stream, "S2=%08x  S3=%08x  S4=%08x\n", regs[R_S2],  regs[R_S3],  regs[R_S4]);
  FntPrint(stream, "S5=%08x  S6=%08x  S7=%08x\n", regs[R_S5],  regs[R_S6],  regs[R_S7]);
  FntPrint(stream, "GP=%08x  FP=%08x  SP=%08x\n", regs[R_GP],  regs[R_FP],  regs[R_SP]);

  // dump stack if possible
  const u32 sp = ALIGN(regs[R_SP], 16);
  if (sp > 0x80100000 && sp < 0x80200000) {
    FntPrint(stream, "\nSTACK\n\n");
    const u32 *data = (const u32 *)sp;
    for (u32 i = 0; data < (const u32 *)0x80200000 && i < 6; data += 4, ++i)
      FntPrint(stream, "%02x %08x %08x %08x %08x\n", (u32)data & 0xFF, data[0], data[1], data[2], data[3]);
  }

  FntFlush(stream);
  SetDispMask(1);

  while (1);

  return 0; // never gets here
}

void ex_install_handler(void) {
  // HwCPU fires when there's an unhandled CPU exception
  EnterCriticalSection();
  exception_event = OpenEvent(HwCPU, EvSpTRAP, EvMdINTR, exception_cb);
  EnableEvent(exception_event);
  ExitCriticalSection();
  printf("ex_install_handler(): opened HwCPU event %08x\n", exception_event);

  /*
  // alternatively:
  // A(40h) - SystemErrorUnresolvedException()
  void **a0tab = (void **)0x200;
  const void *old = a0tab[0x40];
  a0tab[0x40] = exception_cb;
  printf("ex_install_handler(): hooked exception handler: %p -> %p\n", old, exception_cb);
  */
}
