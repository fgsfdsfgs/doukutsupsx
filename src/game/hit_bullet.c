#include "engine/common.h"
#include "engine/math.h"
#include "engine/input.h"
#include "engine/sound.h"

#include "game/bullet.h"
#include "game/game.h"
#include "game/hit.h"
#include "game/npc.h"
#include "game/player.h"
#include "game/stage.h"
#include "game/caret.h"

static inline int hit_check_bullet_block(int x, int y, bullet_t *bul, const u8 atrb) {
  int i;
  int hit = 0;
  if (bul->x - bul->block_xl < ((x * 16) + 8) * 0x200 && bul->x + bul->block_xl > ((x * 16) - 8) * 0x200 &&
      bul->y - bul->block_yl < ((y * 16) + 8) * 0x200 && bul->y + bul->block_yl > ((y * 16) - 8) * 0x200)
    hit |= 0x200;

  if (hit && bul->bits & 0x60 && atrb == 0x43) {
    if (!(bul->bits & 0x40)) bul->cond = 0;

    caret_spawn(bul->x, bul->y, CARET_PROJECTILE_DISSIPATION, DIR_LEFT);
    snd_play_sound(PRIO_NORMAL, 12, FALSE);

    for (i = 0; i < 4; ++i)
      npc_spawn(4, x * 0x200 * 0x10, y * 0x200 * 0x10, m_rand(-0x200, 0x200), m_rand(-0x200, 0x200), 0, NULL, 0x100);

    stage_shift_tile(x, y);
  }

  return hit;
}

static inline int hit_check_bullet_block2(int x, int y, unsigned char *atrb, bullet_t *bul) {
  bool block[4];
  int work_x, work_y;
  int hit, i;

  hit = 0;

  if (bul->bits & 0x40) {
    for (i = 0; i < 4; ++i) {
      if (*atrb == 0x41 || *atrb == 0x61)
        block[i] = TRUE;
      else
        block[i] = FALSE;

      ++atrb;
    }
  } else {
    for (i = 0; i < 4; ++i) {
      if (*atrb == 0x41 || *atrb == 0x43 || *atrb == 0x61)
        block[i] = TRUE;
      else
        block[i] = FALSE;

      ++atrb;
    }
  }

  work_x = ((x * 16) + 8) * 0x200;
  work_y = ((y * 16) + 8) * 0x200;

  // left wall
  if (block[0] && block[2]) {
    if (bul->x - bul->block_xl < work_x) hit |= 1;
  } else if (block[0] && !block[2]) {
    if (bul->x - bul->block_xl < work_x && bul->y - bul->block_yl < work_y - (3 * 0x200)) hit |= 1;
  } else if (!block[0] && block[2]) {
    if (bul->x - bul->block_xl < work_x && bul->y + bul->block_yl > work_y + (3 * 0x200)) hit |= 1;
  }

  // right wall
  if (block[1] && block[3]) {
    if (bul->x + bul->block_xl > work_x) hit |= 4;
  } else if (block[1] && !block[3]) {
    if (bul->x + bul->block_xl > work_x && bul->y - bul->block_yl < work_y - (3 * 0x200)) hit |= 4;
  } else if (!block[1] && block[3]) {
    if (bul->x + bul->block_xl > work_x && bul->y + bul->block_yl > work_y + (3 * 0x200)) hit |= 4;
  }

  // ceiling
  if (block[0] && block[1]) {
    if (bul->y - bul->block_yl < work_y) hit |= 2;
  } else if (block[0] && !block[1]) {
    if (bul->y - bul->block_yl < work_y && bul->x - bul->block_xl < work_x - (3 * 0x200)) hit |= 2;
  } else if (!block[0] && block[1]) {
    if (bul->y - bul->block_yl < work_y && bul->x + bul->block_xl > work_x + (3 * 0x200)) hit |= 2;
  }

  // ground
  if (block[2] && block[3]) {
    if (bul->y + bul->block_yl > work_y) hit |= 8;
  } else if (block[2] && !block[3]) {
    if (bul->y + bul->block_yl > work_y && bul->x - bul->block_xl < work_x - (3 * 0x200)) hit |= 8;
  } else if (!block[2] && block[3]) {
    if (bul->y + bul->block_yl > work_y && bul->x + bul->block_xl > work_x + (3 * 0x200)) hit |= 8;
  }

  // clip
  if (bul->bits & 8) {
    if (hit & 1)
      bul->x = work_x + bul->block_xl;
    else if (hit & 4)
      bul->x = work_x - bul->block_xl;
    else if (hit & 2)
      bul->y = work_y + bul->block_yl;
    else if (hit & 8)
      bul->y = work_y - bul->block_yl;
  } else {
    if (hit & 0xF) bullet_destroy(bul);
  }

  return hit;
}

static inline int hit_check_bullet_triangle_a(int x, int y, bullet_t *bul) {
  int hit = 0;
  if (bul->x < ((x * 16) + 8) * 0x200 && bul->x > ((x * 16) - 8) * 0x200 &&
      bul->y - (2 * 0x200) < ((y * 16) * 0x200) - ((bul->x - ((x * 16) * 0x200)) / 2) + (4 * 0x200) &&
      bul->y + (2 * 0x200) > ((y * 16) - 8) * 0x200) {
    if (bul->bits & 8)
      bul->y = ((y * 16) * 0x200) - ((bul->x - ((x * 16) * 0x200)) / 2) + (6 * 0x200);
    else
      bullet_destroy(bul);

    hit |= 0x82;
  }

  return hit;
}

static inline int hit_check_bullet_triangle_b(int x, int y, bullet_t *bul) {
  int hit = 0;
  if (bul->x < ((x * 16) + 8) * 0x200 && bul->x > ((x * 16) - 8) * 0x200 &&
      bul->y - (2 * 0x200) < ((y * 16) * 0x200) - ((bul->x - ((x * 16) * 0x200)) / 2) - (4 * 0x200) &&
      bul->y + (2 * 0x200) > ((y * 16) - 8) * 0x200) {
    if (bul->bits & 8)
      bul->y = ((y * 16) * 0x200) - ((bul->x - ((x * 16) * 0x200)) / 2) - (2 * 0x200);
    else
      bullet_destroy(bul);

    hit |= 0x82;
  }

  return hit;
}

static inline int hit_check_bullet_triangle_c(int x, int y, bullet_t *bul) {
  int hit = 0;
  if (bul->x < ((x * 16) + 8) * 0x200 && bul->x > ((x * 16) - 8) * 0x200 &&
      bul->y - (2 * 0x200) < ((y * 16) * 0x200) + ((bul->x - ((x * 16) * 0x200)) / 2) - (4 * 0x200) &&
      bul->y + (2 * 0x200) > ((y * 16) - 8) * 0x200) {
    if (bul->bits & 8)
      bul->y = ((y * 16) * 0x200) + ((bul->x - ((x * 16) * 0x200)) / 2) - (2 * 0x200);
    else
      bullet_destroy(bul);

    hit |= 0x42;
  }

  return hit;
}

static inline int hit_check_bullet_triangle_d(int x, int y, bullet_t *bul) {
  int hit = 0;
  if (bul->x < ((x * 16) + 8) * 0x200 && bul->x > ((x * 16) - 8) * 0x200 &&
      bul->y - (2 * 0x200) < ((y * 16) * 0x200) + ((bul->x - ((x * 16) * 0x200)) / 2) + (4 * 0x200) &&
      bul->y + (2 * 0x200) > ((y * 16) - 8) * 0x200) {
    if (bul->bits & 8)
      bul->y = ((y * 16) * 0x200) + ((bul->x - ((x * 16) * 0x200)) / 2) + (6 * 0x200);
    else
      bullet_destroy(bul);

    hit |= 0x42;
  }

  return hit;
}

static inline int hit_check_bullet_triangle_e(int x, int y, bullet_t *bul) {
  int hit = 0;
  if (bul->x < ((x * 16) + 8) * 0x200 && bul->x - (1 * 0x200) > ((x * 16) - 8) * 0x200 &&
      bul->y + (2 * 0x200) > ((y * 16) * 0x200) + ((bul->x - ((x * 16) * 0x200)) / 2) - (4 * 0x200) &&
      bul->y - (2 * 0x200) < ((y * 16) + 8) * 0x200) {
    if (bul->bits & 8)
      bul->y = ((y * 16) * 0x200) + ((bul->x - ((x * 16) * 0x200)) / 2) - (6 * 0x200);
    else
      bullet_destroy(bul);

    hit |= 0x28;
  }

  return hit;
}

static inline int hit_check_bullet_triangle_f(int x, int y, bullet_t *bul) {
  int hit = 0;
  if (bul->x < ((x * 16) + 8) * 0x200 && bul->x > ((x * 16) - 8) * 0x200 &&
      bul->y + (2 * 0x200) > ((y * 16) * 0x200) + ((bul->x - ((x * 16) * 0x200)) / 2) + (4 * 0x200) &&
      bul->y - (2 * 0x200) < ((y * 16) + 8) * 0x200) {
    if (bul->bits & 8)
      bul->y = ((y * 16) * 0x200) + ((bul->x - ((x * 16) * 0x200)) / 2) + (2 * 0x200);
    else
      bullet_destroy(bul);

    hit |= 0x28;
  }

  return hit;
}

static inline int hit_check_bullet_triangle_g(int x, int y, bullet_t *bul) {
  int hit = 0;
  if (bul->x < ((x * 16) + 8) * 0x200 && bul->x > ((x * 16) - 8) * 0x200 &&
      bul->y + (2 * 0x200) > ((y * 16) * 0x200) - ((bul->x - ((x * 16) * 0x200)) / 2) + (4 * 0x200) &&
      bul->y - (2 * 0x200) < ((y * 16) + 8) * 0x200) {
    if (bul->bits & 8)
      bul->y = ((y * 16) * 0x200) - ((bul->x - ((x * 16) * 0x200)) / 2) + (2 * 0x200);
    else
      bullet_destroy(bul);

    hit |= 0x18;
  }

  return hit;
}

static inline int hit_check_bullet_triangle_h(int x, int y, bullet_t *bul) {
  int hit = 0;
  if (bul->x < ((x * 16) + 8) * 0x200 && bul->x > ((x * 16) - 8) * 0x200 &&
      bul->y + (2 * 0x200) > ((y * 16) * 0x200) - ((bul->x - ((x * 16) * 0x200)) / 2) - (4 * 0x200) &&
      bul->y - (2 * 0x200) < ((y * 16) + 8) * 0x200) {
    if (bul->bits & 8)
      bul->y = ((y * 16) * 0x200) - ((bul->x - ((x * 16) * 0x200)) / 2) - (6 * 0x200);
    else
      bullet_destroy(bul);

    hit |= 0x18;
  }

  return hit;
}

void hit_bullet_map(void) {
  static const int ofsx[4] = {0, 1, 0, 1};
  static const int ofsy[4] = {0, 0, 1, 1};
  int x, y, ox, oy;
  u8 atrb[4];

  for (int i = 0; i <= bullet_list_max; ++i) {
    bullet_t *bul = &bullet_list[i];
  
    if (!(bul->cond & BULLETCOND_ALIVE))
      continue;

    x = TO_INT(bul->x) / TILE_SIZE;
    y = TO_INT(bul->y) / TILE_SIZE;

    // Get surrounding tiles
    atrb[0] = stage_get_atrb(x, y);
    atrb[1] = stage_get_atrb(x + 1, y);
    atrb[2] = stage_get_atrb(x, y + 1);
    atrb[3] = stage_get_atrb(x + 1, y + 1);

    // Clear hit tiles
    bul->flags = 0;

    // bit 2 = don't hit map tiles
    if (bul->bits & 4)
      continue;

    for (int j = 0; j < 4; ++j) {
      // if the bullet got killed at some point, bail out
      if (!(bul->cond & BULLETCOND_ALIVE))
        break;

      ox = x + ofsx[j];
      oy = y + ofsy[j];

      switch (atrb[j]) {
        case 0x41:
        case 0x43:
        case 0x44:
        case 0x61:
        case 0x64:
          bul->flags |= hit_check_bullet_block(ox, oy, bul, atrb[j]);
          break;

        case 0x50:
        case 0x70:
          bul->flags |= hit_check_bullet_triangle_a(ox, oy, bul);
          break;

        case 0x51:
        case 0x71:
          bul->flags |= hit_check_bullet_triangle_b(ox, oy, bul);
          break;

        case 0x52:
        case 0x72:
          bul->flags |= hit_check_bullet_triangle_c(ox, oy, bul);
          break;

        case 0x53:
        case 0x73:
          bul->flags |= hit_check_bullet_triangle_d(ox, oy, bul);
          break;

        case 0x54:
        case 0x74:
          bul->flags |= hit_check_bullet_triangle_e(ox, oy, bul);
          break;

        case 0x55:
        case 0x75:
          bul->flags |= hit_check_bullet_triangle_f(ox, oy, bul);
          break;

        case 0x56:
        case 0x76:
          bul->flags |= hit_check_bullet_triangle_g(ox, oy, bul);
          break;

        case 0x57:
        case 0x77:
          bul->flags |= hit_check_bullet_triangle_h(ox, oy, bul);
          break;
      }
    }

    bul->flags |= hit_check_bullet_block2(x, y, atrb, bul);
  }
}
