#pragma once

#include "engine/common.h"
#include "engine/graphics.h"

// also known as "value views"

#define DMGNUM_MAX 0x10

typedef struct {
  gfx_texrect_t *texrects[5]; // sign + digits
  bool cond;
  s32 *tgt_x;
  s32 *tgt_y;
  s32 xofs;
  s32 yofs;
  s32 vofs;
  s32 val;
  s32 count;
  s32 digits;
} dmgnum_t;

extern dmgnum_t dmgnum_list[DMGNUM_MAX];

void dmgnum_init(void);
void dmgnum_spawn(int *tgt_x, int *tgt_y, int val);
void dmgnum_act(void);
void dmgnum_draw(int cam_x, int cam_y);
