#pragma once

#include "engine/common.h"
#include "engine/graphics.h"

#define BULLET_MAX 0x40

enum bullet_cond_flags {
  BULLETCOND_ALIVE = 0x80,
};

typedef struct {
  hitbox_t view;
  gfx_texrect_t texrect;

  u32 flags;
  u32 bits;

  s32 x;
  s32 y;
  s32 xvel;
  s32 yvel;
  s32 tgt_x;
  s32 tgt_y;
  s32 enemy_xl;
  s32 enemy_yl;
  s32 block_xl;
  s32 block_yl;

  u16 class_num;
  s16 act;
  s16 act_wait;
  s16 anim;
  s16 anim_wait;
  s16 life;
  s16 life_count;
  s16 count1;
  s16 count2;
  s16 damage;

  u8 cond;
  u8 dir;
} bullet_t;

typedef struct {
  s8 damage;
  s8 life;
  s16 life_count;
  s16 bits;
  s16 enemy_xl;
  s16 enemy_yl;
  s16 block_xl;
  s16 block_yl;
  hitbox_t view;
} bullet_class_t;

extern bullet_t bullet_list[BULLET_MAX];
extern int bullet_list_max;

void bullet_init(void);
bullet_t *bullet_spawn(int class_num, int x, int y, int dir);
void bullet_act(void);
void bullet_draw(int cam_x, int cam_y);
void bullet_destroy(bullet_t *bul);
int bullet_count_by_arm(const int arm_id);
int bullet_count_by_class(const int class_num);
bool bullet_any_exist(void);
