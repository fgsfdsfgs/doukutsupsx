#pragma once

#include "engine/common.h"

#include "game/player.h"
#include "game/game.h"
#include "game/npc.h"
#include "game/stage.h"

#define PROFILE_FILENAME "BIDOUKUTSU"

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
  char stage_title[MAX_STAGE_TITLE];
  u32 game_tick;
  u32 clock_tick;
  u16 stage_id;
  u16 stage_bank_id;
  u16 music_id;
  u16 tele_dest_num;
} profile_t;

#pragma pack(pop)

extern profile_t profile;
extern int profile_slot;

void profile_reset(void);
void profile_save(void);
bool profile_load(void);
