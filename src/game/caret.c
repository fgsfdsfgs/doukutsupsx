#include <string.h>

#include "engine/common.h"
#include "engine/math.h"
#include "engine/graphics.h"

#include "game/game.h"
#include "game/caret.h"

caret_t caret_list[CARET_MAX];
int caret_list_max = 0;

static const struct {
  s32 view_left;
  s32 view_top;
} caret_recttab[] = {
  {  0,          0         }, // CARET_NULL
  {  4 * 0x200,  4 * 0x200 }, // CARET_BUBBLE
  {  8 * 0x200,  8 * 0x200 }, // CARET_PROJECTILE_DISSIPATION
  {  8 * 0x200,  8 * 0x200 }, // CARET_SHOOT
  {  8 * 0x200,  8 * 0x200 }, // CARET_SNAKE_AFTERIMAGE
  {  4 * 0x200,  4 * 0x200 }, // CARET_ZZZ
  {  8 * 0x200,  8 * 0x200 }, // CARET_SNAKE_AFTERIMAGE_DUPLICATE
  {  4 * 0x200,  4 * 0x200 }, // CARET_EXHAUST
  {  8 * 0x200,  8 * 0x200 }, // CARET_DROWNED_QUOTE
  {  8 * 0x200,  8 * 0x200 }, // CARET_QUESTION_MARK
  { 28 * 0x200,  8 * 0x200 }, // CARET_LEVEL_UP
  {  4 * 0x200,  4 * 0x200 }, // CARET_HURT_PARTICLES
  { 16 * 0x200, 16 * 0x200 }, // CARET_EXPLOSION
  {  4 * 0x200,  4 * 0x200 }, // CARET_TINY_PARTICLES
  { 20 * 0x200, 20 * 0x200 }, // CARET_UNKNOWN
  {  4 * 0x200,  4 * 0x200 }, // CARET_PROJECTILE_DISSIPATION_TINY
  { 20 * 0x200,  4 * 0x200 }, // CARET_EMPTY
  { 52 * 0x200,  4 * 0x200 }  // CARET_PUSH_JUMP_KEY
};

void caret_init(void) {
  memset_word(caret_list, 0, sizeof(caret_list));
  caret_list_max = 0;
}

static inline void caret_draw_instance(caret_t *crt, const int cam_vx, const int cam_vy) {
  const int x = TO_INT(crt->x - crt->view_left);
  const int y = TO_INT(crt->y - crt->view_top);
  // TODO: properly cache caret texrects
  if (!crt->texrect.tpage)
    gfx_set_texrect(&crt->texrect, SURFACE_ID_CARET);
  gfx_draw_texrect(&crt->texrect, GFX_LAYER_FRONT, x - cam_vx, y - cam_vy);
}

void caret_draw(int cam_x, int cam_y) {
  cam_x = TO_INT(cam_x);
  cam_y = TO_INT(cam_y);
  for (int i = 0; i <= caret_list_max; ++i) {
    caret_t *crt = &caret_list[i];
    if (crt->cond && crt->texrect.r.w)
      caret_draw_instance(crt, cam_x, cam_y);
  }
}

// Null
static void caret_act_00(caret_t *crt) {
  (void)crt;
}

// Bubble
static void caret_act_01(caret_t *crt) {
  static const rect_t rc_left[4] = {
    {0, 64, 8, 72},
    {8, 64, 16, 72},
    {16, 64, 24, 72},
    {24, 64, 32, 72},
  };

  static const rect_t rc_right[4] = {
    {64, 24, 72, 32},
    {72, 24, 80, 32},
    {80, 24, 88, 32},
    {88, 24, 96, 32},
  };

  if (crt->act == 0) {
    crt->act = 1;
    crt->xvel = m_rand(-0x400, 0x400);
    crt->yvel = m_rand(-0x400, 0);
  }

  crt->yvel += 0x40;
  crt->x += crt->xvel;
  crt->y += crt->yvel;

  if (++crt->anim_wait > 5) {
    crt->anim_wait = 0;

    if (++crt->anim > 3) {
      crt->cond = 0;
      return;
    }
  }

  if (crt->dir == DIR_LEFT)
    crt->texrect.r = rc_left[crt->anim];
  else
    crt->texrect.r = rc_right[crt->anim];
}

// Projectile dissipation
static void caret_act_02(caret_t *crt) {
  static const rect_t rect_left[4] = {
    {0, 32, 16, 48},
    {16, 32, 32, 48},
    {32, 32, 48, 48},
    {48, 32, 64, 48},
  };

  static const rect_t rect_right[4] = {
    {176, 0, 192, 16},
    {192, 0, 208, 16},
    {208, 0, 224, 16},
    {224, 0, 240, 16},
  };

  static const rect_t rect_up[3] = {
    {0, 32, 16, 48},
    {32, 32, 48, 48},
    {16, 32, 32, 48},
  };

  switch (crt->dir) {
    case DIR_LEFT:
      crt->yvel -= 0x10;
      crt->y += crt->yvel;

      if (++crt->anim_wait > 5) {
        crt->anim_wait = 0;
        ++crt->anim;
      }

      if (crt->anim > 3) {
        crt->cond = 0;
        return;
      }

      crt->texrect.r = rect_left[crt->anim];
      break;

    case DIR_RIGHT:
      if (++crt->anim_wait > 2) {
        crt->anim_wait = 0;
        ++crt->anim;
      }

      if (crt->anim > 3) {
        crt->cond = 0;
        return;
      }

      crt->texrect.r = rect_right[crt->anim];
      break;

    case DIR_UP:
      crt->texrect.r = rect_up[++crt->anim_wait / 2 % 3];

      if (crt->anim_wait > 24)
        crt->cond = 0;

      break;
  }
}

// Shoot
static void caret_act_03(caret_t *crt) {
  static const rect_t rect[4] = {
    {0, 48, 16, 64},
    {16, 48, 32, 64},
    {32, 48, 48, 64},
    {48, 48, 64, 64},
  };

  if (++crt->anim_wait > 2) {
    crt->anim_wait = 0;

    if (++crt->anim > 3) {
      crt->cond = 0;
      return;
    }
  }

  crt->texrect.r = rect[crt->anim];
}

// Snake after-image? This doesn't seem to be used.
static void caret_act_04(caret_t *crt) {
  static const rect_t rect[9] = {
    // Left
    {64, 32, 80, 48},
    {80, 32, 96, 48},
    {96, 32, 112, 48},
    // Up
    {64, 48, 80, 64},
    {80, 48, 96, 64},
    {96, 48, 112, 64},
    // Right
    {64, 64, 80, 80},
    {80, 64, 96, 80},
    {96, 64, 112, 80},
  };

  if (++crt->anim_wait > 1) {
    crt->anim_wait = 0;

    if (++crt->anim > 2) {
      crt->cond = 0;
      return;
    }
  }

  crt->texrect.r = rect[(crt->dir * 3) + crt->anim];
}

// 'Zzz' - snoring
static void caret_act_05(caret_t *crt) {
  static const rect_t rect[7] = {
    {32, 64, 40, 72}, {32, 72, 40, 80}, {40, 64, 48, 72}, {40, 72, 48, 80},
    {40, 64, 48, 72}, {40, 72, 48, 80}, {40, 64, 48, 72},
  };

  if (++crt->anim_wait > 4) {
    crt->anim_wait = 0;
    ++crt->anim;
  }

  if (crt->anim > 6) {
    crt->cond = 0;
    return;
  }

  crt->x += 0x80;
  crt->y -= 0x80;

  crt->texrect.r = rect[crt->anim];
}

// No caret_act_06...

// Exhaust (used by the Booster and hoverbike)
static void caret_act_07(caret_t *crt) {
  static const rect_t rc_left[7] = {
    {56, 0, 64, 8}, {64, 0, 72, 8}, {72, 0, 80, 8}, {80, 0, 88, 8},
    {88, 0, 96, 8}, {96, 0, 104, 8}, {104, 0, 112, 8},
  };

  if (++crt->anim_wait > 1) {
    crt->anim_wait = 0;

    if (++crt->anim > 6) {
      crt->cond = 0;
      return;
    }
  }

  crt->texrect.r = rc_left[crt->anim];

  switch (crt->dir) {
    case DIR_LEFT:
      crt->x -= 2 * 0x200;
      break;
    case DIR_UP:
      crt->y -= 2 * 0x200;
      break;
    case DIR_RIGHT:
      crt->x += 2 * 0x200;
      break;
    case DIR_DOWN:
      crt->y += 2 * 0x200;
      break;
  }
}

// Drowned Quote
static void caret_act_08(caret_t *crt) {
  static const rect_t rc_left = {16, 80, 32, 96};
  static const rect_t rc_right = {32, 80, 48, 96};

  if (crt->dir == DIR_LEFT)
    crt->texrect.r = rc_left;
  else
    crt->texrect.r = rc_right;
}

// The '?' that appears when you press the down key
static void caret_act_09(caret_t *crt) {
  static const rect_t rc_left = {0, 80, 16, 96};
  static const rect_t rc_right = {48, 64, 64, 80};

  if (++crt->anim_wait < 5) crt->y -= 0x800;

  if (crt->anim_wait == 32) crt->cond = 0;

  if (crt->dir == DIR_LEFT)
    crt->texrect.r = rc_left;
  else
    crt->texrect.r = rc_right;
}

// 'Level Up!'
static void caret_act_10(caret_t *crt) {
  static const rect_t rc_left[2] = {
    {0, 0, 56, 16},
    {0, 16, 56, 32},
  };

  static const rect_t rc_right[2] = {
    {0, 96, 56, 112},
    {0, 112, 56, 128},
  };

  ++crt->anim_wait;

  if (crt->dir == DIR_LEFT) {
    if (crt->anim_wait < 20) crt->y -= 2 * 0x200;

    if (crt->anim_wait == 80) crt->cond = 0;
  } else {
    if (crt->anim_wait < 20) crt->y -= 1 * 0x200;

    if (crt->anim_wait == 80) crt->cond = 0;
  }

  if (crt->dir == DIR_LEFT)
    crt->texrect.r = rc_left[crt->anim_wait / 2 % 2];
  else
    crt->texrect.r = rc_right[crt->anim_wait / 2 % 2];
}

// Red hurt particles (used by bosses and invisible hidden pickups)
static void caret_act_11(caret_t *crt) {
  u8 deg;

  if (crt->act == 0) {
    crt->act = 1;
    deg = m_rand(0, 0xFF);
    crt->xvel = m_cos(deg) * 2;
    crt->yvel = m_sin(deg) * 2;
  }

  crt->x += crt->xvel;
  crt->y += crt->yvel;

  static const rect_t rc_right[7] = {
    {56, 8, 64, 16}, {64, 8, 72, 16},  {72, 8, 80, 16},   {80, 8, 88, 16},
    {88, 8, 96, 16}, {96, 8, 104, 16}, {104, 8, 112, 16},
  };

  if (++crt->anim_wait > 2) {
    crt->anim_wait = 0;

    if (++crt->anim > 6) {
      crt->cond = 0;
      return;
    }
  }

  crt->texrect.r = rc_right[crt->anim];
}

// Missile Launcher explosion flash
static void caret_act_12(caret_t *crt) {
  static const rect_t rc_left[2] = {
    {112, 0, 144, 32},
    {144, 0, 176, 32},
  };

  if (++crt->anim_wait > 2) {
    crt->anim_wait = 0;

    if (++crt->anim > 1) {
      crt->cond = 0;
      return;
    }
  }

  crt->texrect.r = rc_left[crt->anim];
}

// Particles used when Quote jumps into the ceiling, and also used by the Demon Crown and Ballos's puppy
static void caret_act_13(caret_t *crt) {
  static const rect_t rc_left[2] = {
    {56, 24, 64, 32},
    {0, 0, 0, 0},
  };

  if (crt->act == 0) {
    crt->act = 1;

    switch (crt->dir) {
      case DIR_LEFT:
        crt->xvel = m_rand(-0x600, 0x600);
        crt->yvel = m_rand(-0x200, 0x200);
        break;

      case DIR_UP:
        crt->yvel = -0x200 * m_rand(1, 3);
        break;
    }
  }

  switch (crt->dir) {
    case DIR_LEFT:
      crt->xvel = (crt->xvel * 4) / 5;
      crt->yvel = (crt->yvel * 4) / 5;
      break;
  }

  crt->x += crt->xvel;
  crt->y += crt->yvel;

  if (++crt->anim_wait > 20) crt->cond = 0;

  crt->texrect.r = rc_left[crt->anim_wait / 2 % 2];

  if (crt->dir == DIR_OTHER) crt->x -= 4 * 0x200;
}

// Broken (unknown and unused)
static void caret_act_14(caret_t *crt) {
  // These rects are invalid.
  // However, notably, there are 5 unused 40x40 sprites at the bottom of Caret.pbm.
  // Perhaps those were originally at these coordinates.
  static const rect_t rect[5] = {
    {0, 96, 40, 136}, {40, 96, 80, 136}, {80, 96, 120, 136},
    {120, 96, 160, 136}, {160, 96, 200, 136},
  };

  if (++crt->anim_wait > 1) {
    crt->anim_wait = 0;

    if (++crt->anim > 4) {
      crt->cond = 0;
      return;
    }
  }

  crt->texrect.r = rect[crt->anim];
}

// Tiny version of the projectile dissipation effect
static void caret_act_15(caret_t *crt) {
  static const rect_t rc_left[4] = {
    {0, 72, 8, 80},
    {8, 72, 16, 80},
    {16, 72, 24, 80},
    {24, 72, 32, 80},
  };

  if (++crt->anim_wait > 2) {
    crt->anim_wait = 0;

    if (++crt->anim > 3) {
      crt->cond = 0;
      return;
    }
  }

  crt->texrect.r = rc_left[crt->anim];
}

// 'Empty!'
static void caret_act_16(caret_t *crt) {
  static const rect_t rc_left[2] = {
    {104, 96, 144, 104},
    {104, 104, 144, 112},
  };

  if (++crt->anim_wait < 10) crt->y -= 2 * 0x200;

  if (crt->anim_wait == 40) crt->cond = 0;

  crt->texrect.r = rc_left[crt->anim_wait / 2 % 2];
}

// 'PUSH JUMP KEY!' (unused)
static void caret_act_17(caret_t *crt) {
  static const rect_t rc_left[2] = {
    {0, 144, 144, 152},
    {0, 0, 0, 0},
  };

  if (++crt->anim_wait >= 40) crt->anim_wait = 0;

  if (crt->anim_wait < 30)
    crt->texrect.r = rc_left[0];
  else
    crt->texrect.r = rc_left[1];
}

typedef void (*caret_func_t)(caret_t *);
static const caret_func_t caret_functab[] = {
  caret_act_00,
  caret_act_01,
  caret_act_02,
  caret_act_03,
  caret_act_04,
  caret_act_05,
  caret_act_04,
  caret_act_07,
  caret_act_08,
  caret_act_09,
  caret_act_10,
  caret_act_11,
  caret_act_12,
  caret_act_13,
  caret_act_14,
  caret_act_15,
  caret_act_16,
  caret_act_17
};

void caret_act(void) {
  for (int i = 0; i <= caret_list_max; ++i) {
    caret_t *crt = &caret_list[i];
    if (crt->cond) {
      crt->texrect.tpage = 0; // force texrect update
      caret_functab[crt->class_num](crt);
      if (!crt->cond && i == caret_list_max)
        --caret_list_max;
    }
  }
}

void caret_spawn(int x, int y, int class_num, int dir) {
  int i;
  for (i = 0; i < CARET_MAX; ++i) {
    if (!caret_list[i].cond)
      break;
  }

  if (i == CARET_MAX) return;

  if (i > caret_list_max)
    caret_list_max = i;

  caret_t *crt = &caret_list[i];
  crt->cond = TRUE;
  crt->class_num = class_num;
  crt->x = x;
  crt->y = y;
  crt->view_left = caret_recttab[class_num].view_left;
  crt->view_top = caret_recttab[class_num].view_top;
  crt->dir = dir;
  crt->xvel = 0;
  crt->yvel = 0;
  crt->act = 0;
  crt->act_wait = 0;
  crt->anim = 0;
  crt->anim_wait = 0;
}
