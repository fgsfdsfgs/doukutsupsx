#pragma once

#include "engine/common.h"

#include "game/player.h"
#include "game/game.h"
#include "game/npc.h"

#pragma pack(push, 1)

typedef struct {
  plr_arm_data_t arms[PLR_MAX_ARMS];
  u8 items[PLR_MAX_ITEMS];
  s32 x;
  s32 y;
  s16 dir;
  s16 life;
  s16 max_life;
  s16 star;
  u16 arm_id;
  u16 equip;
  u16 unit;
} profile_player_t;

typedef struct {
  char magic[4]; // 'CSPF'
  tele_dest_t tele_dest[GAME_MAX_TELEDEST];
  u8 npc_flags[NPC_MAX_FLAGS];
  u8 map_flags[GAME_MAX_MAPFLAGS];
  u8 skip_flags[GAME_MAX_SKIPFLAGS];
  profile_player_t player;
  u32 game_tick;
  u16 stage_id;
  u16 stage_bank_id;
  u16 music_id;
  u16 tele_dest_num;
} profile_t;

#pragma pack(pop)

extern profile_t profile;

void profile_reset(void);
bool profile_write(void);
bool profile_read(void);
