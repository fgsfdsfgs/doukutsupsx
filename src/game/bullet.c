#include <string.h>

#include "engine/common.h"
#include "engine/math.h"
#include "engine/graphics.h"
#include "engine/sound.h"
#include "engine/input.h"

#include "game/game.h"
#include "game/player.h"
#include "game/npc.h"
#include "game/caret.h"
#include "game/bullet.h"

bullet_t bullet_list[BULLET_MAX];
int bullet_list_max = 0;

static bullet_class_t bullet_classtab[] = {
  // Null
  {0, 0, 0, 0, 0, 0, 0, 0, {0, 0, 0, 0}},
  // Snake
  {4, 1, 20, 36, 4, 4, 2, 2, {8, 8, 8, 8}},
  {6, 1, 23, 36, 4, 4, 2, 2, {8, 8, 8, 8}},
  {8, 1, 30, 36, 4, 4, 2, 2, {8, 8, 8, 8}},
  // Polar Star
  {1, 1, 8, 32, 6, 6, 2, 2, {8, 8, 8, 8}},
  {2, 1, 12, 32, 6, 6, 2, 2, {8, 8, 8, 8}},
  {4, 1, 16, 32, 6, 6, 2, 2, {8, 8, 8, 8}},
  // Fireball
  {2, 2, 100, 8, 8, 16, 4, 2, {8, 8, 8, 8}},
  {3, 2, 100, 8, 4, 4, 4, 2, {8, 8, 8, 8}},
  {3, 2, 100, 8, 4, 4, 4, 2, {8, 8, 8, 8}},
  // Machine Gun
  {2, 1, 20, 32, 2, 2, 2, 2, {8, 8, 8, 8}},
  {4, 1, 20, 32, 2, 2, 2, 2, {8, 8, 8, 8}},
  {6, 1, 20, 32, 2, 2, 2, 2, {8, 8, 8, 8}},
  // Missile Launcher
  {0, 10, 50, 40, 2, 2, 2, 2, {8, 8, 8, 8}},
  {0, 10, 70, 40, 4, 4, 4, 4, {8, 8, 8, 8}},
  {0, 10, 90, 40, 4, 4, 0, 0, {8, 8, 8, 8}},
  // Missile Launcher explosion
  {1, 100, 100, 20, 16, 16, 0, 0, {0, 0, 0, 0}},
  {1, 100, 100, 20, 16, 16, 0, 0, {0, 0, 0, 0}},
  {1, 100, 100, 20, 16, 16, 0, 0, {0, 0, 0, 0}},
  // Bubbler
  {1, 1, 20, 8, 2, 2, 2, 2, {4, 4, 4, 4}},
  {2, 1, 20, 8, 2, 2, 2, 2, {4, 4, 4, 4}},
  {2, 1, 20, 8, 4, 4, 4, 4, {4, 4, 4, 4}},
  // Bubbler level 3 thorns
  {3, 1, 32, 32, 2, 2, 2, 2, {4, 4, 4, 4}},
  // Blade slashes
  {0, 100, 0, 36, 8, 8, 8, 8, {12, 12, 12, 12}},
  // Falling spike that deals 127 damage
  {127, 1, 2, 4, 8, 4, 8, 4, {0, 0, 0, 0}},
  // Blade
  {15, 1, 30, 36, 8, 8, 4, 2, {8, 8, 8, 8}},
  {6, 3, 18, 36, 10, 10, 4, 2, {12, 12, 12, 12}},
  {1, 100, 30, 36, 6, 6, 4, 4, {12, 12, 12, 12}},
  // Super Missile Launcher
  {0, 10, 30, 40, 2, 2, 2, 2, {8, 8, 8, 8}},
  {0, 10, 40, 40, 4, 4, 4, 4, {8, 8, 8, 8}},
  {0, 10, 40, 40, 4, 4, 0, 0, {8, 8, 8, 8}},
  // Super Missile Launcher explosion
  {2, 100, 100, 20, 12, 12, 0, 0, {0, 0, 0, 0}},
  {2, 100, 100, 20, 12, 12, 0, 0, {0, 0, 0, 0}},
  {2, 100, 100, 20, 12, 12, 0, 0, {0, 0, 0, 0}},
  // Nemesis
  {4, 4, 20, 32, 4, 4, 3, 3, {8, 8, 24, 8}},
  {4, 2, 20, 32, 2, 2, 2, 2, {8, 8, 24, 8}},
  {1, 1, 20, 32, 2, 2, 2, 2, {8, 8, 24, 8}},
  // Spur
  {4, 4, 30, 64, 6, 6, 3, 3, {8, 8, 8, 8}},
  {8, 8, 30, 64, 6, 6, 3, 3, {8, 8, 8, 8}},
  {12, 12, 30, 64, 6, 6, 3, 3, {8, 8, 8, 8}},
  // Spur trail
  {3, 100, 30, 32, 6, 6, 3, 3, {4, 4, 4, 4}},
  {6, 100, 30, 32, 6, 6, 3, 3, {4, 4, 4, 4}},
  {11, 100, 30, 32, 6, 6, 3, 3, {4, 4, 4, 4}},
  // Curly's Nemesis
  {4, 4, 20, 32, 4, 4, 3, 3, {8, 8, 24, 8}},
  // Screen-nuke that kills all enemies
  {0, 4, 4, 4, 0, 0, 0, 0, {0, 0, 0, 0}},
  // Whimsical Star
  {1, 1, 1, 36, 1, 1, 1, 1, {1, 1, 1, 1}},
};

void bullet_init(void) {
  for (int i = 0; i < BULLET_MAX; ++i)
    bullet_list[i].cond = 0;
  bullet_list_max = 0;
}

bullet_t *bullet_spawn(int class_num, int x, int y, int dir) {
  int i = 0;
  while (i < BULLET_MAX && bullet_list[i].cond & BULLETCOND_ALIVE)
    ++i;

  if (i == BULLET_MAX)
    return NULL;

  if (i > bullet_list_max)
    bullet_list_max = i;

  bullet_t *bullet = &bullet_list[i];
  bullet_class_t *bclass = &bullet_classtab[class_num];
  memset(bullet, 0, sizeof(*bullet));
  bullet->cond = BULLETCOND_ALIVE;
  bullet->class_num = class_num;
  bullet->life = bclass->life;
  bullet->life_count = bclass->life_count;
  bullet->bits = bclass->bits;
  bullet->damage = bclass->damage;
  bullet->enemy_xl = TO_FIX((int)bclass->enemy_xl);
  bullet->enemy_yl = TO_FIX((int)bclass->enemy_yl);
  bullet->block_xl = TO_FIX((int)bclass->block_xl);
  bullet->block_yl = TO_FIX((int)bclass->block_yl);
  bullet->view.back = TO_FIX((int)bclass->view.back);
  bullet->view.front = TO_FIX((int)bclass->view.front);
  bullet->view.top = TO_FIX((int)bclass->view.top);
  bullet->view.bottom = TO_FIX((int)bclass->view.bottom);
  bullet->x = x;
  bullet->y = y;
  bullet->dir = dir;

  return bullet;
}

static inline void bullet_draw_instance(bullet_t *b, const int cam_vx, const int cam_vy) {
  register int x, y;

  switch (b->dir) {
    case DIR_LEFT:
      x = b->x - b->view.front;
      y = b->y - b->view.top;
      break;
    case DIR_UP:
      x = b->x - b->view.top;
      y = b->y - b->view.front;
      break;
    case DIR_RIGHT:
      x = b->x - b->view.back;
      y = b->y - b->view.top;
      break;
    case DIR_DOWN:
      x = b->x - b->view.top;
      y = b->y - b->view.back;
      break;
  }

  x = TO_INT(x);
  y = TO_INT(y);

  // TODO: cache bullet texrects
  gfx_set_texrect(&b->texrect, SURFACE_ID_BULLET);
  gfx_draw_texrect(&b->texrect, GFX_LAYER_BACK, x - cam_vx, y - cam_vy);
}

void bullet_draw(int cam_x, int cam_y) {
  cam_x = TO_INT(cam_x);
  cam_y = TO_INT(cam_y);
  for (int i = 0; i <= bullet_list_max; ++i) {
    if (bullet_list[i].texrect.r.w && (bullet_list[i].cond & BULLETCOND_ALIVE))
      bullet_draw_instance(&bullet_list[i], cam_x, cam_y);
  }
}

static void bullet_act_null(bullet_t *bul) {
  // holy fuck it's fucking nothing
}

static void bullet_act_snake1(bullet_t *bul) {
  if (++bul->count1 > bul->life_count) {
    bul->cond = 0;
    // caret_spawn(bul->x, bul->y, CARET_SHOOT, DIR_LEFT);
    return;
  }

  if (bul->act == 0) {
    bul->anim = m_rand(0, 2);
    bul->act = 1;

    switch (bul->dir) {
      case DIR_LEFT:
        bul->xvel = -0x600;
        break;
      case DIR_UP:
        bul->yvel = -0x600;
        break;
      case DIR_RIGHT:
        bul->xvel = 0x600;
        break;
      case DIR_DOWN:
        bul->yvel = 0x600;
        break;
    }
  } else {
    bul->x += bul->xvel;
    bul->y += bul->yvel;
  }

  if (++bul->anim_wait > 0) {
    bul->anim_wait = 0;
    ++bul->anim;
  }

  if (bul->anim > 3) bul->anim = 0;

  static const rect_t rc_left[4] = {
    {136, 80, 152, 80},
    {120, 80, 136, 96},
    {136, 64, 152, 80},
    {120, 64, 136, 80},
  };

  static const rect_t rc_right[4] = {
    {120, 64, 136, 80},
    {136, 64, 152, 80},
    {120, 80, 136, 96},
    {136, 80, 152, 80},
  };

  if (bul->dir == DIR_LEFT)
    bul->texrect.r = rc_left[bul->anim];
  else
    bul->texrect.r = rc_right[bul->anim];
}

static void bullet_act_snake2(bullet_t *bul) {
  static unsigned int inc;

  const int level = bul->class_num;  // 1, 2 or 3

  if (++bul->count1 > bul->life_count) {
    bul->cond = 0;
    caret_spawn(bul->x, bul->y, CARET_SHOOT, DIR_LEFT);
    return;
  }

  if (bul->act == 0) {
    bul->anim = m_rand(0, 2);
    bul->act = 1;

    switch (bul->dir) {
      case DIR_LEFT:
        bul->xvel = -0x200;
        break;
      case DIR_UP:
        bul->yvel = -0x200;
        break;
      case DIR_RIGHT:
        bul->xvel = 0x200;
        break;
      case DIR_DOWN:
        bul->yvel = 0x200;
        break;
    }

    ++inc;

    switch (bul->dir) {
      case DIR_LEFT:
      case DIR_RIGHT:
        if (inc % 2)
          bul->yvel = 0x400;
        else
          bul->yvel = -0x400;

        break;

      case DIR_UP:
      case DIR_DOWN:
        if (inc % 2)
          bul->xvel = 0x400;
        else
          bul->xvel = -0x400;

        break;
    }
  } else {
    switch (bul->dir) {
      case DIR_LEFT:
        bul->xvel -= 0x80;
        break;
      case DIR_UP:
        bul->yvel -= 0x80;
        break;
      case DIR_RIGHT:
        bul->xvel += 0x80;
        break;
      case DIR_DOWN:
        bul->yvel += 0x80;
        break;
    }

    switch (bul->dir) {
      case DIR_LEFT:
      case DIR_RIGHT:
        if (bul->count1 % 5 == 2) {
          if (bul->yvel < 0)
            bul->yvel = 0x400;
          else
            bul->yvel = -0x400;
        }

        break;

      case DIR_UP:
      case DIR_DOWN:
        if (bul->count1 % 5 == 2) {
          if (bul->xvel < 0)
            bul->xvel = 0x400;
          else
            bul->xvel = -0x400;
        }

        break;
    }

    bul->x += bul->xvel;
    bul->y += bul->yvel;
  }

  if (++bul->anim_wait > 0) {
    bul->anim_wait = 0;
    ++bul->anim;
  }

  if (bul->anim > 2) bul->anim = 0;

  static const rect_t rect[3] = {
    {192, 16, 208, 32},
    {208, 16, 224, 32},
    {224, 16, 240, 32},
  };

  bul->texrect.r = rect[bul->anim];

  if (level == 2)
    npc_spawn(129, bul->x, bul->y, 0, -0x200, bul->anim, NULL, 0x100);
  else
    npc_spawn(129, bul->x, bul->y, 0, -0x200, bul->anim + 3, NULL, 0x100);
}

static void bullet_act_polarstar(bullet_t *bul) {
  const int level = bul->class_num - 4 + 1;

  if (++bul->count1 > bul->life_count) {
    bul->cond = 0;
    caret_spawn(bul->x, bul->y, CARET_SHOOT, DIR_LEFT);
    return;
  }

  if (bul->act == 0) {
    bul->act = 1;

    // Set speed
    switch (bul->dir) {
      case DIR_LEFT:
        bul->xvel = -0x1000;
        break;
      case DIR_UP:
        bul->yvel = -0x1000;
        break;
      case DIR_RIGHT:
        bul->xvel = 0x1000;
        break;
      case DIR_DOWN:
        bul->yvel = 0x1000;
        break;
    }

    // Set hitbox
    switch (level) {
      case 1:
        switch (bul->dir) {
          case DIR_LEFT:
            bul->enemy_yl = 0x400;
            break;
          case DIR_UP:
            bul->enemy_xl = 0x400;
            break;
          case DIR_RIGHT:
            bul->enemy_yl = 0x400;
            break;
          case DIR_DOWN:
            bul->enemy_xl = 0x400;
            break;
        }
        break;
      case 2:
        switch (bul->dir) {
          case DIR_LEFT:
            bul->enemy_yl = 0x800;
            break;
          case DIR_UP:
            bul->enemy_xl = 0x800;
            break;
          case DIR_RIGHT:
            bul->enemy_yl = 0x800;
            break;
          case DIR_DOWN:
            bul->enemy_xl = 0x800;
            break;
        }
        break;
    }
  } else {
    // Move
    bul->x += bul->xvel;
    bul->y += bul->yvel;
  }

  static const rect_t rect1[2] = {
    {128, 32, 144, 48},
    {144, 32, 160, 48},
  };

  static const rect_t rect2[2] = {
    {160, 32, 176, 48},
    {176, 32, 192, 48},
  };

  static const rect_t rect3[2] = {
    {128, 48, 144, 64},
    {144, 48, 160, 64},
  };

  // Set framerect
  switch (level) {
    case 1:
      if (bul->dir == DIR_UP || bul->dir == DIR_DOWN)
        bul->texrect.r = rect1[1];
      else
        bul->texrect.r = rect1[0];

      break;

    case 2:
      if (bul->dir == DIR_UP || bul->dir == DIR_DOWN)
        bul->texrect.r = rect2[1];
      else
        bul->texrect.r = rect2[0];

      break;

    case 3:
      if (bul->dir == DIR_UP || bul->dir == DIR_DOWN)
        bul->texrect.r = rect3[1];
      else
        bul->texrect.r = rect3[0];

      break;
  }
}

static void bullet_act_fireball(bullet_t *bul) {
  const int level = bul->class_num - 7 + 1;

  bool do_break;

  if (++bul->count1 > bul->life_count) {
    bul->cond = 0;
    caret_spawn(bul->x, bul->y, CARET_SHOOT, DIR_LEFT);
    return;
  }

  do_break = FALSE;
  if (bul->flags & 2 && bul->flags & 8) do_break = TRUE;
  if (bul->flags & 1 && bul->flags & 4) do_break = TRUE;

  if (bul->dir == DIR_LEFT && bul->flags & 1) bul->dir = DIR_RIGHT;
  if (bul->dir == DIR_RIGHT && bul->flags & 4) bul->dir = DIR_LEFT;

  if (do_break) {
    bul->cond = 0;
    caret_spawn(bul->x, bul->y, CARET_PROJECTILE_DISSIPATION, DIR_LEFT);
    snd_play_sound(-1, 28, SOUND_MODE_PLAY);
    return;
  }

  if (bul->act == 0) {
    bul->act = 1;

    switch (bul->dir) {
      case DIR_LEFT:
        bul->xvel = -0x400;
        break;

      case DIR_UP:
        bul->xvel = player.xvel;

        if (player.xvel < 0)
          bul->dir = DIR_LEFT;
        else
          bul->dir = DIR_RIGHT;

        if (player.dir == DIR_LEFT)
          bul->xvel -= 0x80;
        else
          bul->xvel += 0x80;

        bul->yvel = -0x5FF;
        break;

      case DIR_RIGHT:
        bul->xvel = 0x400;
        break;

      case DIR_DOWN:
        bul->xvel = player.xvel;

        if (player.xvel < 0)
          bul->dir = DIR_LEFT;
        else
          bul->dir = DIR_RIGHT;

        bul->yvel = 0x5FF;

        break;
    }
  } else {
    if (bul->flags & 8)
      bul->yvel = -0x400;
    else if (bul->flags & 1)
      bul->xvel = 0x400;
    else if (bul->flags & 4)
      bul->xvel = -0x400;

    bul->yvel += 85;
    if (bul->yvel > 0x3FF) bul->yvel = 0x3FF;

    bul->x += bul->xvel;
    bul->y += bul->yvel;

    if (bul->flags & 0xD) snd_play_sound(-1, 34, SOUND_MODE_PLAY);
  }

  static const rect_t rect_left1[4] = {
    {128, 0, 144, 16},
    {144, 0, 160, 16},
    {160, 0, 176, 16},
    {176, 0, 192, 16},
  };

  static const rect_t rect_right1[4] = {
    {128, 16, 144, 32},
    {144, 16, 160, 32},
    {160, 16, 176, 32},
    {176, 16, 192, 32},
  };

  static const rect_t rect_left2[3] = {
    {192, 16, 208, 32},
    {208, 16, 224, 32},
    {224, 16, 240, 32},
  };

  static const rect_t rect_right2[3] = {
    {224, 16, 240, 32},
    {208, 16, 224, 32},
    {192, 16, 208, 32},
  };

  ++bul->anim;

  if (level == 1) {
    if (bul->anim > 3) bul->anim = 0;

    if (bul->dir == DIR_LEFT)
      bul->texrect.r = rect_left1[bul->anim];
    else
      bul->texrect.r = rect_right1[bul->anim];
  } else {
    if (bul->anim > 2) bul->anim = 0;

    if (bul->dir == DIR_LEFT)
      bul->texrect.r = rect_left2[bul->anim];
    else
      bul->texrect.r = rect_right2[bul->anim];

    if (level == 2)
      npc_spawn(129, bul->x, bul->y, 0, -0x200, bul->anim, NULL, 0x100);
    else
      npc_spawn(129, bul->x, bul->y, 0, -0x200, bul->anim + 3, NULL, 0x100);
  }
}

static void bullet_act_machinegun(bullet_t *bul) {
  const int level = bul->class_num - 10 + 1;
  int move;

  static const rect_t rect1[4] = {
    {64, 0, 80, 16},
    {80, 0, 96, 16},
    {96, 0, 112, 16},
    {112, 0, 128, 16},
  };

  static const rect_t rect2[4] = {
    {64, 16, 80, 32},
    {80, 16, 96, 32},
    {96, 16, 112, 32},
    {112, 16, 128, 32},
  };

  static const rect_t rect3[4] = {
    {64, 32, 80, 48},
    {80, 32, 96, 48},
    {96, 32, 112, 48},
    {112, 32, 128, 48},
  };

  if (++bul->count1 > bul->life_count) {
    bul->cond = 0;
    caret_spawn(bul->x, bul->y, CARET_SHOOT, DIR_LEFT);
    return;
  }

  if (bul->act == 0) {
    switch (level) {
      case 1:
        move = 0x1000;
        break;
      case 2:
        move = 0x1000;
        break;
      case 3:
        move = 0x1000;
        break;
    }

    bul->act = 1;

    switch (bul->dir) {
      case DIR_LEFT:
        bul->xvel = -move;
        bul->yvel = m_rand(-0xAA, 0xAA);
        break;
      case DIR_UP:
        bul->yvel = -move;
        bul->xvel = m_rand(-0xAA, 0xAA);
        break;
      case DIR_RIGHT:
        bul->xvel = move;
        bul->yvel = m_rand(-0xAA, 0xAA);
        break;
      case DIR_DOWN:
        bul->yvel = move;
        bul->xvel = m_rand(-0xAA, 0xAA);
        break;
    }
  } else {
    bul->x += bul->xvel;
    bul->y += bul->yvel;

    switch (level) {
      case 1:
        bul->texrect.r = rect1[bul->dir];
        break;

      case 2:
        bul->texrect.r = rect2[bul->dir];

        if (bul->dir == DIR_UP || bul->dir == DIR_DOWN)
          npc_spawn(127, bul->x, bul->y, 0, 0, DIR_UP, NULL, 0x100);
        else
          npc_spawn(127, bul->x, bul->y, 0, 0, DIR_LEFT, NULL, 0x100);

        break;

      case 3:
        bul->texrect.r = rect3[bul->dir];
        npc_spawn(128, bul->x, bul->y, 0, 0, bul->dir, NULL, 0x100);
        break;
    }
  }
}

static void bullet_act_missile(bullet_t *bul) {
  static unsigned int inc;
  const int level = bul->class_num - 13 + 1;
  bool hit;

  if (++bul->count1 > bul->life_count) {
    bul->cond = 0;
    caret_spawn(bul->x, bul->y, CARET_SHOOT, DIR_LEFT);
    return;
  }

  hit = FALSE;

  if (bul->life != 10) hit = TRUE;
  if (bul->dir == DIR_LEFT && bul->flags & 1) hit = TRUE;
  if (bul->dir == DIR_RIGHT && bul->flags & 4) hit = TRUE;
  if (bul->dir == DIR_UP && bul->flags & 2) hit = TRUE;
  if (bul->dir == DIR_DOWN && bul->flags & 8) hit = TRUE;
  if (bul->dir == DIR_LEFT && bul->flags & 0x80) hit = TRUE;
  if (bul->dir == DIR_LEFT && bul->flags & 0x20) hit = TRUE;
  if (bul->dir == DIR_RIGHT && bul->flags & 0x40) hit = TRUE;
  if (bul->dir == DIR_RIGHT && bul->flags & 0x10) hit = TRUE;

  if (hit) {
    bullet_spawn(level + 15, bul->x, bul->y, DIR_LEFT);
    bul->cond = 0;
  }

  switch (bul->act) {
    case 0:
      bul->act = 1;

      switch (bul->dir) {
        case DIR_LEFT:
        case DIR_RIGHT:
          bul->tgt_y = bul->y;
          break;
        case DIR_UP:
        case DIR_DOWN:
          bul->tgt_x = bul->x;
          break;
      }

      if (level == 3) {
        switch (bul->dir) {
          case DIR_LEFT:
          case DIR_RIGHT:
            if (bul->y > player.y)
              bul->yvel = 0x100;
            else
              bul->yvel = -0x100;

            bul->xvel = m_rand(-0x200, 0x200);
            break;

          case DIR_UP:
          case DIR_DOWN:
            if (bul->x > player.x)
              bul->xvel = 0x100;
            else
              bul->xvel = -0x100;

            bul->yvel = m_rand(-0x200, 0x200);
            break;
        }

        switch (++inc % 3) {
          case 0:
            bul->anim = 0x80;
            break;
          case 1:
            bul->anim = 0x40;
            break;
          case 2:
            bul->anim = 0x33;
            break;
        }
      } else {
        bul->anim = 0x80;
      }
      // Fallthrough
    case 1:
      switch (bul->dir) {
        case DIR_LEFT:
          bul->xvel += -bul->anim;
          break;
        case DIR_UP:
          bul->yvel += -bul->anim;
          break;
        case DIR_RIGHT:
          bul->xvel += bul->anim;
          break;
        case DIR_DOWN:
          bul->yvel += bul->anim;
          break;
      }

      if (level == 3) {
        switch (bul->dir) {
          case DIR_LEFT:
          case DIR_RIGHT:
            if (bul->y < bul->tgt_y)
              bul->yvel += 0x20;
            else
              bul->yvel -= 0x20;

            break;

          case DIR_UP:
          case DIR_DOWN:
            if (bul->x < bul->tgt_x)
              bul->xvel += 0x20;
            else
              bul->xvel -= 0x20;
            break;
        }
      }

      if (bul->xvel < -0xA00) bul->xvel = -0xA00;
      if (bul->xvel > 0xA00) bul->xvel = 0xA00;

      if (bul->yvel < -0xA00) bul->yvel = -0xA00;
      if (bul->yvel > 0xA00) bul->yvel = 0xA00;

      bul->x += bul->xvel;
      bul->y += bul->yvel;

      break;
  }

  if (++bul->count2 > 2) {
    bul->count2 = 0;

    switch (bul->dir) {
      case DIR_LEFT:
        caret_spawn(bul->x + (8 * 0x200), bul->y, CARET_EXHAUST, DIR_RIGHT);
        break;
      case DIR_UP:
        caret_spawn(bul->x, bul->y + (8 * 0x200), CARET_EXHAUST, DIR_DOWN);
        break;
      case DIR_RIGHT:
        caret_spawn(bul->x - (8 * 0x200), bul->y, CARET_EXHAUST, DIR_LEFT);
        break;
      case DIR_DOWN:
        caret_spawn(bul->x, bul->y - (8 * 0x200), CARET_EXHAUST, DIR_UP);
        break;
    }
  }

  static const rect_t rect1[4] = {
    {0, 0, 16, 16},
    {16, 0, 32, 16},
    {32, 0, 48, 16},
    {48, 0, 64, 16},
  };

  static const rect_t rect2[4] = {
    {0, 16, 16, 32},
    {16, 16, 32, 32},
    {32, 16, 48, 32},
    {48, 16, 64, 32},
  };

  static const rect_t rect3[4] = {
    {0, 32, 16, 48},
    {16, 32, 32, 48},
    {32, 32, 48, 48},
    {48, 32, 64, 48},
  };

  switch (level) {
    case 1:
      bul->texrect.r = rect1[bul->dir];
      break;
    case 2:
      bul->texrect.r = rect2[bul->dir];
      break;
    case 3:
      bul->texrect.r = rect3[bul->dir];
      break;
  }
}

static void bullet_act_explosion(bullet_t *bul) {
  const int level = bul->class_num - 16 + 1;

  switch (bul->act) {
    case 0:
      bul->act = 1;

      switch (level) {
        case 1:
          bul->act_wait = 10;
          break;
        case 2:
          bul->act_wait = 15;
          break;
        case 3:
          bul->act_wait = 5;
          break;
      }

      snd_play_sound(-1, 44, SOUND_MODE_PLAY);
      // Fallthrough
    case 1:
      switch (level) {
        case 1:
          if (bul->act_wait % 3 == 0)
            npc_spawn_death_fx(bul->x + (m_rand(-16, 16) * 0x200), bul->y + (m_rand(-16, 16) * 0x200), bul->enemy_xl, 2, 1);
          break;

        case 2:
          if (bul->act_wait % 3 == 0)
            npc_spawn_death_fx(bul->x + (m_rand(-32, 32) * 0x200), bul->y + (m_rand(-32, 32) * 0x200), bul->enemy_xl, 2, 1);
          break;

        case 3:
          if (bul->act_wait % 3 == 0)
            npc_spawn_death_fx(bul->x + (m_rand(-40, 40) * 0x200), bul->y + (m_rand(-40, 40) * 0x200), bul->enemy_xl, 2, 1);
          break;
      }

      if (--bul->act_wait < 0) bul->cond = 0;

      break;
  }
}

static void bullet_act_supermissile(bullet_t *bul) {
  const int level = bul->class_num - 28 + 1;
  bool hit;

  static unsigned int inc;

  if (++bul->count1 > bul->life_count) {
    bul->cond = 0;
    caret_spawn(bul->x, bul->y, CARET_SHOOT, DIR_LEFT);
    return;
  }

  hit = FALSE;

  if (bul->life != 10) hit = TRUE;
  if (bul->dir == DIR_LEFT && bul->flags & 1) hit = TRUE;
  if (bul->dir == DIR_RIGHT && bul->flags & 4) hit = TRUE;
  if (bul->dir == DIR_UP && bul->flags & 2) hit = TRUE;
  if (bul->dir == DIR_DOWN && bul->flags & 8) hit = TRUE;
  if (bul->dir == DIR_LEFT && bul->flags & 0x80) hit = TRUE;
  if (bul->dir == DIR_LEFT && bul->flags & 0x20) hit = TRUE;
  if (bul->dir == DIR_RIGHT && bul->flags & 0x40) hit = TRUE;
  if (bul->dir == DIR_RIGHT && bul->flags & 0x10) hit = TRUE;

  if (hit) {
    bullet_spawn(level + 30, bul->x, bul->y, DIR_LEFT);
    bul->cond = 0;
  }

  switch (bul->act) {
    case 0:
      bul->act = 1;

      switch (bul->dir) {
        case DIR_LEFT:
        case DIR_RIGHT:
          bul->tgt_y = bul->y;
          bul->enemy_xl = 0x1000;
          bul->block_xl = 0x1000;
          break;

        case DIR_UP:
        case DIR_DOWN:
          bul->tgt_x = bul->x;
          bul->enemy_yl = 0x1000;
          bul->block_yl = 0x1000;
          break;
      }

      if (level == 3) {
        switch (bul->dir) {
          case DIR_LEFT:
          case DIR_RIGHT:
            if (bul->y > player.y)
              bul->yvel = 0x100;
            else
              bul->yvel = -0x100;

            bul->xvel = m_rand(-0x200, 0x200);
            break;

          case DIR_UP:
          case DIR_DOWN:
            if (bul->x > player.x)
              bul->xvel = 0x100;
            else
              bul->xvel = -0x100;

            bul->yvel = m_rand(-0x200, 0x200);
            break;
        }

        switch (++inc % 3) {
          case 0:
            bul->anim = 0x200;
            break;
          case 1:
            bul->anim = 0x100;
            break;
          case 2:
            bul->anim = 0xAA;
            break;
        }
      } else {
        bul->anim = 0x200;
      }
      // Fallthrough
    case 1:
      switch (bul->dir) {
        case DIR_LEFT:
          bul->xvel += -bul->anim;
          break;
        case DIR_UP:
          bul->yvel += -bul->anim;
          break;
        case DIR_RIGHT:
          bul->xvel += bul->anim;
          break;
        case DIR_DOWN:
          bul->yvel += bul->anim;
          break;
      }

      if (level == 3) {
        switch (bul->dir) {
          case DIR_LEFT:
          case DIR_RIGHT:
            if (bul->y < bul->tgt_y)
              bul->yvel += 0x40;
            else
              bul->yvel -= 0x40;

            break;
          case DIR_UP:
          case DIR_DOWN:
            if (bul->x < bul->tgt_x)
              bul->xvel += 0x40;
            else
              bul->xvel -= 0x40;

            break;
        }
      }

      if (bul->xvel < -0x1400) bul->xvel = -0x1400;
      if (bul->xvel > 0x1400) bul->xvel = 0x1400;

      if (bul->yvel < -0x1400) bul->yvel = -0x1400;
      if (bul->yvel > 0x1400) bul->yvel = 0x1400;

      bul->x += bul->xvel;
      bul->y += bul->yvel;

      break;
  }

  if (++bul->count2 > 2) {
    bul->count2 = 0;

    switch (bul->dir) {
      case DIR_LEFT:
        caret_spawn(bul->x + (8 * 0x200), bul->y, CARET_EXHAUST, DIR_RIGHT);
        break;
      case DIR_UP:
        caret_spawn(bul->x, bul->y + (8 * 0x200), CARET_EXHAUST, DIR_DOWN);
        break;
      case DIR_RIGHT:
        caret_spawn(bul->x - (8 * 0x200), bul->y, CARET_EXHAUST, DIR_LEFT);
        break;
      case DIR_DOWN:
        caret_spawn(bul->x, bul->y - (8 * 0x200), CARET_EXHAUST, DIR_UP);
        break;
    }
  }

  static const rect_t rect1[4] = {
    {120, 96, 136, 112},
    {136, 96, 152, 112},
    {152, 96, 168, 112},
    {168, 96, 184, 112},
  };

  static const rect_t rect2[4] = {
    {184, 96, 200, 112},
    {200, 96, 216, 112},
    {216, 96, 232, 112},
    {232, 96, 248, 112},
  };

  switch (level) {
    case 1:
      bul->texrect.r = rect1[bul->dir];
      break;
    case 2:
      bul->texrect.r = rect2[bul->dir];
      break;
    case 3:
      bul->texrect.r = rect1[bul->dir];
      break;
  }
}

static void bullet_act_superexplosion(bullet_t *bul) {
  const int level = bul->class_num - 31 + 1;

  switch (bul->act) {
    case 0:
      bul->act = 1;

      switch (level) {
        case 1:
          bul->act_wait = 10;
          break;
        case 2:
          bul->act_wait = 14;
          break;
        case 3:
          bul->act_wait = 6;
          break;
      }

      snd_play_sound(-1, 44, SOUND_MODE_PLAY);
      // Fallthrough
    case 1:
      switch (level) {
        case 1:
          if (bul->act_wait % 3 == 0)
            npc_spawn_death_fx(bul->x + (m_rand(-16, 16) * 0x200), bul->y + (m_rand(-16, 16) * 0x200), bul->enemy_xl, 2, 1);
          break;
        case 2:
          if (bul->act_wait % 3 == 0)
            npc_spawn_death_fx(bul->x + (m_rand(-32, 32) * 0x200), bul->y + (m_rand(-32, 32) * 0x200), bul->enemy_xl, 2, 1);
          break;
        case 3:
          if (bul->act_wait % 3 == 0)
            npc_spawn_death_fx(bul->x + (m_rand(-40, 40) * 0x200), bul->y + (m_rand(-40, 40) * 0x200), bul->enemy_xl, 2, 1);
          break;
      }

      if (--bul->act_wait < 0) bul->cond = 0;

      break;
  }
}

static void bullet_act_bubble1(bullet_t *bul) {
  if (bul->flags & 0x2FF) {
    bul->cond = 0;
    caret_spawn(bul->x, bul->y, CARET_PROJECTILE_DISSIPATION, DIR_LEFT);
    return;
  }

  switch (bul->act) {
    case 0:
      bul->act = 1;

      switch (bul->dir) {
        case DIR_LEFT:
          bul->xvel = -0x600;
          break;
        case DIR_RIGHT:
          bul->xvel = 0x600;
          break;
        case DIR_UP:
          bul->yvel = -0x600;
          break;
        case DIR_DOWN:
          bul->yvel = 0x600;
          break;
      }

      break;
  }

  switch (bul->dir) {
    case DIR_LEFT:
      bul->xvel += 0x2A;
      break;
    case DIR_RIGHT:
      bul->xvel -= 0x2A;
      break;
    case DIR_UP:
      bul->yvel += 0x2A;
      break;
    case DIR_DOWN:
      bul->yvel -= 0x2A;
      break;
  }

  bul->x += bul->xvel;
  bul->y += bul->yvel;

  if (++bul->act_wait > 40) {
    bul->cond = 0;
    caret_spawn(bul->x, bul->y, CARET_PROJECTILE_DISSIPATION_TINY, DIR_LEFT);
  }

  static const rect_t rect[4] = {
    {192, 0, 200, 8},
    {200, 0, 208, 8},
    {208, 0, 216, 8},
    {216, 0, 224, 8},
  };

  if (++bul->anim_wait > 3) {
    bul->anim_wait = 0;
    ++bul->anim;
  }

  if (bul->anim > 3) bul->anim = 3;

  bul->texrect.r = rect[bul->anim];
}

static void bullet_act_bubble2(bullet_t *bul) {
  bool delete = FALSE;

  if (bul->dir == DIR_LEFT && bul->flags & 1) delete = TRUE;
  if (bul->dir == DIR_RIGHT && bul->flags & 4) delete = TRUE;
  if (bul->dir == DIR_UP && bul->flags & 2) delete = TRUE;
  if (bul->dir == DIR_DOWN && bul->flags & 8) delete = TRUE;

  if (delete) {
    bul->cond = 0;
    caret_spawn(bul->x, bul->y, CARET_PROJECTILE_DISSIPATION, DIR_LEFT);
    return;
  }

  switch (bul->act) {
    case 0:
      bul->act = 1;

      switch (bul->dir) {
        case DIR_LEFT:
          bul->xvel = -0x600;
          bul->yvel = m_rand(-0x100, 0x100);
          break;
        case DIR_RIGHT:
          bul->xvel = 0x600;
          bul->yvel = m_rand(-0x100, 0x100);
          break;
        case DIR_UP:
          bul->yvel = -0x600;
          bul->xvel = m_rand(-0x100, 0x100);
          break;
        case DIR_DOWN:
          bul->yvel = 0x600;
          bul->xvel = m_rand(-0x100, 0x100);
          break;
      }

      break;
  }

  switch (bul->dir) {
    case DIR_LEFT:
      bul->xvel += 0x10;
      break;
    case DIR_RIGHT:
      bul->xvel -= 0x10;
      break;
    case DIR_UP:
      bul->yvel += 0x10;
      break;
    case DIR_DOWN:
      bul->yvel -= 0x10;
      break;
  }

  bul->x += bul->xvel;
  bul->y += bul->yvel;

  if (++bul->act_wait > 60) {
    bul->cond = 0;
    caret_spawn(bul->x, bul->y, CARET_PROJECTILE_DISSIPATION_TINY, DIR_LEFT);
  }

  static const rect_t rect[4] = {
    {192, 8, 200, 16},
    {200, 8, 208, 16},
    {208, 8, 216, 16},
    {216, 8, 224, 16},
  };

  if (++bul->anim_wait > 3) {
    bul->anim_wait = 0;
    ++bul->anim;
  }

  if (bul->anim > 3) bul->anim = 3;

  bul->texrect.r = rect[bul->anim];
}

static void bullet_act_bubble3(bullet_t *bul) {
  // the only fucking input check in the entire bullet code
  if (++bul->act_wait > 100 || !(input_held & IN_FIRE)) {
    bul->cond = 0;
    caret_spawn(bul->x, bul->y, CARET_PROJECTILE_DISSIPATION, DIR_LEFT);
    snd_play_sound(-1, 100, SOUND_MODE_PLAY);

    if (player.up)
      bullet_spawn(22, bul->x, bul->y, DIR_UP);
    else if (player.down)
      bullet_spawn(22, bul->x, bul->y, DIR_DOWN);
    else
      bullet_spawn(22, bul->x, bul->y, player.dir);

    return;
  }

  switch (bul->act) {
    case 0:
      bul->act = 1;

      switch (bul->dir) {
        case DIR_LEFT:
          bul->xvel = m_rand(-0x400, -0x200);
          bul->yvel = (m_rand(-4, 4) * 0x200) / 2;
          break;
        case DIR_RIGHT:
          bul->xvel = m_rand(0x200, 0x400);
          bul->yvel = (m_rand(-4, 4) * 0x200) / 2;
          break;
        case DIR_UP:
          bul->yvel = m_rand(-0x400, -0x200);
          bul->xvel = (m_rand(-4, 4) * 0x200) / 2;
          break;
        case DIR_DOWN:
          bul->yvel = m_rand(0x80, 0x100);
          bul->xvel = (m_rand(-4, 4) * 0x200) / 2;
          break;
      }

      break;
  }

  if (bul->x < player.x) bul->xvel += 0x20;
  if (bul->x > player.x) bul->xvel -= 0x20;

  if (bul->y < player.y) bul->yvel += 0x20;
  if (bul->y > player.y) bul->yvel -= 0x20;

  if (bul->xvel < 0 && bul->flags & 1) bul->xvel = 0x400;
  if (bul->xvel > 0 && bul->flags & 4) bul->xvel = -0x400;

  if (bul->yvel < 0 && bul->flags & 2) bul->yvel = 0x400;
  if (bul->yvel > 0 && bul->flags & 8) bul->yvel = -0x400;

  bul->x += bul->xvel;
  bul->y += bul->yvel;

  static const rect_t rect[4] = {
    {240, 16, 248, 24},
    {248, 16, 256, 24},
    {240, 24, 248, 32},
    {248, 24, 256, 32},
  };

  if (++bul->anim_wait > 3) {
    bul->anim_wait = 0;
    ++bul->anim;
  }

  if (bul->anim > 3) bul->anim = 3;

  bul->texrect.r = rect[bul->anim];
}

static void bullet_act_bubblespine(bullet_t *bul) {
  if (++bul->count1 > bul->life_count || bul->flags & 8) {
    bul->cond = 0;
    caret_spawn(bul->x, bul->y, CARET_SHOOT, DIR_LEFT);
    return;
  }

  if (bul->act == 0) {
    bul->act = 1;

    switch (bul->dir) {
      case DIR_LEFT:
        bul->xvel = (-m_rand(10, 16) * 0x200) / 2;
        break;
      case DIR_UP:
        bul->yvel = (-m_rand(10, 16) * 0x200) / 2;
        break;
      case DIR_RIGHT:
        bul->xvel = (m_rand(10, 16) * 0x200) / 2;
        break;
      case DIR_DOWN:
        bul->yvel = (m_rand(10, 16) * 0x200) / 2;
        break;
    }
  } else {
    bul->x += bul->xvel;
    bul->y += bul->yvel;
  }

  if (++bul->anim_wait > 1) {
    bul->anim_wait = 0;
    ++bul->anim;
  }

  if (bul->anim > 1) bul->anim = 0;

  static const rect_t rc_left[2] = {
    {224, 0, 232, 8},
    {232, 0, 240, 8},
  };

  static const rect_t rc_right[2] = {
    {224, 0, 232, 8},
    {232, 0, 240, 8},
  };

  static const rect_t rc_down[2] = {
    {224, 8, 232, 16},
    {232, 8, 240, 16},
  };

  switch (bul->dir) {
    case DIR_LEFT:
      bul->texrect.r = rc_left[bul->anim];
      break;
    case DIR_UP:
      bul->texrect.r = rc_down[bul->anim];
      break;
    case DIR_RIGHT:
      bul->texrect.r = rc_right[bul->anim];
      break;
    case DIR_DOWN:
      bul->texrect.r = rc_down[bul->anim];
      break;
  }
}

static void bullet_act_bladeslash(bullet_t *bul) {
  switch (bul->act) {
    case 0:
      bul->act = 1;
      bul->y -= 12 * 0x200;

      if (bul->dir == DIR_LEFT)
        bul->x += 16 * 0x200;
      else
        bul->x -= 16 * 0x200;
      // Fallthrough
    case 1:
      if (++bul->anim_wait > 2) {
        bul->anim_wait = 0;
        ++bul->anim;
      }

      if (bul->dir == DIR_LEFT)
        bul->x -= 2 * 0x200;
      else
        bul->x += 2 * 0x200;

      bul->y += 2 * 0x200;

      if (bul->anim == 1)
        bul->damage = 2;
      else
        bul->damage = 1;

      if (bul->anim > 4) {
        bul->cond = 0;
        return;
      }

      break;
  }

  static const rect_t rc_left[5] = {
    {0, 64, 24, 88}, {24, 64, 48, 88}, {48, 64, 72, 88},
    {72, 64, 96, 88}, {96, 64, 120, 88},
  };

  static const rect_t rc_right[5] = {
    {0, 88, 24, 112}, {24, 88, 48, 112}, {48, 88, 72, 112},
    {72, 88, 96, 112}, {96, 88, 120, 112},
  };

  if (bul->dir == DIR_LEFT)
    bul->texrect.r = rc_left[bul->anim];
  else
    bul->texrect.r = rc_right[bul->anim];
}

static void bullet_act_dropspike(bullet_t *bul) {
  static const rect_t rc = {0, 0, 0, 0};

  if (++bul->act_wait > 2) bul->cond = 0;

  bul->texrect.r = rc;
}

static void bullet_act_sword1(bullet_t *bul) {
  if (++bul->count1 > bul->life_count) {
    bul->cond = 0;
    caret_spawn(bul->x, bul->y, CARET_SHOOT, DIR_LEFT);
    return;
  }

  if (bul->count1 == 3) bul->bits &= ~4;

  if (bul->count1 % 5 == 1) snd_play_sound(-1, 34, SOUND_MODE_PLAY);

  if (bul->act == 0) {
    bul->act = 1;

    switch (bul->dir) {
      case DIR_LEFT:
        bul->xvel = -0x800;
        break;
      case DIR_UP:
        bul->yvel = -0x800;
        break;
      case DIR_RIGHT:
        bul->xvel = 0x800;
        break;
      case DIR_DOWN:
        bul->yvel = 0x800;
        break;
    }
  } else {
    bul->x += bul->xvel;
    bul->y += bul->yvel;
  }

  static const rect_t rc_left[4] = {
    {0, 48, 16, 64},
    {16, 48, 32, 64},
    {32, 48, 48, 64},
    {48, 48, 64, 64},
  };

  static const rect_t rc_right[4] = {
    {64, 48, 80, 64},
    {80, 48, 96, 64},
    {96, 48, 112, 64},
    {112, 48, 128, 64},
  };

  if (++bul->anim_wait > 1) {
    bul->anim_wait = 0;
    ++bul->anim;
  }

  if (bul->anim > 3) bul->anim = 0;

  if (bul->dir == DIR_LEFT)
    bul->texrect.r = rc_left[bul->anim];
  else
    bul->texrect.r = rc_right[bul->anim];
}

static void bullet_act_sword2(bullet_t *bul) {
  if (++bul->count1 > bul->life_count) {
    bul->cond = 0;
    caret_spawn(bul->x, bul->y, CARET_SHOOT, DIR_LEFT);
    return;
  }

  if (bul->count1 == 3) bul->bits &= ~4;

  if (bul->count1 % 7 == 1) snd_play_sound(-1, 106, SOUND_MODE_PLAY);

  if (bul->act == 0) {
    bul->act = 1;

    switch (bul->dir) {
      case DIR_LEFT:
        bul->xvel = -0x800;
        break;
      case DIR_UP:
        bul->yvel = -0x800;
        break;
      case DIR_RIGHT:
        bul->xvel = 0x800;
        break;
      case DIR_DOWN:
        bul->yvel = 0x800;
        break;
    }
  } else {
    bul->x += bul->xvel;
    bul->y += bul->yvel;
  }

  static const rect_t rc_left[4] = {
    {160, 48, 184, 72},
    {184, 48, 208, 72},
    {208, 48, 232, 72},
    {232, 48, 256, 72},
  };

  static const rect_t rc_right[4] = {
    {160, 72, 184, 96},
    {184, 72, 208, 96},
    {208, 72, 232, 96},
    {232, 72, 256, 96},
  };

  if (++bul->anim_wait > 1) {
    bul->anim_wait = 0;
    ++bul->anim;
  }

  if (bul->anim > 3) bul->anim = 0;

  if (bul->dir == DIR_LEFT)
    bul->texrect.r = rc_left[bul->anim];
  else
    bul->texrect.r = rc_right[bul->anim];
}

static void bullet_act_sword3(bullet_t *bul) {
  static const rect_t rc_left[2] = {
    {272, 0, 296, 24},
    {296, 0, 320, 24},
  };

  static const rect_t rc_up[2] = {
    {272, 48, 296, 72},
    {296, 0, 320, 24},
  };

  static const rect_t rc_right[2] = {
    {272, 24, 296, 48},
    {296, 24, 320, 48},
  };

  static const rect_t rc_down[2] = {
    {296, 48, 320, 72},
    {296, 24, 320, 48},
  };

  switch (bul->act) {
    case 0:
      bul->act = 1;
      bul->xvel = 0;
      bul->yvel = 0;
      // Fallthrough
    case 1:
      switch (bul->dir) {
        case DIR_LEFT:
          bul->xvel = -0x800;
          break;
        case DIR_UP:
          bul->yvel = -0x800;
          break;
        case DIR_RIGHT:
          bul->xvel = 0x800;
          break;
        case DIR_DOWN:
          bul->yvel = 0x800;
          break;
      }

      if (bul->life != 100) {
        bul->act = 2;
        bul->anim = 1;
        bul->damage = -1;
        bul->act_wait = 0;
      }

      if (++bul->act_wait % 4 == 1) {
        snd_play_sound(-1, 106, SOUND_MODE_PLAY);

        if (++bul->count1 % 2)
          bullet_spawn(23, bul->x, bul->y, DIR_LEFT);
        else
          bullet_spawn(23, bul->x, bul->y, DIR_RIGHT);
      }

      if (++bul->count1 == 5) bul->bits &= ~4;

      if (bul->count1 > bul->life_count) {
        bul->cond = 0;
        caret_spawn(bul->x, bul->y, CARET_SHOOT, DIR_LEFT);
        return;
      }

      break;

    case 2:
      bul->xvel = 0;
      bul->yvel = 0;
      ++bul->act_wait;

      if (m_rand(-1, 1) == 0) {
        snd_play_sound(-1, 106, SOUND_MODE_PLAY);

        if (m_rand(0, 1) % 2)
          bullet_spawn(23, bul->x + (m_rand(-0x40, 0x40) * 0x200), bul->y + (m_rand(-0x40, 0x40) * 0x200), DIR_LEFT);
        else
          bullet_spawn(23, bul->x + (m_rand(-0x40, 0x40) * 0x200), bul->y + (m_rand(-0x40, 0x40) * 0x200), DIR_RIGHT);
      }

      if (bul->act_wait > 50) bul->cond = 0;
  }

  bul->x += bul->xvel;
  bul->y += bul->yvel;

  switch (bul->dir) {
    case DIR_LEFT:
      bul->texrect.r = rc_left[bul->anim];
      break;
    case DIR_UP:
      bul->texrect.r = rc_up[bul->anim];
      break;
    case DIR_RIGHT:
      bul->texrect.r = rc_right[bul->anim];
      break;
    case DIR_DOWN:
      bul->texrect.r = rc_down[bul->anim];
      break;
  }

  if (bul->act_wait % 2) bul->texrect.r.right = 0;
}

static void bullet_act_nemesis(bullet_t *bul) {
  const int level = bul->class_num - 34 + 1;

  if (++bul->count1 > bul->life_count) {
    bul->cond = 0;
    caret_spawn(bul->x, bul->y, CARET_SHOOT, DIR_LEFT);
    return;
  }

  if (bul->act == 0) {
    bul->act = 1;
    bul->count1 = 0;

    switch (bul->dir) {
      case DIR_LEFT:
        bul->xvel = -0x1000;
        break;
      case DIR_UP:
        bul->yvel = -0x1000;
        break;
      case DIR_RIGHT:
        bul->xvel = 0x1000;
        break;
      case DIR_DOWN:
        bul->yvel = 0x1000;
        break;
    }

    switch (level) {
      case 3:
        bul->xvel /= 3;
        bul->yvel /= 3;
        break;
    }
  } else {
    if (level == 1 && bul->count1 % 4 == 1) {
      switch (bul->dir) {
        case DIR_LEFT:
          npc_spawn(4, bul->x, bul->y, -0x200, m_rand(-0x200, 0x200), DIR_RIGHT, NULL, 0x100);
          break;
        case DIR_UP:
          npc_spawn(4, bul->x, bul->y, m_rand(-0x200, 0x200), -0x200, DIR_RIGHT, NULL, 0x100);
          break;
        case DIR_RIGHT:
          npc_spawn(4, bul->x, bul->y, 0x200, m_rand(-0x200, 0x200), DIR_RIGHT, NULL, 0x100);
          break;
        case DIR_DOWN:
          npc_spawn(4, bul->x, bul->y, m_rand(-0x200, 0x200), 0x200, DIR_RIGHT, NULL, 0x100);
          break;
      }
    }

    bul->x += bul->xvel;
    bul->y += bul->yvel;
  }

  if (++bul->anim > 1) bul->anim = 0;

  static const rect_t rc_left[2] = {
    {0, 112, 32, 128},
    {0, 128, 32, 144},
  };

  static const rect_t rc_up[2] = {
    {32, 112, 48, 144},
    {48, 112, 64, 144},
  };

  static const rect_t rc_right[2] = {
    {64, 112, 96, 128},
    {64, 128, 96, 144},
  };

  static const rect_t rc_down[2] = {
    {96, 112, 112, 144},
    {112, 112, 128, 144},
  };

  switch (bul->dir) {
    case DIR_LEFT:
      bul->texrect.r = rc_left[bul->anim];
      break;
    case DIR_UP:
      bul->texrect.r = rc_up[bul->anim];
      break;
    case DIR_RIGHT:
      bul->texrect.r = rc_right[bul->anim];
      break;
    case DIR_DOWN:
      bul->texrect.r = rc_down[bul->anim];
      break;
  }

  bul->texrect.r.top += ((level - 1) / 2) * 32;
  bul->texrect.r.bottom += ((level - 1) / 2) * 32;
  bul->texrect.r.left += ((level - 1) % 2) * 128;
  bul->texrect.r.right += ((level - 1) % 2) * 128;
}

static void bullet_act_spur(bullet_t *bul) {
  const int level = bul->class_num - 37 + 1;

  if (++bul->count1 > bul->life_count) {
    bul->cond = 0;
    caret_spawn(bul->x, bul->y, CARET_SHOOT, DIR_LEFT);
    return;
  }

  if (bul->damage && bul->life != 100) bul->damage = 0;

  if (bul->act == 0) {
    bul->act = 1;

    switch (bul->dir) {
      case DIR_LEFT:
        bul->xvel = -0x1000;
        break;
      case DIR_UP:
        bul->yvel = -0x1000;
        break;
      case DIR_RIGHT:
        bul->xvel = 0x1000;
        break;
      case DIR_DOWN:
        bul->yvel = 0x1000;
        break;
    }

    switch (level) {
      case 1:
        switch (bul->dir) {
          case DIR_LEFT:
            bul->enemy_yl = 0x400;
            break;
          case DIR_UP:
            bul->enemy_xl = 0x400;
            break;
          case DIR_RIGHT:
            bul->enemy_yl = 0x400;
            break;
          case DIR_DOWN:
            bul->enemy_xl = 0x400;
            break;
        }

        break;

      case 2:
        switch (bul->dir) {
          case DIR_LEFT:
            bul->enemy_yl = 0x800;
            break;
          case DIR_UP:
            bul->enemy_xl = 0x800;
            break;
          case DIR_RIGHT:
            bul->enemy_yl = 0x800;
            break;
          case DIR_DOWN:
            bul->enemy_xl = 0x800;
            break;
        }

        break;
    }
  } else {
    bul->x += bul->xvel;
    bul->y += bul->yvel;
  }

  static const rect_t rect1[2] = {
    {128, 32, 144, 48},
    {144, 32, 160, 48},
  };

  static const rect_t rect2[2] = {
    {160, 32, 176, 48},
    {176, 32, 192, 48},
  };

  static const rect_t rect3[2] = {
    {128, 48, 144, 64},
    {144, 48, 160, 64},
  };

  bul->damage = bul->life;

  switch (level) {
    case 1:
      if (bul->dir == DIR_UP || bul->dir == DIR_DOWN)
        bul->texrect.r = rect1[1];
      else
        bul->texrect.r = rect1[0];

      break;

    case 2:
      if (bul->dir == DIR_UP || bul->dir == DIR_DOWN)
        bul->texrect.r = rect2[1];
      else
        bul->texrect.r = rect2[0];

      break;

    case 3:
      if (bul->dir == DIR_UP || bul->dir == DIR_DOWN)
        bul->texrect.r = rect3[1];
      else
        bul->texrect.r = rect3[0];

      break;
  }

  bullet_spawn(39 + level, bul->x, bul->y, bul->dir);
}

static void bullet_act_spurtrail(bullet_t *bul) {
  const int level = bul->class_num - 40 + 1;

  if (++bul->count1 > 20) bul->anim = bul->count1 - 20;

  if (bul->anim > 2) {
    bul->cond = 0;
    return;
  }

  if (bul->damage && bul->life != 100) bul->damage = 0;

  static const rect_t rc_h_lv1[3] = {
    {192, 32, 200, 40},
    {200, 32, 208, 40},
    {208, 32, 216, 40},
  };

  static const rect_t rc_v_lv1[3] = {
  {192, 40, 200, 48},
  {200, 40, 208, 48},
  {208, 40, 216, 48},
  };

  static const rect_t rc_h_lv2[3] = {
    {216, 32, 224, 40},
    {224, 32, 232, 40},
    {232, 32, 240, 40},
  };

  static const rect_t rc_v_lv2[3] = {
    {216, 40, 224, 48},
    {224, 40, 232, 48},
    {232, 40, 240, 48},
  };

  static const rect_t rc_h_lv3[3] = {
    {240, 32, 248, 40},
    {248, 32, 256, 40},
    {256, 32, 264, 40},
  };

  static const rect_t rc_v_lv3[3] = {
    {240, 32, 248, 40},
    {248, 32, 256, 40},
    {256, 32, 264, 40},
  };

  switch (level) {
    case 1:
      if (bul->dir == DIR_LEFT || bul->dir == DIR_RIGHT)
        bul->texrect.r = rc_h_lv1[bul->anim];
      else
        bul->texrect.r = rc_v_lv1[bul->anim];

      break;

    case 2:
      if (bul->dir == DIR_LEFT || bul->dir == DIR_RIGHT)
        bul->texrect.r = rc_h_lv2[bul->anim];
      else
        bul->texrect.r = rc_v_lv2[bul->anim];

      break;

    case 3:
      if (bul->dir == DIR_LEFT || bul->dir == DIR_RIGHT)
        bul->texrect.r = rc_h_lv3[bul->anim];
      else
        bul->texrect.r = rc_v_lv3[bul->anim];

      break;
  }
}

static void bullet_act_screenclear(bullet_t *bul) {
  if (++bul->count1 > bul->life_count) {
    bul->cond = 0;
    return;
  }

  bul->damage = 10000;
  bul->enemy_xl = 0xC8000;
  bul->enemy_yl = 0xC8000;
}

static void bullet_act_star(bullet_t *bul) {
  if (++bul->count1 > bul->life_count)
    bul->cond = 0;
}

typedef void (*bullet_func_t)(bullet_t *);

static const bullet_func_t bullet_functab[] = {
  bullet_act_null,
  // snake
  bullet_act_snake1,
  bullet_act_snake2,
  bullet_act_snake2,
  // polar star
  bullet_act_polarstar,
  bullet_act_polarstar,
  bullet_act_polarstar,
  // fireball
  bullet_act_fireball,
  bullet_act_fireball,
  bullet_act_fireball,
  // machine gun
  bullet_act_machinegun,
  bullet_act_machinegun,
  bullet_act_machinegun,
  // missile launcher
  bullet_act_missile,
  bullet_act_missile,
  bullet_act_missile,
  // missile explosion
  bullet_act_explosion,
  bullet_act_explosion,
  bullet_act_explosion,
  // bubbler
  bullet_act_bubble1,
  bullet_act_bubble2,
  bullet_act_bubble3,
  // bubbler level 3 spine
  bullet_act_bubblespine,
  // blade slash
  bullet_act_bladeslash,
  // falling spike
  bullet_act_dropspike,
  // blade
  bullet_act_sword1,
  bullet_act_sword2,
  bullet_act_sword3,
  // super missile launcher
  bullet_act_supermissile,
  bullet_act_supermissile,
  bullet_act_supermissile,
  // super missile explosion
  bullet_act_superexplosion,
  bullet_act_superexplosion,
  bullet_act_superexplosion,
  // nemesis
  bullet_act_nemesis,
  bullet_act_nemesis,
  bullet_act_nemesis,
  // spur
  bullet_act_spur,
  bullet_act_spur,
  bullet_act_spur,
  // spur trail
  bullet_act_spurtrail,
  bullet_act_spurtrail,
  bullet_act_spurtrail,
  // curly's nemesis
  bullet_act_nemesis,
  // screen nuke
  bullet_act_screenclear,
  // whimsical star
  bullet_act_star,
};

void bullet_act(void) {
  for (int i = 0; i <= bullet_list_max; ++i) {
    bullet_t *bul = &bullet_list[i];
    if (bul->cond & BULLETCOND_ALIVE) {
      if (bul->life < 0) {
        bul->cond = 0;
        // there are other places where a bullet might get destroyed, but whatever
        if (i == bullet_list_max)
          --bullet_list_max;
        continue;
      }
      bullet_functab[bul->class_num](bul);
    }
  }
}

void bullet_destroy(bullet_t *bul) {
  if (bul->class_num < 37 || bul->class_num > 39) {
    snd_play_sound(-1, 28, SOUND_MODE_PLAY);
  } else {
    caret_spawn(bul->x, bul->y, CARET_PROJECTILE_DISSIPATION, DIR_UP);
  }

  bul->cond = 0;
  caret_spawn(bul->x, bul->y, CARET_PROJECTILE_DISSIPATION, DIR_RIGHT);
}

int bullet_count_by_arm(const int arm_id) {
  int count = 0;
  for (int i = 0; i <= bullet_list_max; ++i) {
    if (bullet_list[i].cond & BULLETCOND_ALIVE && (bullet_list[i].class_num + 2) / 3 == arm_id)
      ++count;
  }
  return count;
}

int bullet_count_by_class(const int class_num) {
  int count = 0;
  for (int i = 0; i <= bullet_list_max; ++i) {
    if (bullet_list[i].cond & BULLETCOND_ALIVE && bullet_list[i].class_num == class_num)
      ++count;
  }
  return count;
}
