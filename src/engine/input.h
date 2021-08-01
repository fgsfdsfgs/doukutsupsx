#pragma once

#include "engine/common.h"

enum input_flags {
  IN_LEFT = 1,
  IN_RIGHT = 2,
  IN_UP = 4,
  IN_DOWN = 8,
  IN_JUMP = 16,
  IN_FIRE = 32,
  IN_SWAP_L = 64,
  IN_SWAP_R = 128,
  IN_INVENTORY = 256,
  IN_MAP = 512,
  IN_PAUSE = 1024,

  // to allow switching fire/jump cancel
  IN_OK = 2048,
  IN_CANCEL = 4096,

  // real actions, not counting OK/CANCEL
  IN_NUM_ACTIONS = 11
};

// IN_ flags of actions currently held
extern u32 input_held;
// IN_ flags of actions pressed this frame
extern u32 input_trig;

void in_init(void);
void in_update(void);
