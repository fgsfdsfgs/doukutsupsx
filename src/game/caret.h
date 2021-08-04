#pragma once

// also known as particles

#define CARET_MAX 0x40

enum caret_classnames {
  CARET_NULL = 0,
  CARET_BUBBLE = 1,
  CARET_PROJECTILE_DISSIPATION = 2,
  CARET_SHOOT = 3,
  CARET_SNAKE_AFTERIMAGE = 4,
  CARET_ZZZ = 5,
  CARET_SNAKE_AFTERIMAGE_DUPLICATE = 6,
  CARET_EXHAUST = 7,
  CARET_DROWNED_QUOTE = 8,
  CARET_QUESTION_MARK = 9,
  CARET_LEVEL_UP = 10,
  CARET_HURT_PARTICLES = 11,
  CARET_EXPLOSION = 12,
  CARET_TINY_PARTICLES = 13,
  CARET_UNKNOWN = 14,
  CARET_PROJECTILE_DISSIPATION_TINY = 15,
  CARET_EMPTY = 16,
  CARET_PUSH_JUMP_KEY = 17
};

typedef struct {
  gfx_texrect_t texrect;
  s32 x;
  s32 y;
  s32 xvel;
  s32 yvel;
  s32 view_left;
  s32 view_top;
  s32 dir;
  u16 class_num;
  u16 cond;
  s16 act;
  s16 act_wait;
  s16 anim;
  s16 anim_wait;
} caret_t;

extern caret_t caret_list[CARET_MAX];
extern int caret_list_max;

void caret_init(void);
void caret_act(void);
void caret_draw(int cam_x, int cam_y);
void caret_spawn(int x, int y, int class_num, int dir);
