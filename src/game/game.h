#pragma once

#include "engine/common.h"

// common game stuff

#define GAME_MAX_SKIPFLAGS 8
#define GAME_MAX_MAPFLAGS 0x10

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

extern u8 skip_flags[GAME_MAX_SKIPFLAGS];
extern u8 map_flags[GAME_MAX_MAPFLAGS];
extern u8 tele_flags[GAME_MAX_MAPFLAGS];

void game_init(void);
void game_reset(void);
void game_start(void);
void game_frame(void);

static inline u32 game_get_skipflag(const u32 i) {
  return skip_flags[i >> 3] & (1 << (i & 7));
}

static inline void game_set_skipflag(const u32 i) {
  skip_flags[i >> 3] |= (1 << (i & 7));
}

static inline void game_clear_skipflag(const u32 i) {
  skip_flags[i >> 3] &= ~(1 << (i & 7));
}

static inline u32 game_get_mapflag(const u32 i) {
  return map_flags[i >> 3] & (1 << (i & 7));
}

static inline void game_set_mapflag(const u32 i) {
  map_flags[i >> 3] |= (1 << (i & 7));
}

static inline u32 game_get_teleflag(const u32 i) {
  return tele_flags[i >> 3] & (1 << (i & 7));
}

static inline void game_set_teleflag(const u32 i) {
  tele_flags[i >> 3] |= (1 << (i & 7));
}
