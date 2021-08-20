#pragma once

#include "engine/common.h"
#include "engine/input.h"
#include "engine/mcrd.h"

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
  u16 reserved;
} profile_player_t;

typedef struct {
  u16 binds[IN_NUM_ACTIONS];
  s16 vol_sfx;
  s16 vol_music;
  u32 reserved[4];
} profile_config_t;

typedef struct {
  profile_player_t player;
  char stage_title[MAX_STAGE_TITLE];
  tele_dest_t tele_dest[GAME_MAX_TELEDEST];
  u8 npc_flags[NPC_MAX_FLAGS];
  u8 map_flags[GAME_MAX_MAPFLAGS];
  u8 skip_flags[GAME_MAX_SKIPFLAGS];
  u32 game_tick;
  u32 clock_tick;
  u16 stage_id;
  u16 stage_bank_id;
  u16 music_id;
  u16 tele_dest_num;
  u32 reserved[4];
} profile_save_t;

#define PROFILE_DATA_SIZE (sizeof(profile_save_t) + sizeof(profile_config_t) + 4)

typedef struct {
  char magic[4]; // 'CSPF'
  profile_save_t save;
  profile_config_t config;
  // padding to align with memcard sectors
  u8 pad[ALIGN(PROFILE_DATA_SIZE, MCRD_SECSIZE) - PROFILE_DATA_SIZE];
} profile_t;

#pragma pack(pop)

extern profile_t profile;
extern int profile_slot;

void profile_reset(void);
void profile_save(void);
bool profile_load(void);
