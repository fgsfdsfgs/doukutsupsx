#pragma once

#include "engine/common.h"

// psyq lacks these
enum pad_buttons {
  PAD_SELECT = 1,
  PAD_L3 = 2,
  PAD_R3 = 4,
  PAD_START = 8,
  PAD_UP = 16,
  PAD_RIGHT = 32,
  PAD_DOWN = 64,
  PAD_LEFT = 128,
  PAD_L2 = 256,
  PAD_R2 = 512,
  PAD_L1 = 1024,
  PAD_R1 = 2048,
  PAD_TRIANGLE = 4096,
  PAD_CIRCLE = 8192,
  PAD_CROSS = 16384,
  PAD_SQUARE = 32768,
};

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
  IN_OK = 1024,
  IN_CANCEL = 2048,
  IN_PAUSE = 4096,
  IN_DEBUG = 8192,

  IN_NUM_ACTIONS = 14
};

// action->pad bindings
extern u16 input_binds[IN_NUM_ACTIONS];
// default bindings
extern const u16 input_binds_default[IN_NUM_ACTIONS];

// IN_ flags of actions currently held
extern u32 input_held;
// IN_ flags of actions pressed this frame
extern u32 input_trig;
// raw pad buttons currently held
extern u16 input_pad;

// don't raise trigger events for next frame
extern bool input_suppress_trig;

void in_init(void);
void in_update(void);
