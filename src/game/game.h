#pragma once

#include "engine/common.h"

// common game stuff

#define GAME_MAX_SKIPFLAGS 8
#define GAME_MAX_MAPFLAGS 0x10
#define GAME_MAX_TELEDEST 8

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

typedef struct {
  u16 stage_num;
  u16 event_num;
} tele_dest_t;

extern u32 game_tick;
extern u32 game_flags;

extern u8 skip_flags[GAME_MAX_SKIPFLAGS];
extern u8 map_flags[GAME_MAX_MAPFLAGS];

extern tele_dest_t tele_dest[GAME_MAX_TELEDEST];
extern int tele_dest_num;

void game_init(void);
void game_reset(void);
void game_start_intro(void);
void game_start_new(void);
void game_frame(void);

tele_dest_t *game_add_tele_dest(const u16 stage_id, const u16 event_num);
tele_dest_t *game_find_tele_dest(const u16 stage_id);

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
