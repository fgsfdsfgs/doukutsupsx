#include "engine/common.h"
#include "engine/sound.h"
#include "engine/input.h"

#include "game/stage.h"
#include "game/game.h"
#include "game/npc.h"
#include "game/player.h"

static inline void star_particle(void) {
  if (!(player.cond & 2) && player.yvel < -0x200) {
    snd_play_sound(CHAN_STEP, 3, SOUND_MODE_PLAY);
    // set_caret(player.x, player.y - player.hit.top, CARET_TINY_PARTICLES, DIR_LEFT);
    // set_caret(player.x, player.y - player.hit.top, CARET_TINY_PARTICLES, DIR_LEFT);
  }
}

static inline int hit_check_player_block(const u32 btns, int x, int y) {
  int hit = 0;

  // left wall
  if (player.y - player.hit.top < (y * 0x10 + 4) * 0x200 &&
      player.y + player.hit.bottom > (y * 0x10 - 4) * 0x200 &&
      player.x - player.hit.back < (x * 0x10 + 8) * 0x200 &&
      player.x - player.hit.back > x * 0x10 * 0x200) {
    // clip
    player.x = ((x * 0x10 + 8) * 0x200) + player.hit.back;

    // halt momentum
    if (player.xvel < -0x180) player.xvel = -0x180;
    if (!(btns & IN_LEFT) && player.xvel < 0) player.xvel = 0;

    // set that a left wall was hit
    hit |= 1;
  }

  // right wall
  if (player.y - player.hit.top < (y * 0x10 + 4) * 0x200 &&
      player.y + player.hit.bottom > (y * 0x10 - 4) * 0x200 &&
      player.x + player.hit.back > (x * 0x10 - 8) * 0x200 &&
      player.x + player.hit.back < x * 0x10 * 0x200) {
    // clip
    player.x = ((x * 0x10 - 8) * 0x200) - player.hit.back;

    // halt momentum
    if (player.xvel > 0x180) player.xvel = 0x180;
    if (!(btns & IN_RIGHT) && player.xvel > 0) player.xvel = 0;

    // set that a right wall was hit
    hit |= 4;
  }

  // ceiling
  if (player.x - player.hit.back < (x * 0x10 + 5) * 0x200 &&
      player.x + player.hit.back > (x * 0x10 - 5) * 0x200 &&
      player.y - player.hit.top < (y * 0x10 + 8) * 0x200 &&
      player.y - player.hit.top > y * 0x10 * 0x200) {
    // clip
    player.y = ((y * 0x10 + 8) * 0x200) + player.hit.top;

    // halt momentum
    if (!(player.cond & 2) && player.yvel < -0x200) star_particle();
    if (player.yvel < 0) player.yvel = 0;

    // set that a ceiling was hit
    hit |= 2;
  }

  // floor
  if (player.x - player.hit.back < (x * 0x10 + 5) * 0x200 &&
      player.x + player.hit.back > ((x * 0x10 - 5) * 0x200) &&
      player.y + player.hit.bottom > (y * 0x10 - 8) * 0x200 &&
      player.y + player.hit.bottom < y * 0x10 * 0x200) {
    // clip
    player.y = ((y * 0x10 - 8) * 0x200) - player.hit.bottom;

    // halt momentum
    if (player.yvel > 0x400) snd_play_sound(CHAN_STEP, 23, SOUND_MODE_PLAY);
    if (player.yvel > 0) player.yvel = 0;

    // set that a floor was hit
    hit |= 8;
  }

  return hit;
}

static inline int hit_check_player_slope_a(const u32 btns, int x, int y) {
  int hit = 0;

  if (player.x < (x * 0x10 + 8) * 0x200 && player.x > (x * 0x10 - 8) * 0x200 &&
      player.y - player.hit.top < (y * 0x10 * 0x200) - (player.x - x * 0x10 * 0x200) / 2 + 0x800 &&
      player.y + player.hit.bottom > (y * 0x10 - 8) * 0x200) {
    // clip
    player.y = (y * 0x10 * 0x200) - ((player.x - x * 0x10 * 0x200) / 2) +
               0x800 + player.hit.top;

    // halt momentum
    if (!(player.cond & 2) && player.yvel < -0x200) star_particle();
    if (player.yvel < 0) player.yvel = 0;

    // set that hit a ceiling
    hit |= 2;
  }

  return hit;
}

static inline int hit_check_player_slope_b(const u32 btns, int x, int y) {
  int hit = 0;

  if (player.x < (x * 0x10 + 8) * 0x200 && player.x > (x * 0x10 - 8) * 0x200 &&
      player.y - player.hit.top < (y * 0x10 * 0x200) - ((player.x - x * 0x10 * 0x200) / 2) - 0x800 &&
      player.y + player.hit.bottom > (y * 0x10 - 8) * 0x200) {
    // clip
    player.y = (y * 0x10 * 0x200) - ((player.x - x * 0x10 * 0x200) / 2) -
               0x800 + player.hit.top;

    // halt momentum
    if (!(player.cond & 2) && player.yvel < -0x200) star_particle();
    if (player.yvel < 0) player.yvel = 0;

    // set that hit a ceiling
    hit |= 2;
  }

  return hit;
}

static inline int hit_check_player_slope_c(const u32 btns, int x, int y) {
  int hit = 0;

  if (player.x < (x * 0x10 + 8) * 0x200 && player.x > (x * 0x10 - 8) * 0x200 &&
      player.y - player.hit.top < (y * 0x10 * 0x200) + ((player.x - x * 0x10 * 0x200) / 2) - 0x800 &&
      player.y + player.hit.bottom > (y * 0x10 - 8) * 0x200) {
    // clip
    player.y = (y * 0x10 * 0x200) + ((player.x - x * 0x10 * 0x200) / 2) -
               0x800 + player.hit.top;

    // halt momentum
    if (!(player.cond & 2) && player.yvel < -0x200) star_particle();
    if (player.yvel < 0) player.yvel = 0;

    // set that hit a ceiling
    hit |= 2;
  }

  return hit;
}

static inline int hit_check_player_slope_d(const u32 btns, int x, int y) {
  int hit = 0;

  if (player.x < (x * 0x10 + 8) * 0x200 && player.x > (x * 0x10 - 8) * 0x200 &&
      player.y - player.hit.top < (y * 0x10 * 0x200) + ((player.x - x * 0x10 * 0x200) / 2) + 0x800 &&
      player.y + player.hit.bottom > (y * 0x10 - 8) * 0x200) {
    // clip
    player.y = (y * 0x10 * 0x200) + ((player.x - x * 0x10 * 0x200) / 2) +
               0x800 + player.hit.top;

    // halt momentum
    if (!(player.cond & 2) && player.yvel < -0x200) star_particle();
    if (player.yvel < 0) player.yvel = 0;

    // set that hit a ceiling
    hit |= 2;
  }

  return hit;
}

static inline int hit_check_player_slope_e(const u32 btns, int x, int y) {
  int hit = 0;

  hit |= 0x10000;

  if (player.x < (x * 0x10 + 8) * 0x200 && player.x > (x * 0x10 - 8) * 0x200 &&
      player.y + player.hit.bottom > (y * 0x10 * 0x200) + ((player.x - x * 0x10 * 0x200) / 2) - 0x800 &&
      player.y - player.hit.top < (y * 0x10 + 8) * 0x200) {
    // clip
    player.y = (y * 0x10 * 0x200) + ((player.x - x * 0x10 * 0x200) / 2) -
               0x800 - player.hit.bottom;

    // halt momentum
    if (player.yvel > 0x400) snd_play_sound(CHAN_STEP, 23, SOUND_MODE_PLAY);
    if (player.yvel > 0) player.yvel = 0;

    // set that hit this slope
    hit |= 0x28;
  }

  return hit;
}

static inline int hit_check_player_slope_f(const u32 btns, int x, int y) {
  int hit = 0;

  hit |= 0x20000;

  if (player.x < (x * 0x10 + 8) * 0x200 && player.x > (x * 0x10 - 8) * 0x200 &&
      player.y + player.hit.bottom > (y * 0x10 * 0x200) + ((player.x - x * 0x10 * 0x200) / 2) + 0x800 &&
      player.y - player.hit.top < (y * 0x10 + 8) * 0x200) {
    // clip
    player.y = (y * 0x10 * 0x200) + ((player.x - x * 0x10 * 0x200) / 2) +
               0x800 - player.hit.bottom;

    // halt momentum
    if (player.yvel > 0x400) snd_play_sound(CHAN_STEP, 23, SOUND_MODE_PLAY);
    if (player.yvel > 0) player.yvel = 0;

    // set that hit this slope
    hit |= 0x28;
  }

  return hit;
}

static inline int hit_check_player_slope_g(const u32 btns, int x, int y) {
  int hit = 0;

  hit |= 0x40000;

  if (player.x < (x * 0x10 + 8) * 0x200 && player.x > (x * 0x10 - 8) * 0x200 &&
      player.y + player.hit.bottom > (y * 0x10 * 0x200) - ((player.x - x * 0x10 * 0x200) / 2) + 0x800 &&
      player.y - player.hit.top < (y * 0x10 + 8) * 0x200) {
    // clip
    player.y = (y * 0x10 * 0x200) - ((player.x - x * 0x10 * 0x200) / 2) +
               0x800 - player.hit.bottom;

    // halt momentum
    if (player.yvel > 0x400) snd_play_sound(CHAN_STEP, 23, SOUND_MODE_PLAY);
    if (player.yvel > 0) player.yvel = 0;

    // set that hit this slope
    hit |= 0x18;
  }

  return hit;
}

static inline int hit_check_player_slope_h(const u32 btns, int x, int y) {
  int hit = 0;

  hit |= 0x80000;

  if (player.x < (x * 0x10 + 8) * 0x200 && player.x > (x * 0x10 - 8) * 0x200 &&
      player.y + player.hit.bottom > (y * 0x10 * 0x200) - ((player.x - x * 0x10 * 0x200) / 2) - 0x800 &&
      player.y - player.hit.top < (y * 0x10 + 8) * 0x200) {
    // clip
    player.y = (y * 0x10 * 0x200) - ((player.x - x * 0x10 * 0x200) / 2) -
               0x800 - player.hit.bottom;

    // halt momentum
    if (player.yvel > 0x400) snd_play_sound(CHAN_STEP, 23, SOUND_MODE_PLAY);
    if (player.yvel > 0) player.yvel = 0;

    // set that hit this slope
    hit |= 0x18;
  }

  return hit;
}

static inline int hit_check_player_water(const u32 btns, int x, int y) {
  int hit = 0;

  if (player.x - player.hit.back < (x * 0x10 + 5) * 0x200 &&
      player.x + player.hit.back > ((x * 0x10 - 5) * 0x200) &&
      player.y - player.hit.top < ((y * 0x10 + 5) * 0x200) &&
      player.y + player.hit.bottom > y * 0x10 * 0x200)
    hit |= 0x100;

  return hit;
}

static inline int hit_check_player_damage(const u32 btns, int x, int y) {
  int hit = 0;

  if (player.x - 0x800 < (x * 0x10 + 4) * 0x200 &&
      player.x + 0x800 > (x * 0x10 - 4) * 0x200 &&
      player.y - 0x800 < (y * 0x10 + 3) * 0x200 &&
      player.y + 0x800 > (y * 0x10 - 3) * 0x200)
    hit |= 0x400;

  return hit;
}

static inline int hit_check_player_damage_w(const u32 btns, int x, int y) {
  int hit = 0;

  if (player.x - 0x800 < (x * 0x10 + 4) * 0x200 &&
      player.x + 0x800 > (x * 0x10 - 4) * 0x200 &&
      player.y - 0x800 < (y * 0x10 + 3) * 0x200 &&
      player.y + 0x800 > (y * 0x10 - 3) * 0x200)
    hit |= 0xD00;

  return hit;
}

static inline int hit_check_player_wind_left(const u32 btns, int x, int y) {
  int hit = 0;
  if (player.x - player.hit.back < (x * 0x10 + 6) * 0x200 &&
      player.x + player.hit.back > (x * 0x10 - 6) * 0x200 &&
      player.y - player.hit.top < (y * 0x10 + 6) * 0x200 &&
      player.y + player.hit.bottom > (y * 0x10 - 6) * 0x200)
    hit |= 0x1000;

  return hit;
}

static inline int hit_check_player_wind_up(const u32 btns, int x, int y) {
  int hit = 0;
  if (player.x - player.hit.back < (x * 0x10 + 6) * 0x200 &&
      player.x + player.hit.back > (x * 0x10 - 6) * 0x200 &&
      player.y - player.hit.top < (y * 0x10 + 6) * 0x200 &&
      player.y + player.hit.bottom > (y * 0x10 - 6) * 0x200)
    hit |= 0x2000;

  return hit;
}

static inline int hit_check_player_wind_right(const u32 btns, int x, int y) {
  int hit = 0;
  if (player.x - player.hit.back < (x * 0x10 + 6) * 0x200 &&
      player.x + player.hit.back > (x * 0x10 - 6) * 0x200 &&
      player.y - player.hit.top < (y * 0x10 + 6) * 0x200 &&
      player.y + player.hit.bottom > (y * 0x10 - 6) * 0x200)
    hit |= 0x4000;

  return hit;
}

static inline int hit_check_player_wind_down(const u32 btns, int x, int y) {
  int hit = 0;
  if (player.x - player.hit.back < (x * 0x10 + 6) * 0x200 &&
      player.x + player.hit.back > (x * 0x10 - 6) * 0x200 &&
      player.y - player.hit.top < (y * 0x10 + 6) * 0x200 &&
      player.y + player.hit.bottom > (y * 0x10 - 6) * 0x200)
    hit |= 0x8000;

  return hit;
}

static inline void hit_player_map(const u32 btns) {
  static const int ofsx[4] = { 0, 1, 0, 1 };
  static const int ofsy[4] = { 0, 0, 1, 1 };

  const int px = TO_INT(player.x / TILE_SIZE);
  const int py = TO_INT(player.y / TILE_SIZE);

  for (int i = 0; i < 4; ++i) {
    const int x = px + ofsx[i];
    const int y = py + ofsy[i];
    const u8 atrb = stage_get_atrb(x, y);

    switch (atrb) {
      // blocks
      case 0x05:
      case 0x41:
      case 0x43:
      case 0x46:
        player.flags |= hit_check_player_block(btns, x, y);
        break;

      // slopes
      case 0x50:
        player.flags |= hit_check_player_slope_a(btns, x, y);
        break;

      case 0x51:
        player.flags |= hit_check_player_slope_b(btns, x, y);
        break;

      case 0x52:
        player.flags |= hit_check_player_slope_c(btns, x, y);
        break;

      case 0x53:
        player.flags |= hit_check_player_slope_d(btns, x, y);
        break;

      case 0x54:
        player.flags |= hit_check_player_slope_e(btns, x, y);
        break;

      case 0x55:
        player.flags |= hit_check_player_slope_f(btns, x, y);
        break;

      case 0x56:
        player.flags |= hit_check_player_slope_g(btns, x, y);
        break;

      case 0x57:
        player.flags |= hit_check_player_slope_h(btns, x, y);
        break;

      // spikes
      case 0x42:
        player.flags |= hit_check_player_damage(btns, x, y);
        break;

      // water spikes
      case 0x62:
        player.flags |= hit_check_player_damage_w(btns, x, y);
        break;

      // wind
      case 0x80:
        player.flags |= hit_check_player_wind_left(btns, x, y);
        break;

      case 0x81:
        player.flags |= hit_check_player_wind_up(btns, x, y);
        break;

      case 0x82:
        player.flags |= hit_check_player_wind_right(btns, x, y);
        break;

      case 0x83:
        player.flags |= hit_check_player_wind_down(btns, x, y);
        break;

      // water
      case 0x02:
        player.flags |= hit_check_player_water(btns, x, y);
        break;

      // water and water blocks (same as the previous case)
      case 0x60:
        player.flags |= hit_check_player_water(btns, x, y);
        break;

      case 0x61:
        player.flags |= hit_check_player_block(btns, x, y);
        player.flags |= hit_check_player_water(btns, x, y);
        break;

      // water slopes
      case 0x70:
        player.flags |= hit_check_player_slope_a(btns, x, y);
        player.flags |= hit_check_player_water(btns, x, y);
        break;

      case 0x71:
        player.flags |= hit_check_player_slope_b(btns, x, y);
        player.flags |= hit_check_player_water(btns, x, y);
        break;

      case 0x72:
        player.flags |= hit_check_player_slope_c(btns, x, y);
        player.flags |= hit_check_player_water(btns, x, y);
        break;

      case 0x73:
        player.flags |= hit_check_player_slope_d(btns, x, y);
        player.flags |= hit_check_player_water(btns, x, y);
        break;

      case 0x74:
        player.flags |= hit_check_player_slope_e(btns, x, y);
        player.flags |= hit_check_player_water(btns, x, y);
        break;

      case 0x75:
        player.flags |= hit_check_player_slope_f(btns, x, y);
        player.flags |= hit_check_player_water(btns, x, y);
        break;

      case 0x76:
        player.flags |= hit_check_player_slope_g(btns, x, y);
        player.flags |= hit_check_player_water(btns, x, y);
        break;

      case 0x77:
        player.flags |= hit_check_player_slope_h(btns, x, y);
        player.flags |= hit_check_player_water(btns, x, y);
        break;

      // water current
      case 0xA0:
        player.flags |= hit_check_player_wind_left(btns, x, y);
        player.flags |= hit_check_player_water(btns, x, y);
        break;

      case 0xA1:
        player.flags |= hit_check_player_wind_up(btns, x, y);
        player.flags |= hit_check_player_water(btns, x, y);
        break;

      case 0xA2:
        player.flags |= hit_check_player_wind_right(btns, x, y);
        player.flags |= hit_check_player_water(btns, x, y);
        break;

      case 0xA3:
        player.flags |= hit_check_player_wind_down(btns, x, y);
        player.flags |= hit_check_player_water(btns, x, y);
        break;
    }
  }

  if (player.y > stage_water_y + TO_FIX(4))
    player.flags |= 0x100;
}

static inline int hit_check_player_npc_soft(const npc_t *npc) {
  int hit = 0;

  if (player.y - player.hit.top < npc->y + npc->hit.bottom - (3 * 0x200)
    && player.y + player.hit.bottom > npc->y - npc->hit.top + (3 * 0x200)
    && player.x - player.hit.back < npc->x + npc->hit.back
    && player.x - player.hit.back > npc->x) {
    if (player.xvel < 0x200)
      player.xvel += 0x200;
    hit |= 1;
  }

  if (player.y - player.hit.top < npc->y + npc->hit.bottom - (3 * 0x200)
    && player.y + player.hit.bottom > npc->y - npc->hit.top + (3 * 0x200)
    && player.x + player.hit.back - 0x200 > npc->x - npc->hit.back
    && player.x + player.hit.back - 0x200 < npc->x) {
    if (player.xvel > -0x200)
      player.xvel -= 0x200;
    hit |= 4;
  }

  if (player.x - player.hit.back < npc->x + npc->hit.back - (3 * 0x200)
    && player.x + player.hit.back > npc->x - npc->hit.back + (3 * 0x200)
    && player.y - player.hit.top < npc->y + npc->hit.bottom
    && player.y - player.hit.top > npc->y) {
    if (player.yvel < 0)
      player.yvel = 0;
    hit |= 2;
  }

  if (player.x - player.hit.back < npc->x + npc->hit.back - (3 * 0x200)
    && player.x + player.hit.back > npc->x - npc->hit.back + (3 * 0x200)
    && player.y + player.hit.bottom > npc->y - npc->hit.top
    && player.hit.bottom + player.y < npc->y + (3 * 0x200)) {
    if (npc->bits & NPC_BOUNCY) {
      player.yvel = npc->yvel - 0x200;
      hit |= 8;
    } else if (!(player.flags & 8) && player.yvel > npc->yvel) {
      player.y = npc->y - npc->hit.top - player.hit.bottom + 0x200;
      player.yvel = npc->yvel;
      player.x += npc->xvel;
      hit |= 8;
    }
  }

  return hit;
}

static inline u8 hit_check_player_npc_normal(const npc_t *npc) {
  if (npc->dir == 0) {
    if (player.x + (2 * 0x200) > npc->x - npc->hit.front
      && player.x - (2 * 0x200) < npc->x + npc->hit.back
      && player.y + (2 * 0x200) > npc->y - npc->hit.top
      && player.y - (2 * 0x200) < npc->y + npc->hit.bottom)
      return 1;
  } else {
    if (player.x + (2 * 0x200) > npc->x - npc->hit.back
      && player.x - (2 * 0x200) < npc->x + npc->hit.front
      && player.y + (2 * 0x200) > npc->y - npc->hit.top
      && player.y - (2 * 0x200) < npc->y + npc->hit.bottom)
      return 1;
  }

  return 0;
}

static inline int hit_check_player_npc_solid(const npc_t *npc) {
  // TODO: comment this
  int hit = 0;

  // this used to use floats; replaced it with fixed division
  int fy1, fx1;
  int fx2, fy2;

  if (npc->x > player.x)
    fx1 = (npc->x - player.x);
  else
    fx1 = (player.x - npc->x);

  if (npc->y > player.y)
    fy1 = (npc->y - player.y);
  else
    fy1 = (player.y - npc->y);

  fx2 = npc->hit.back;
  fy2 = npc->hit.top;

  if (fx1 == 0) fx1 = FIX_SCALE;
  if (fx2 == 0) fx2 = FIX_SCALE;

  if ((fy1 * FIX_SCALE) / fx1 > (fy2 * FIX_SCALE) / fx2) {
    if (player.x - player.hit.back < npc->x + npc->hit.back && player.x + player.hit.back > npc->x - npc->hit.back) {
      if (player.y - player.hit.top < npc->y + npc->hit.bottom && player.y - player.hit.top > npc->y) {
        if (player.yvel < npc->yvel) {
          player.y = npc->y + npc->hit.bottom + player.hit.top + FIX_SCALE;
          player.yvel = npc->yvel;
        } else {
          if (player.yvel < 0) player.yvel = 0;
        }

        hit |= 2;
      }

      if (player.y + player.hit.bottom > npc->y - npc->hit.top && player.hit.bottom + player.y < npc->y + TO_FIX(3)) {
        if (player.yvel - npc->yvel > TO_FIX(2))
          snd_play_sound(-1, 23, SOUND_MODE_PLAY);

        if (player.unit == 1) {
          player.y = npc->y - npc->hit.top - player.hit.bottom + FIX_SCALE;
          hit |= 8;
        } else if (npc->bits & NPC_BOUNCY) {
          player.yvel = npc->yvel - FIX_SCALE;
          hit |= 8;
        } else if (!(player.flags & 8) && player.yvel > npc->yvel) {
          player.y = npc->y - npc->hit.top - player.hit.bottom + FIX_SCALE;
          player.yvel = npc->yvel;
          player.x += npc->xvel;
          hit |= 8;
        }
      }
    }
  } else {
    if (player.y - player.hit.top < npc->y + npc->hit.bottom && player.y + player.hit.bottom > npc->y - npc->hit.top) {
      if (player.x - player.hit.back < npc->x + npc->hit.back && player.x - player.hit.back > npc->x) {
        if (player.xvel < npc->xvel) player.xvel = npc->xvel;

        player.x = npc->x + npc->hit.back + player.hit.back;

        hit |= 1;
      }

      if (player.x + player.hit.back > npc->x - npc->hit.back && player.hit.back + player.x < npc->x) {
        if (player.xvel > npc->xvel) player.xvel = npc->xvel;

        player.x = npc->x - npc->hit.back - player.hit.back;

        hit |= 4;
      }
    }
  }

  return hit;
}

static inline void hit_player_npc(void) {
  int hit = 0;

  if (!(player.cond & PLRCOND_ALIVE) || player.cond & PLRCOND_INVISIBLE)
    return;

  for (int i = 0; i <= npc_list_max; ++i) {
    npc_t *npc = &npc_list[i];

    if (!(npc->cond & NPCCOND_ALIVE))
      continue;

    hit = 0;

    if (npc->bits & NPC_SOLID_SOFT) {
      hit = hit_check_player_npc_soft(npc);
      player.flags |= hit;
    } else if (npc->bits & NPC_SOLID_HARD) {
      hit = hit_check_player_npc_solid(npc);
      player.flags |= hit;
    } else {
      hit = hit_check_player_npc_normal(npc);
    }

    // Special NPCs (pickups)
    if (hit != 0 && npc->class_num == 1) {
      snd_play_sound(CHAN_ITEM, 14, SOUND_MODE_PLAY);
      plr_add_exp(npc->exp);
      npc->cond = 0;
    }

    if (hit != 0 && npc->class_num == 86) {
      snd_play_sound(CHAN_ITEM, 42, SOUND_MODE_PLAY);
      plr_add_ammo(npc->event_num, npc->exp);
      npc->cond = 0;
    }

    if (hit != 0 && npc->class_num == 87) {
      snd_play_sound(CHAN_ITEM, 20, SOUND_MODE_PLAY);
      plr_add_life(npc->exp);
      npc->cond = 0;
    }

    // Run event on contact
    // if (!(game_flags & 4) && hit != 0 && npc->bits & NPC_EVENT_WHEN_TOUCHED)
    //   StartTextScript(npc->event_num);

    // NPC damage
    if (game_flags & GFLAG_INPUT_ENABLED && !(npc->bits & NPC_INTERACTABLE)) {
      if (npc->bits & NPC_REAR_AND_TOP_DONT_HURT) {
        if (hit & 4 && npc->xvel < 0) plr_damage(npc->damage);
        if (hit & 1 && npc->xvel > 0) plr_damage(npc->damage);
        if (hit & 8 && npc->yvel < 0) plr_damage(npc->damage);
        if (hit & 2 && npc->yvel > 0) plr_damage(npc->damage);
      } else if (hit != 0 && npc->damage && !(game_flags & 4)) {
        plr_damage(npc->damage);
      }
    }

    // Interaction
    if (!(game_flags & 4) && hit != 0 && player.cond & 1 && npc->bits & NPC_INTERACTABLE) {
      // StartTextScript(npc->event_num);
      player.xvel = 0;
      player.question = FALSE;
    }
  }

  // Create question mark when NPC hasn't been interacted with
  // if (player.question)
  //   SetCaret(player.x, player.y, CARET_QUESTION_MARK, DIR_LEFT);
}

void hit_player(const u32 btns) {
  player.flags = 0;
  hit_player_map(btns);
  hit_player_npc();
}
