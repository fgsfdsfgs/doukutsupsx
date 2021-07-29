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

void cam_update(void);
void cam_center_on_player(void);
void cam_set_pos(const int fx, const int fy);
void cam_set_target(s32 *tx, s32 *ty, const int wait);
