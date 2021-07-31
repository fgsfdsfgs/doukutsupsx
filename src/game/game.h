#pragma once

#include "engine/common.h"

// common game stuff

enum dir {
  DIR_LEFT = 0,
  DIR_UP = 1,
  DIR_RIGHT = 2,
  DIR_DOWN = 3,
  DIR_AUTO = 4,
  DIR_OTHER = 5
};

enum game_flags {
  GFLAG_INPUT_ENABLED = 1 << 1,
};

extern u32 game_tick;
extern u32 game_flags;

void game_init(void);
void game_update(void);
void game_draw(void);
