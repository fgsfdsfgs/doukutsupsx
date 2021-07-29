#pragma once

#include "engine/common.h"

// common game stuff

enum dir {
  DIR_LEFT = 0,
  DIR_CENTER = 1,
  DIR_RIGHT = 2,
};

extern u32 game_tick;
extern u32 game_flags;

void game_init(void);
void game_update(void);
void game_draw(void);
