#pragma once

#include "engine/common.h"
#include "engine/sound.h"

#define ORG_START_CH 8
#define ORG_MAX_TRACKS 16
#define ORG_INVALID 0xFFFF
#define ORG_PREVIOUS 0xFFFE

typedef struct org_note {
  s32 pos;
  u8 len;
  u8 key;
  u8 vol;
  u8 pan;
} org_note_t;

void org_init(void);
bool org_load(const u32 id, u8 *data, sfx_bank_t *bank);
void org_free(void);
void org_restart_from(const s32 pos);
void org_tick(void);
void org_pause(const bool paused);
void org_start_fade(void);

u32 org_get_id(void);
int org_get_wait(void);
int org_get_pos(void);
u16 org_get_mute_mask(void);
u16 org_set_mute_mask(const u16 mask);

org_note_t *org_get_track(const int tracknum, u32 *numnotes);
org_note_t *org_get_track_pos(const int tracknum);
