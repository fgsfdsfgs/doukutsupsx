#include <psxpad.h>
#include <psxapi.h>

#include "engine/common.h"
#include "engine/input.h"

// IN_ flags of actions currently held
u32 input_held;
// IN_ flags of actions pressed this frame
u32 input_trig;
// IN_ flags of actions held on previous frame
static u32 input_old;

static u8 padbuf[2][34];

// pad->action bindings
static u32 binds[IN_NUM_ACTIONS] = {
  PAD_LEFT,     // IN_LEFT = 1
  PAD_RIGHT,    // IN_RIGHT = 2
  PAD_UP,       // IN_UP = 4
  PAD_DOWN,     // IN_DOWN = 8
  PAD_SQUARE,   // IN_JUMP = 16
  PAD_CROSS,    // IN_FIRE = 32
  PAD_TRIANGLE, // IN_SWAP_L = 64
  PAD_CIRCLE,   // IN_SWAP_R = 128
  PAD_R2,       // IN_INVENTORY = 256
  PAD_R1,       // IN_MAP = 512
  PAD_START,    // IN_PAUSE = 1024
  PAD_SELECT,   // IN_DEBUG = 2048
};

void in_init(void) {
  InitPAD(padbuf[0], 34, padbuf[1], 34);

  padbuf[0][0] = padbuf[0][1] = 0xff;
  padbuf[1][0] = padbuf[1][1] = 0xff;

  StartPAD();
  ChangeClearPAD(0);
}

static inline u32 in_remap(const u32 held) {
  // TODO: proper bindings
  register u32 in = 0;
  for (u32 i = 0; i < IN_NUM_ACTIONS; ++i) {
    if (held & binds[i])
      in |= 1 << i;
  }
  // TODO: ok/cancel switcheroo
  if (in & IN_FIRE)
    in |= IN_OK;
  else if (in & IN_JUMP)
    in |= IN_CANCEL;
  return in;
}

void in_update(void) {
  PADTYPE *pad = (PADTYPE *)&padbuf[0][0];
  input_old = input_held;
  input_held = in_remap(~pad->btn);
  input_trig = ~input_old & input_held;
}
