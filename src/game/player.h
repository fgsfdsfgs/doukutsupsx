#pragma once

#include "engine/common.h"
#include "engine/graphics.h"

// also known as MyChar.h

#define PLR_MAX_ARMS 16

enum equip_flags {
  EQUIP_BOOSTER_0_8      = 0x01,
  EQUIP_MAP              = 0x02,
  EQUIP_ARMS_BARRIER     = 0x04,
  EQUIP_TURBOCHARGE      = 0x08,
  EQUIP_AIR_TANK         = 0x10,
  EQUIP_BOOSTER_2_0      = 0x20,
  EQUIP_MIMIGA_MASK      = 0x40,
  EQUIP_WHIMSICAL_STAR   = 0x80,
  EQUIP_NIKUMARU_COUNTER = 0x100
};

enum plr_cond_flags {
  PLRCOND_UNKNOWN01 = 0x01,
  PLRCOND_INVISIBLE = 0x02,
  PLRCOND_UNKNOWN04 = 0x04,
  PLRCOND_ONGROUND  = 0x08,
  PLRCOND_UNKNOWN20 = 0x20,
  PLRCOND_ALIVE     = 0x80,
};

// fuckton of fields
// sorted them in order of size/alignment and renamed some
typedef struct {
  gfx_texrect_t rect;
  gfx_texrect_t rect_arms;

  hitbox_t hit;
  hitbox_t view;

  u32 flags;
  u32 equip;

  s32 x;
  s32 y;
  s32 xvel;
  s32 yvel;
  s32 tgt_x;
  s32 tgt_y;
  s32 index_x;
  s32 index_y;
  s32 anim;
  s32 anim_wait;
  s32 arm;
  s32 boost_cnt;
  bool up;
  bool down;

  s16 life;
  s16 max_life;

  u8 cond;
  s8 dir;
  u8 shock;
  s8 boost_sw;
  s8 splash;
  s8 question;
} player_t;

extern player_t player;

void plr_init(void);
void plr_draw(int cam_x, int cam_y);
void plr_set_pos(int x, int y);
void plr_animate(const u32 btns);
void plr_act(const u32 btns, const u32 trig);
