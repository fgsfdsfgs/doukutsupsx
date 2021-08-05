#pragma once

#include "engine/common.h"

typedef struct {
  s32 x;
  s32 y;
  s32 *tgt_x;
  s32 *tgt_y;
  s32 wait;
  s32 quake;
  s32 quake2;
} cam_t;

extern cam_t camera;

void cam_init(void);
void cam_update(void);
void cam_center_on_player(void);
void cam_target_player(const int delay);
void cam_target_npc(const int event_num, const int delay);
void cam_set_pos(const int fx, const int fy);
void cam_set_target(s32 *tx, s32 *ty, const int wait);

bool cam_is_fading(void);
void cam_draw_fade(void);
void cam_update_fade(void);
void cam_start_fade_out(const int dir);
void cam_start_fade_in(const int dir);
void cam_clear_fade(void);
void cam_complete_fade(void);
