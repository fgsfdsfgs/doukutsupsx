#include "engine/common.h"
#include "engine/sound.h"
#include "engine/input.h"

#include "game/stage.h"
#include "game/game.h"
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

void hit_player(const u32 btns) {
  player.flags = 0;
  hit_player_map(btns);
}
