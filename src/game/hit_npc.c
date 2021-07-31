#include "engine/common.h"
#include "engine/input.h"
#include "engine/sound.h"

#include "game/game.h"
#include "game/hit.h"
#include "game/npc.h"
#include "game/bullet.h"
#include "game/player.h"
#include "game/stage.h"

static inline void hit_check_npc_block(npc_t *npc, int x, int y) {
  int hit = 0;

  if (npc->y - npc->hit.top < (y * 0x10 + 5) * 0x200 &&
      npc->y + npc->hit.bottom > (y * 0x10 - 5) * 0x200 &&
      npc->x - npc->hit.back < (x * 0x10 + 8) * 0x200 &&
      npc->x - npc->hit.back > x * 0x10 * 0x200) {
    npc->x = ((x * 0x10 + 8) * 0x200) + npc->hit.back;
    hit |= 1;
  }

  if (npc->y - npc->hit.top < (y * 0x10 + 5) * 0x200 &&
      npc->y + npc->hit.bottom > (y * 0x10 - 5) * 0x200 &&
      npc->x + npc->hit.back > (x * 0x10 - 8) * 0x200 &&
      npc->x + npc->hit.back < x * 0x10 * 0x200) {
    npc->x = ((x * 0x10 - 8) * 0x200) - npc->hit.back;
    hit |= 4;
  }

  if (npc->x - npc->hit.back < (x * 0x10 + 5) * 0x200 &&
      npc->x + npc->hit.back > (x * 0x10 - 5) * 0x200 &&
      npc->y - npc->hit.top < (y * 0x10 + 8) * 0x200 &&
      npc->y - npc->hit.top > y * 0x10 * 0x200) {
    npc->y = ((y * 0x10 + 8) * 0x200) + npc->hit.top;
    npc->yvel = 0;
    hit |= 2;
  }

  if (npc->x - npc->hit.back < (x * 0x10 + 5) * 0x200 &&
      npc->x + npc->hit.back > (x * 0x10 - 5) * 0x200 &&
      npc->y + npc->hit.bottom > (y * 0x10 - 8) * 0x200 &&
      npc->y + npc->hit.bottom < y * 0x10 * 0x200) {
    npc->y = ((y * 0x10 - 8) * 0x200) - npc->hit.bottom;
    npc->yvel = 0;
    hit |= 8;
  }

  npc->flags |= hit;
}

static inline void hit_check_npc_triangle_a(npc_t *npc, int x, int y) {
  int hit = 0;

  if (npc->x < (x * 0x10 + 8) * 0x200 && npc->x > (x * 0x10 - 8) * 0x200 &&
      npc->y - npc->hit.top <
          (y * 0x10 * 0x200) - ((npc->x - (x * 0x10 * 0x200)) / 2) + 0x800 &&
      npc->y + npc->hit.bottom > (y * 0x10 - 8) * 0x200) {
    // clip
    npc->y = (y * 0x10 * 0x200) - ((npc->x - (x * 0x10 * 0x200)) / 2) + 0x800 +
             npc->hit.top;

    // halt momentum
    if (npc->yvel < 0) npc->yvel = 0;

    // set that hit a ceiling
    hit |= 2;
  }

  npc->flags |= hit;
}

static inline void hit_check_npc_triangle_b(npc_t *npc, int x, int y) {
  int hit = 0;

  if (npc->x < (x * 0x10 + 8) * 0x200 && npc->x > (x * 0x10 - 8) * 0x200 &&
      npc->y - npc->hit.top <
          (y * 0x10 * 0x200) - ((npc->x - (x * 0x10 * 0x200)) / 2) - 0x800 &&
      npc->y + npc->hit.bottom > (y * 0x10 - 8) * 0x200) {
    // clip
    npc->y = (y * 0x10 * 0x200) - ((npc->x - (x * 0x10 * 0x200)) / 2) - 0x800 +
             npc->hit.top;

    // halt momentum
    if (npc->yvel < 0) npc->yvel = 0;

    // set that hit a ceiling
    hit |= 2;
  }

  npc->flags |= hit;
}

static inline void hit_check_npc_triangle_c(npc_t *npc, int x, int y) {
  int hit = 0;

  if (npc->x < (x * 0x10 + 8) * 0x200 && npc->x > (x * 0x10 - 8) * 0x200 &&
      npc->y - npc->hit.top <
          (y * 0x10 * 0x200) + ((npc->x - (x * 0x10 * 0x200)) / 2) - 0x800 &&
      npc->y + npc->hit.bottom > (y * 0x10 - 8) * 0x200) {
    // clip
    npc->y = (y * 0x10 * 0x200) + ((npc->x - (x * 0x10 * 0x200)) / 2) - 0x800 +
             npc->hit.top;

    // halt momentum
    if (npc->yvel < 0) npc->yvel = 0;

    // set that hit a ceiling
    hit |= 2;
  }

  npc->flags |= hit;
}

static inline void hit_check_npc_triangle_d(npc_t *npc, int x, int y) {
  int hit = 0;

  if (npc->x < (x * 0x10 + 8) * 0x200 && npc->x > (x * 0x10 - 8) * 0x200 &&
      npc->y - npc->hit.top <
          (y * 0x10 * 0x200) + ((npc->x - (x * 0x10 * 0x200)) / 2) + 0x800 &&
      npc->y + npc->hit.bottom > (y * 0x10 - 8) * 0x200) {
    // clip
    npc->y = (y * 0x10 * 0x200) + ((npc->x - (x * 0x10 * 0x200)) / 2) + 0x800 +
             npc->hit.top;

    // halt momentum
    if (npc->yvel < 0) npc->yvel = 0;

    // set that hit a ceiling
    hit |= 2;
  }

  npc->flags |= hit;
}

static inline void hit_check_npc_triangle_e(npc_t *npc, int x, int y) {
  int hit = 0;

  hit |= 0x10000;

  if (npc->x < (x * 0x10 + 8) * 0x200 && npc->x > (x * 0x10 - 8) * 0x200 &&
      npc->y + npc->hit.bottom >
          (y * 0x10 * 0x200) + ((npc->x - (x * 0x10 * 0x200)) / 2) - 0x800 &&
      npc->y - npc->hit.top < (y * 0x10 + 8) * 0x200) {
    // clip
    npc->y = (y * 0x10 * 0x200) + ((npc->x - (x * 0x10 * 0x200)) / 2) - 0x800 -
             npc->hit.bottom;

    // halt momentum
    if (npc->yvel > 0) npc->yvel = 0;

    // set that hit this slope
    hit |= 0x28;
  }

  npc->flags |= hit;
}

static inline void hit_check_npc_triangle_f(npc_t *npc, int x, int y) {
  int hit = 0;

  hit |= 0x20000;

  if (npc->x < (x * 0x10 + 8) * 0x200 &&
      npc->x >= (x * 0x10 - 8) * 0x200  // note that this function uses '>='.
                                        // i'm not sure if this is a bug.
      && npc->y + npc->hit.bottom >
             (y * 0x10 * 0x200) + ((npc->x - (x * 0x10 * 0x200)) / 2) + 0x800 &&
      npc->y - npc->hit.top < (y * 0x10 + 8) * 0x200) {
    // clip
    npc->y = (y * 0x10 * 0x200) + ((npc->x - (x * 0x10 * 0x200)) / 2) + 0x800 -
             npc->hit.bottom;

    // halt momentum
    if (npc->yvel > 0) npc->yvel = 0;

    // set that hit this slope
    hit |= 0x28;
  }

  npc->flags |= hit;
}

static inline void hit_check_npc_triangle_g(npc_t *npc, int x, int y) {
  int hit = 0;

  hit |= 0x40000;

  if (npc->x < (x * 0x10 + 8) * 0x200 && npc->x > (x * 0x10 - 8) * 0x200 &&
      npc->y + npc->hit.bottom >
          (y * 0x10 * 0x200) - ((npc->x - (x * 0x10 * 0x200)) / 2) + 0x800 &&
      npc->y - npc->hit.top < (y * 0x10 + 8) * 0x200) {
    // clip
    npc->y = (y * 0x10 * 0x200) - ((npc->x - (x * 0x10 * 0x200)) / 2) + 0x800 -
             npc->hit.bottom;

    // halt momentum
    if (npc->yvel > 0) npc->yvel = 0;

    // set that hit this slope
    hit |= 0x18;
  }

  npc->flags |= hit;
}

static inline void hit_check_npc_triangle_h(npc_t *npc, int x, int y) {
  int hit = 0;

  hit |= 0x80000;

  if (npc->x < (x * 0x10 + 8) * 0x200 && npc->x > (x * 0x10 - 8) * 0x200 &&
      npc->y + npc->hit.bottom >
          (y * 0x10 * 0x200) - ((npc->x - (x * 0x10 * 0x200)) / 2) - 0x800 &&
      npc->y - npc->hit.top < (y * 0x10 + 8) * 0x200) {
    // clip
    npc->y = (y * 0x10 * 0x200) - ((npc->x - (x * 0x10 * 0x200)) / 2) - 0x800 -
             npc->hit.bottom;

    // halt momentum
    if (npc->yvel > 0) npc->yvel = 0;

    // set that hit this slope
    hit |= 0x18;
  }

  npc->flags |= hit;
}

static inline void hit_check_npc_water(npc_t *npc, int x, int y) {
  int hit = 0;

  if (npc->x - npc->hit.back < (x * 0x10 + 6) * 0x200 &&
      npc->x + npc->hit.back > (x * 0x10 - 6) * 0x200 &&
      npc->y - npc->hit.top < (y * 0x10 + 6) * 0x200 &&
      npc->y + npc->hit.bottom > (y * 0x10 - 6) * 0x200)
    hit |= 0x100;

  npc->flags |= hit;
}

void hit_npc_map(void) {
  static const int ofsx[9] = { 0, 1, 0, 1, 2, 2, 2, 0, 1 };
  static const int ofsy[9] = { 0, 0, 1, 1, 0, 1, 2, 2, 2 };

  for (int i = 0; i <= npc_list_max; ++i) {
    npc_t *npc = &npc_list[i];

    if (!(npc->cond & NPCCOND_ALIVE))
      continue;

    if (npc->bits & NPC_IGNORE_SOLIDITY)
      continue;

    int x, y;
    int num_checks;
    if (npc->info->size >= 3) {
      num_checks = 9;
      x = TO_INT(npc->x - 0x1000) / TILE_SIZE;
      y = TO_INT(npc->y - 0x1000) / TILE_SIZE;
    } else {
      num_checks = 4;
      x = TO_INT(npc->x) / TILE_SIZE;
      y = TO_INT(npc->y) / TILE_SIZE;
    }

    npc->flags = 0;

    for (int j = 0; j < num_checks; ++j) {
      const int ox = x + ofsx[j];
      const int oy = y + ofsy[j];
      const u8 atrb = stage_get_atrb(ox, oy);

      switch (atrb) {
        // No NPC block
        case 0x44:
          if (npc->bits & NPC_IGNORE_TILE_44) break;
          // Fallthrough
        // Block
        case 0x03:
        case 0x05:
        case 0x41:
        case 0x43:
          hit_check_npc_block(npc, ox, oy);
          break;

        // slopes
        case 0x50:
          hit_check_npc_triangle_a(npc, ox, oy);
          break;

        case 0x51:
          hit_check_npc_triangle_b(npc, ox, oy);
          break;

        case 0x52:
          hit_check_npc_triangle_c(npc, ox, oy);
          break;

        case 0x53:
          hit_check_npc_triangle_d(npc, ox, oy);
          break;

        case 0x54:
          hit_check_npc_triangle_e(npc, ox, oy);
          break;

        case 0x55:
          hit_check_npc_triangle_f(npc, ox, oy);
          break;

        case 0x56:
          hit_check_npc_triangle_g(npc, ox, oy);
          break;

        case 0x57:
          hit_check_npc_triangle_h(npc, ox, oy);
          break;

        // water
        case 0x02:
        case 0x60:
        case 0x62:
          hit_check_npc_water(npc, ox, oy);
          break;

        // water block
        case 0x04:
        case 0x61:
        case 0x64:
          hit_check_npc_block(npc, ox, oy);
          hit_check_npc_water(npc, ox, oy);
          break;

        // water slopes
        case 0x70:
          hit_check_npc_triangle_a(npc, ox, oy);
          hit_check_npc_water(npc, ox, oy);
          break;

        case 0x71:
          hit_check_npc_triangle_b(npc, ox, oy);
          hit_check_npc_water(npc, ox, oy);
          break;

        case 0x72:
          hit_check_npc_triangle_c(npc, ox, oy);
          hit_check_npc_water(npc, ox, oy);
          break;

        case 0x73:
          hit_check_npc_triangle_d(npc, ox, oy);
          hit_check_npc_water(npc, ox, oy);
          break;

        case 0x74:
          hit_check_npc_triangle_e(npc, ox, oy);
          hit_check_npc_water(npc, ox, oy);
          break;

        case 0x75:
          hit_check_npc_triangle_f(npc, ox, oy);
          hit_check_npc_water(npc, ox, oy);
          break;

        case 0x76:
          hit_check_npc_triangle_g(npc, ox, oy);
          hit_check_npc_water(npc, ox, oy);
          break;

        case 0x77:
          hit_check_npc_triangle_h(npc, ox, oy);
          hit_check_npc_water(npc, ox, oy);
          break;

        case 0xA0:
          npc->flags |= 0x100;
          // Fallthrough
        case 0x80:
          npc->flags |= 0x1000;
          break;

        case 0xA1:
          npc->flags |= 0x100;
          // Fallthrough
        case 0x81:
          npc->flags |= 0x2000;
          break;

        case 0xA2:
          npc->flags |= 0x100;
          // Fallthrough
        case 0x82:
          npc->flags |= 0x4000;
          break;

        case 0xA3:
          npc->flags |= 0x100;
          // Fallthrough
        case 0x83:
          npc->flags |= 0x8000;
          break;
      }

      if (npc->y > stage_water_y + 0x800)
        npc->flags |= 0x100;
    }
  }
}

void hit_npc_bullet(void) {
  bool hit;

  for (int n = 0; n <= npc_list_max; ++n) {
    npc_t *npc = &npc_list[n];

    if (!(npc->cond & NPCCOND_ALIVE))
      continue;

    if (npc->bits & NPC_SHOOTABLE && npc->bits & NPC_INTERACTABLE)
      continue;

    for (int b = 0; b <= bullet_list_max; ++b) {
      bullet_t *bul = &bullet_list[b];

      if (!(bul->cond & BULLETCOND_ALIVE))
        continue;

      if (bul->damage == -1)
        continue;

      // Check if bullet touches npc
      hit = FALSE;
      if (npc->bits & NPC_SHOOTABLE
        && npc->x - npc->hit.back < bul->x + bul->enemy_xl
        && npc->x + npc->hit.back > bul->x - bul->enemy_xl
        && npc->y - npc->hit.top < bul->y + bul->enemy_yl
        && npc->y + npc->hit.bottom > bul->y - bul->enemy_yl)
        hit = TRUE;
      else if (npc->bits & NPC_INVULNERABLE
        && npc->x - npc->hit.back < bul->x + bul->block_xl
        && npc->x + npc->hit.back > bul->x - bul->block_xl
        && npc->y - npc->hit.top < bul->y + bul->block_yl
        && npc->y + npc->hit.bottom > bul->y - bul->block_yl)
        hit = TRUE;

      if (hit) {
        // Damage NPC
        if (npc->bits & NPC_SHOOTABLE) {
          npc->life -= bul->damage;

          if (npc->life < 1) {
            npc->life = 0;

            if (npc->bits & NPC_SHOW_DAMAGE)
              npc->damage_view -= bul->damage;

            if ((player.cond & PLRCOND_ALIVE) && npc->bits & NPC_EVENT_WHEN_KILLED) {
              // StartTextScript(npc->event_num);
            } else {
              npc->cond |= NPCCOND_KILLED;
            }
          } else {
            if (npc->shock < 14) {
              // SetCaret((bul->x + npc->x) / 2, (bul->y + npc->y) / 2, CARET_HURT_PARTICLES, DIR_LEFT);
              // SetCaret((bul->x + npc->x) / 2, (bul->y + npc->y) / 2, CARET_HURT_PARTICLES, DIR_LEFT);
              // SetCaret((bul->x + npc->x) / 2, (bul->y + npc->y) / 2, CARET_HURT_PARTICLES, DIR_LEFT);
              snd_play_sound(-1, npc->snd_hit, SOUND_MODE_PLAY);
              npc->shock = 16;
            }

            if (npc->bits & NPC_SHOW_DAMAGE)
              npc->damage_view -= bul->damage;
          }
        } else if (!(bul->bits & 0x10)) {
          // Hit invulnerable NPC
          // SetCaret((bul->x + npc->x) / 2, (bul->y + npc->y) / 2, CARET_PROJECTILE_DISSIPATION, DIR_RIGHT);
          snd_play_sound(-1, 31, SOUND_MODE_PLAY);
          bul->life = 0;
          continue;
        }

        --bul->life;
      }
    }

    if (npc->cond & NPCCOND_KILLED)
      npc_kill(npc, TRUE);
  }
}
