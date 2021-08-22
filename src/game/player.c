#include <string.h>

#include "engine/common.h"
#include "engine/math.h"
#include "engine/graphics.h"
#include "engine/sound.h"
#include "engine/input.h"

#include "game/player.h"
#include "game/npc.h"
#include "game/tsc.h"
#include "game/game.h"
#include "game/caret.h"
#include "game/dmgnum.h"

// also known as MyChar.cpp
// most of the animation and physics code was left intact

#define ARMS_FRAME_W   24
#define ARMS_FRAME_H   16
#define ARMS_IMG_W     320
#define ARMS_IMG_H     240
#define ARMS_IMG_ROWS  (ARMS_IMG_W / ARMS_FRAME_W)
#define ARMS_IMG_COL_H (6 * ARMS_FRAME_H)

#define CHAR_FRAME_W 16
#define CHAR_FRAME_H 16

player_t player;

static gfx_texrect_t rc_left[] = {
  {{ 0, 0, 16, 16 }},
  {{ 16, 0, 32, 16 }},
  {{ 0, 0, 16, 16 }},
  {{ 32, 0, 48, 16 }},
  {{ 0, 0, 16, 16 }},
  {{ 48, 0, 64, 16 }},
  {{ 64, 0, 80, 16 }},
  {{ 48, 0, 64, 16 }},
  {{ 80, 0, 96, 16 }},
  {{ 48, 0, 64, 16 }},
  {{ 96, 0, 112, 16 }},
  {{ 112, 0, 128, 16 }},
};

static gfx_texrect_t rc_right[] = {
  {{ 0, 16, 16, 32 }},
  {{ 16, 16, 32, 32 }},
  {{ 0, 16, 16, 32 }},
  {{ 32, 16, 48, 32 }},
  {{ 0, 16, 16, 32 }},
  {{ 48, 16, 64, 32 }},
  {{ 64, 16, 80, 32 }},
  {{ 48, 16, 64, 32 }},
  {{ 80, 16, 96, 32 }},
  {{ 48, 16, 64, 32 }},
  {{ 96, 16, 112, 32 }},
  {{ 112, 16, 128, 32 }},
};

static gfx_texrect_t rc_bubble[] = {
  {{ 56, 96, 80, 120 }},
  {{ 80, 96, 104, 120 }},
};

static gfx_texrect_t rc_arms[PLR_MAX_ARMS];

static const struct phystab {
  s16 max_dash;
  s16 max_move;
  s16 gravity1;
  s16 gravity2;
  s16 jump;
  s16 dash1;
  s16 dash2;
  s16 resist;
} phystab[2] = {
  { 0x32C,     0x5FF,     0x50,     0x20,     0x500,     0x200 / 6,     0x200 / 16,     0x200 / 10     },
  { 0x32C / 2, 0x5FF / 2, 0x50 / 2, 0x20 / 2, 0x500 / 2, 0x200 / 6 / 2, 0x200 / 16 / 2, 0x200 / 10 / 2 },
};

void plr_init(void) {
  // init player struct

  plr_reset();

  // init texrects; player surfaces should already be loaded by now

  for (u32 i = 0; i < sizeof(rc_left) / sizeof(*rc_left); ++i) {
    gfx_set_texrect(&rc_left[i], SURFACE_ID_MY_CHAR);
    gfx_set_texrect(&rc_right[i], SURFACE_ID_MY_CHAR);
  }

  for (u32 i = 0; i < PLR_MAX_ARMS; ++i) {
    rc_arms[i].r.left = (i % ARMS_IMG_ROWS) * ARMS_FRAME_W;
    rc_arms[i].r.right = rc_arms[i].r.left + ARMS_FRAME_W;
    rc_arms[i].r.top = (i / ARMS_IMG_ROWS) * ARMS_IMG_COL_H;
    rc_arms[i].r.bottom = rc_arms[i].r.top + ARMS_FRAME_H;
    gfx_set_texrect(&rc_arms[i], SURFACE_ID_ARMS);
  }

  gfx_set_texrect(&rc_bubble[0], SURFACE_ID_CARET);
  gfx_set_texrect(&rc_bubble[1], SURFACE_ID_CARET);
}

void plr_reset(void) {
  memset(&player, 0, sizeof(player));

  player.cond = PLRCOND_ALIVE;
  player.dir = DIR_RIGHT;

  player.view.back = TO_FIX(8);
  player.view.top = TO_FIX(8);
  player.view.front = TO_FIX(8);
  player.view.bottom = TO_FIX(8);

  player.hit.back = TO_FIX(5);
  player.hit.top = TO_FIX(8);
  player.hit.front = TO_FIX(5);
  player.hit.bottom = TO_FIX(8);

  player.life = 3;
  player.life_bar = 3;
  player.max_life = 3;

  player.arm = 0;
  player.arms_x = 16;
}

void plr_animate(const bool input_enabled) {
  if (player.cond & PLRCOND_INVISIBLE)
    return;

  if (player.flags & 0x08) {
    if (player.cond & PLRCOND_USE_BUTTON) {
      player.anim = 11;
    } else if (input_held & IN_UP && input_held & (IN_LEFT | IN_RIGHT) && input_enabled) {
      player.cond |= PLRCOND_WALKING;

      if (++player.anim_wait > 4) {
        player.anim_wait = 0;
        if (++player.anim == 7 || player.anim == 9)
          snd_play_sound(PRIO_LOW, 24, FALSE);
      }

      if (player.anim > 9 || player.anim < 6)
        player.anim = 6;
    } else if (input_held & (IN_LEFT | IN_RIGHT) && input_enabled) {
      player.cond |= PLRCOND_WALKING;

      if (++player.anim_wait > 4) {
        player.anim_wait = 0;
        if (++player.anim == 2 || player.anim == 4)
          snd_play_sound(PRIO_LOW, 24, FALSE);
      }

      if (player.anim > 4 || player.anim < 1)
        player.anim = 1;
    } else if (input_held & IN_UP && input_enabled) {
      if (player.cond & PLRCOND_WALKING)
        snd_play_sound(PRIO_LOW, 24, FALSE);

      player.cond &= ~PLRCOND_WALKING;
      player.anim = 5;
    } else {
      if (player.cond & PLRCOND_WALKING)
        snd_play_sound(PRIO_LOW, 24, FALSE);

      player.cond &= ~PLRCOND_WALKING;
      player.anim = 0;
    }
  } else if (player.up) {
    player.anim = 6;
  } else if (player.down) {
    player.anim = 10;
  } else if (player.yvel > 0) {
    player.anim = 1;
  } else {
    player.anim = 3;
  }

  if (player.dir == DIR_LEFT)
    player.rect = rc_left[player.anim];
  else
    player.rect = rc_right[player.anim];
}

static inline void plr_draw_arm(int plr_vx, int plr_vy, int cam_vx, int cam_vy) {
  int arm_ofs_y = 0;

  player.rect_arms = rc_arms[player.arm];

  if (player.dir == DIR_RIGHT)
    player.rect_arms.v += ARMS_FRAME_H;

  if (player.up) {
    arm_ofs_y = -4;
    player.rect_arms.v += ARMS_FRAME_H * 2;
  } else if (player.down) {
    arm_ofs_y = 4;
    player.rect_arms.v += ARMS_FRAME_H * 4;
  } else {
    arm_ofs_y = 0;
  }

  if (player.anim == 1 || player.anim == 3 || player.anim == 6 || player.anim == 8)
    ++player.rect_arms.v;

  int tx = plr_vx - cam_vx;
  int ty = plr_vy - cam_vy + arm_ofs_y;
  if (player.dir == DIR_LEFT) tx -= 8;
  gfx_draw_texrect(&player.rect_arms, GFX_LAYER_BACK, tx, ty);
}

static inline void plr_draw_char(int plr_vx, int plr_vy, int cam_vx, int cam_vy) {
  gfx_texrect_t rect = player.rect;
  if (player.equip & EQUIP_MIMIGA_MASK)
    rect.r.y += CHAR_FRAME_H * 2;
  gfx_draw_texrect_16x16(&rect, GFX_LAYER_BACK, plr_vx - cam_vx, plr_vy - cam_vy);
}

static inline void plr_draw_bubble(int plr_vx, int plr_vy, int cam_vx, int cam_vy) {
  ++player.bubble;
  if ((player.equip & EQUIP_AIR_TANK) || player.unit == 1)
    gfx_draw_texrect(&rc_bubble[player.bubble / 2 % 2], GFX_LAYER_BACK, plr_vx - 12 - cam_vx, plr_vy - 12 - cam_vy);
}

void plr_draw(int cam_x, int cam_y) {
  if (!(player.cond & PLRCOND_ALIVE) || player.cond & PLRCOND_INVISIBLE)
    return;

  const int plr_vx = TO_INT(player.x - player.view.front);
  const int plr_vy = TO_INT(player.y - player.view.top);
  const int cam_vx = TO_INT(cam_x);
  const int cam_vy = TO_INT(cam_y);

  plr_draw_arm(plr_vx, plr_vy, cam_vx, cam_vy);

  if ((player.shock >> 1) & 1)
    return;

  plr_draw_char(plr_vx, plr_vy, cam_vx, cam_vy);
  plr_draw_bubble(plr_vx, plr_vy, cam_vx, cam_vy);
}

void plr_set_pos(int x, int y) {
  player.x = x;
  player.y = y;
  player.xvel = 0;
  player.yvel = 0;
  player.tgt_x = x;
  player.tgt_y = y;
  player.index_x = 0;
  player.index_y = 0;
  player.cond &= ~PLRCOND_USE_BUTTON;
}

void plr_jump_back(int from) {
  player.cond &= ~PLRCOND_USE_BUTTON;
  player.yvel = -FIX_SCALE;
  if (from == DIR_LEFT) {
    player.dir = DIR_LEFT;
    player.xvel = FIX_SCALE;
  } else if (from == DIR_RIGHT) {
    player.dir = DIR_RIGHT;
    player.xvel = -FIX_SCALE;
  } else {
    npc_t *npc = npc_find_by_event_num(from);
    if (npc) {
      if (player.x < npc->x) {
        player.dir = DIR_LEFT;
        player.xvel = FIX_SCALE;
      } else {
        player.dir = DIR_RIGHT;
        player.xvel = -FIX_SCALE;
      }
    }
  }
}

void plr_face_towards(int what) {
  if (what == 3) {
    player.cond |= PLRCOND_USE_BUTTON;
  } else {
    player.cond &= ~PLRCOND_USE_BUTTON;
    if (what < 10) {
      player.dir = what;
    } else {
      npc_t *npc = npc_find_by_event_num(what);
      if (!npc) return;
      player.dir = (player.x < npc->x) ? DIR_LEFT : DIR_RIGHT;
    }
  }
  player.xvel = 0;
  plr_animate(0);
}

static inline void plr_update_air(void) {
  if (player.equip & EQUIP_AIR_TANK) {
    player.air = 1000;
    player.air_count = 0;
    return;
  }

  if (!(player.flags & 0x100)) {
    player.air = 1000;
    if (player.air_count)
      --player.air_count;
  } else {
    player.air_count = 60;
    if (--player.air == 0) {
      if (npc_get_flag(4000)) {
        // core cutscene
        tsc_start_event(1100);
      } else {
        // drown
        tsc_start_event(41);
        caret_spawn(player.x, player.y, CARET_DROWNED_QUOTE, player.dir);
        player.cond &= ~PLRCOND_ALIVE;
      }
    }
  }
}

// jesus h christ
static void plr_act_normal(const bool input_enabled) {
  // Get speeds and accelerations
  const struct phystab *phys = &phystab[(player.flags & 0x100) != 0];
  int a, x;

  if (input_enabled && !(game_flags & 4))
    plr_update_air();

  if (player.cond & PLRCOND_INVISIBLE)
    return;

  // Don't create "?" effect
  player.question = FALSE;

  // If can't control player, stop boosting
  if (!input_enabled) player.boost_sw = 0;

  // Movement on the ground
  if (player.flags & 8 || player.flags & 0x10 || player.flags & 0x20) {
    // Stop boosting and refuel
    player.boost_sw = 0;

    if (player.equip & EQUIP_BOOSTER_0_8) {
      player.boost_cnt = 50;
    } else if (player.equip & EQUIP_BOOSTER_2_0) {
      player.boost_cnt = 50;
    } else {
      player.boost_cnt = 0;
    }

    // Move in direction held
    if (input_enabled) {
      if (input_trig == IN_DOWN && input_held == IN_DOWN && !(player.cond & PLRCOND_USE_BUTTON) && !(game_flags & 4)) {
        player.cond |= PLRCOND_USE_BUTTON;
        player.question = TRUE;
      } else if (input_held == IN_DOWN) {
        // There probably used to be commented-out code here
      } else {
        if (input_held & IN_LEFT && player.xvel > -phys->max_dash)
          player.xvel -= phys->dash1;
        if (input_held & IN_RIGHT && player.xvel < phys->max_dash)
          player.xvel += phys->dash1;
        if (input_held & IN_LEFT)
          player.dir = 0;
        if (input_held & IN_RIGHT)
          player.dir = 2;
      }
    }

    // Friction
    if (!(player.cond & PLRCOND_IN_WIND)) {
      if (player.xvel < 0) {
        if (player.xvel > -phys->resist)
          player.xvel = 0;
        else
          player.xvel += phys->resist;
      }
      if (player.xvel > 0) {
        if (player.xvel < phys->resist)
          player.xvel = 0;
        else
          player.xvel -= phys->resist;
      }
    }
  } else {
    // Start boosting
    if (input_enabled) {
      if (player.equip & (EQUIP_BOOSTER_0_8 | EQUIP_BOOSTER_2_0) && input_trig & IN_JUMP && player.boost_cnt != 0) {
        // Booster 0.8
        if (player.equip & EQUIP_BOOSTER_0_8) {
          player.boost_sw = 1;
          if (player.yvel > 0x100)
            player.yvel /= 2;
        }

        // Booster 2.0
        if (player.equip & EQUIP_BOOSTER_2_0) {
          if (input_held & IN_UP) {
            player.boost_sw = 2;
            player.xvel = 0;
            player.yvel = -0x5FF;
          } else if (input_held & IN_LEFT) {
            player.boost_sw = 1;
            player.yvel = 0;
            player.xvel = -0x5FF;
          } else if (input_held & IN_RIGHT) {
            player.boost_sw = 1;
            player.yvel = 0;
            player.xvel = 0x5FF;
          } else if (input_held & IN_DOWN) {
            player.boost_sw = 3;
            player.xvel = 0;
            player.yvel = 0x5FF;
          } else {
            player.boost_sw = 2;
            player.xvel = 0;
            player.yvel = -0x5FF;
          }
        }
      }

      // Move left and right
      if (input_held & IN_LEFT && player.xvel > -phys->max_dash)
        player.xvel -= phys->dash2;
      if (input_held & IN_RIGHT && player.xvel < phys->max_dash)
        player.xvel += phys->dash2;

      if (input_held & IN_LEFT) player.dir = 0;
      if (input_held & IN_RIGHT) player.dir = 2;
    }

    // Slow down when stopped boosting (Booster 2.0)
    if (player.equip & EQUIP_BOOSTER_2_0 && player.boost_sw != 0 && (!(input_held & IN_JUMP) || player.boost_cnt == 0)) {
      if (player.boost_sw == 1)
        player.xvel /= 2;
      else if (player.boost_sw == 2)
        player.yvel /= 2;
    }

    // Stop boosting
    if (player.boost_cnt == 0 || !(input_held & IN_JUMP))
      player.boost_sw = 0;
  }

  // Jumping
  if (input_enabled) {
    // Look up and down
    if (input_held & IN_UP)
      player.up = TRUE;
    else
      player.up = FALSE;

    if (input_held & IN_DOWN && !(player.flags & 8))
      player.down = TRUE;
    else
      player.down = FALSE;

    if (input_trig & IN_JUMP && (player.flags & 8 || player.flags & 0x10 || player.flags & 0x20)) {
      if (player.flags & 0x2000) {
        // Another weird empty case needed for accurate assembly.
        // There probably used to be some commented-out code here.
      } else {
        player.yvel = -phys->jump;
        snd_play_sound(PRIO_LOW, 15, FALSE);
      }
    }
  }

  // Stop interacting when moved
  if (input_enabled && input_held & (IN_LEFT | IN_RIGHT | IN_UP | IN_JUMP | IN_FIRE))
    player.cond &= ~PLRCOND_USE_BUTTON;

  // Booster losing fuel
  if (player.boost_sw != 0 && player.boost_cnt != 0)
    --player.boost_cnt;

  // Wind / current forces
  if (player.flags & 0x1000) player.xvel -= 0x88;
  if (player.flags & 0x2000) player.yvel -= 0x80;
  if (player.flags & 0x4000) player.xvel += 0x88;
  if (player.flags & 0x8000) player.yvel += 0x55;

  // Booster 2.0 forces and effects
  if (player.equip & EQUIP_BOOSTER_2_0 && player.boost_sw != 0) {
    if (player.boost_sw == 1) {
      // Go up when going into a wall
      if (player.flags & 5) player.yvel = -0x100;

      // Move in direction facing
      if (player.dir == 0) player.xvel -= 0x20;
      if (player.dir == 2) player.xvel += 0x20;

      // Boost particles (and sound) 
      if (input_trig & IN_JUMP || player.boost_cnt % 3 == 1) {
        if (player.dir == 0)
          caret_spawn(player.x + (2 * 0x200), player.y + (2 * 0x200), CARET_EXHAUST, DIR_RIGHT);
        if (player.dir == 2)
          caret_spawn(player.x - (2 * 0x200), player.y + (2 * 0x200), CARET_EXHAUST, DIR_LEFT);
        snd_play_sound(PRIO_LOW, 113, FALSE);
      }
    } else if (player.boost_sw == 2) {
      // Move upwards
      player.yvel -= 0x20;

      // Boost particles (and sound)
      if (input_trig & IN_JUMP || player.boost_cnt % 3 == 1) {
        caret_spawn(player.x, player.y + (6 * 0x200), CARET_EXHAUST, DIR_DOWN);
        snd_play_sound(PRIO_LOW, 113, FALSE);
      }
    } else if (player.boost_sw == 3 && (input_trig & IN_JUMP || player.boost_cnt % 3 == 1)) {
      // Boost particles (and sound)
      caret_spawn(player.x, player.y - (6 * 0x200), CARET_EXHAUST, DIR_UP);
      snd_play_sound(PRIO_LOW, 113, FALSE);
    }
  } else if (player.flags & 0x2000) {
    // Upwards wind/current
    player.yvel += phys->gravity1;
  } else if (player.equip & EQUIP_BOOSTER_0_8 && player.boost_sw != 0 && player.yvel > -0x400) {
    // Booster 0.8
    // Upwards force
    player.yvel -= 0x20;

    if (player.boost_cnt % 3 == 0) {
      caret_spawn(player.x, player.y + (player.hit.bottom / 2), CARET_EXHAUST, DIR_DOWN);
      snd_play_sound(PRIO_LOW, 113, FALSE);
    }

    // Bounce off of ceiling
    if (player.flags & 2) player.yvel = 0x200;
  } else if (player.yvel < 0 && input_enabled && input_held & IN_JUMP) {
    // Gravity while jump is held
    player.yvel += phys->gravity2;
  } else {
    // Normal gravity
    player.yvel += phys->gravity1;
  }

  // Keep player on slopes
  if (!input_enabled || !(input_trig & IN_JUMP)) {
    if (player.flags & 0x10 && player.xvel < 0) player.yvel = -player.xvel;
    if (player.flags & 0x20 && player.xvel > 0) player.yvel = player.xvel;
    if (player.flags & 8 && player.flags & 0x80000 && player.xvel < 0) player.yvel = 0x400;
    if (player.flags & 8 && player.flags & 0x10000 && player.xvel > 0) player.yvel = 0x400;
    if (player.flags & 8 && player.flags & 0x20000 && player.flags & 0x40000)
      player.yvel = 0x400;
  }

  // Limit speed
  if (player.flags & 0x100 && !(player.flags & 0xF000)) {
    if (player.xvel < -0x2FF) player.xvel = -0x2FF;
    if (player.xvel > 0x2FF) player.xvel = 0x2FF;
    if (player.yvel < -0x2FF) player.yvel = -0x2FF;
    if (player.yvel > 0x2FF) player.yvel = 0x2FF;
  } else {
    if (player.xvel < -0x5FF) player.xvel = -0x5FF;
    if (player.xvel > 0x5FF) player.xvel = 0x5FF;
    if (player.yvel < -0x5FF) player.yvel = -0x5FF;
    if (player.yvel > 0x5FF) player.yvel = 0x5FF;
  }

  // Water splashing
  if (!player.splash && player.flags & 0x100) {
    int dir;

    if (player.flags & 0x800)
      dir = 2;
    else
      dir = 0;

    if (!(player.flags & 8) && player.yvel > 0x200) {
      for (a = 0; a < 8; ++a) {
        x = player.x + (m_rand(-8, 8) * 0x200);
        npc_spawn(73, x, player.y, player.xvel + m_rand(-0x200, 0x200), m_rand(-0x200, 0x80) - (player.yvel / 2), dir, NULL, 0);
      }
      snd_play_sound(PRIO_LOW, 56, FALSE);
    } else {
      if (player.xvel > 0x200 || player.xvel < -0x200) {
        for (a = 0; a < 8; ++a) {
          x = player.x + (m_rand(-8, 8) * 0x200);
          npc_spawn(73, x, player.y, player.xvel + m_rand(-0x200, 0x200), m_rand(-0x200, 0x80), dir, NULL, 0);
        }
        snd_play_sound(PRIO_LOW, 56, FALSE);
      }
    }

    player.splash = TRUE;
  }

  if (!(player.flags & 0x100))
    player.splash = FALSE;

  // Spike damage
  if (player.flags & 0x400)
    plr_damage(10);

  // Camera
  if (player.dir == 0) {
    player.index_x -= 0x200;
    if (player.index_x < -0x8000)
      player.index_x = -0x8000;
  } else {
    player.index_x += 0x200;
    if (player.index_x > 0x8000)
      player.index_x = 0x8000;
  }
  if (input_held & IN_UP && input_enabled) {
    player.index_y -= 0x200;
    if (player.index_y < -0x8000)
      player.index_y = -0x8000;
  } else if (input_held & IN_DOWN && input_enabled) {
    player.index_y += 0x200;
    if (player.index_y > 0x8000) 
      player.index_y = 0x8000;
  } else {
    if (player.index_y > 0x200)
      player.index_y -= 0x200;
    if (player.index_y < -0x200)
      player.index_y += 0x200;
  }

  player.tgt_x = player.x + player.index_x;
  player.tgt_y = player.y + player.index_y;

  // Change position
  if (!(player.xvel <= phys->resist && player.xvel >= -phys->resist))
    player.x += player.xvel;

  player.y += player.yvel;
}

static void plr_act_stream(const bool input_enabled) {
  player.up = FALSE;
  player.down = FALSE;

  if (input_enabled) {
    if (input_held & (IN_LEFT | IN_RIGHT)) {
      if (input_held & IN_LEFT) player.xvel -= 0x100;
      if (input_held & IN_RIGHT) player.xvel += 0x100;
    } else if (player.xvel < 0x80 && player.xvel > -0x80) {
      player.xvel = 0;
    } else if (player.xvel > 0) {
      player.xvel -= 0x80;
    } else if (player.xvel < 0) {
      player.xvel += 0x80;
    }
    if (input_held & (IN_UP | IN_DOWN)) {
      if (input_held & IN_UP) player.yvel -= 0x100;
      if (input_held & IN_DOWN) player.yvel += 0x100;
    } else if (player.yvel < 0x80 && player.yvel > -0x80) {
      player.yvel = 0;
    } else if (player.yvel > 0) {
      player.yvel -= 0x80;
    } else if (player.yvel < 0) {
      player.yvel += 0x80;
    }
  } else {
    if (player.xvel < 0x80 && player.xvel > -0x40)
      player.xvel = 0;
    else if (player.xvel > 0)
      player.xvel -= 0x80;
    else if (player.xvel < 0)
      player.xvel += 0x80;
    if (player.yvel < 0x80 && player.yvel > -0x40)
      player.yvel = 0;
    else if (player.yvel > 0)
      player.yvel -= 0x80;
    else if (player.yvel < 0)
      player.yvel += 0x80;
  }

  if (player.yvel < -0x200 && player.flags & 2)
    caret_spawn(player.x, player.y - player.hit.top, CARET_TINY_PARTICLES, DIR_OTHER);
  if (player.yvel > 0x200 && player.flags & 8)
    caret_spawn(player.x, player.y + player.hit.bottom, CARET_TINY_PARTICLES, DIR_OTHER);

  if (player.xvel > 0x400) player.xvel = 0x400;
  if (player.xvel < -0x400) player.xvel = -0x400;

  if (player.yvel > 0x400) player.yvel = 0x400;
  if (player.yvel < -0x400) player.yvel = -0x400;

  if ((input_held & (IN_LEFT | IN_UP)) == (IN_LEFT | IN_UP)) {
    if (player.xvel < -780) player.xvel = -780;
    if (player.yvel < -780) player.yvel = -780;
  }

  if ((input_held & (IN_RIGHT | IN_UP)) == (IN_RIGHT | IN_UP)) {
    if (player.xvel > 780) player.xvel = 780;
    if (player.yvel < -780) player.yvel = -780;
  }

  if ((input_held & (IN_LEFT | IN_DOWN)) == (IN_LEFT | IN_DOWN)) {
    if (player.xvel < -780) player.xvel = -780;
    if (player.yvel > 780) player.yvel = 780;
  }

  if ((input_held & (IN_RIGHT | IN_DOWN)) == (IN_RIGHT | IN_DOWN)) {
    if (player.xvel > 780) player.xvel = 780;
    if (player.yvel > 780) player.yvel = 780;
  }

  player.x += player.xvel;
  player.y += player.yvel;
}

void plr_act(const bool input_enabled) {
  if (!(player.cond & PLRCOND_ALIVE))
    return;

  if (player.exp_wait)
    --player.exp_wait;

  if (player.shock) {
    --player.shock;
  } else if (player.exp_count) {
    dmgnum_spawn(&player.x, &player.y, player.exp_count);
    player.exp_count = 0;
  }

  if (player.unit == 1)
    plr_act_stream(input_enabled);
  else
    plr_act_normal(input_enabled);

  player.cond &= ~PLRCOND_IN_WIND;
}

void plr_damage(int val) {
  if (!(game_flags & GFLAG_INPUT_ENABLED))
    return;

  if (player.shock)
    return;

  // Damage player
  snd_play_sound(PRIO_HIGH, 16, FALSE);
  player.cond &= ~PLRCOND_USE_BUTTON;
  player.shock = 128;

  if (player.unit != 1)
    player.yvel = -0x400;

  player.life -= (s16)val;

  // Lose a whimsical star
  if (player.equip & EQUIP_WHIMSICAL_STAR && player.star > 0)
    player.star = (s16)player.star - 1;

  // Lose experience
  if (player.equip & EQUIP_ARMS_BARRIER)
    player.arms[player.arm].exp -= val;
  else
    player.arms[player.arm].exp -= val * 2;

  while (player.arms[player.arm].exp < 0) {
    if (player.arms[player.arm].level > 1) {
      --player.arms[player.arm].level;

      const int lv = player.arms[player.arm].level - 1;
      player.arms[player.arm].exp = plr_arms_exptab[player.arm][lv] + player.arms[player.arm].exp;

      if (player.life > 0 && player.arm != 13)
        caret_spawn(player.x, player.y, CARET_LEVEL_UP, DIR_RIGHT);
    } else {
      player.arms[player.arm].exp = 0;
    }
  }

  // Tell player how much damage was taken
  dmgnum_spawn(&player.x, &player.y, -val);

  // Death
  if (player.life <= 0) {
    snd_play_sound(PRIO_HIGH, 17, FALSE);
    player.cond = 0;
    npc_spawn_death_fx(player.x, player.y, TO_FIX(10), 0x40, 0);
    tsc_start_event(40);
  }
}

void plr_add_life(int val) {
  player.life += val;
  if (player.life > player.max_life)
    player.life = player.max_life;
  player.life_bar = player.life;
}

void plr_add_max_life(int val) {
  player.max_life += val;
  if (player.max_life > PLR_MAX_LIFE)
    player.max_life = PLR_MAX_LIFE;
  plr_add_life(val);
}

void plr_add_exp(int val) {
  int lv = player.arms[player.arm].level - 1;

  player.arms[player.arm].exp += val;

  if (lv == 2) {
    if (player.arms[player.arm].exp >= plr_arms_exptab[player.arm][lv]) {
      player.arms[player.arm].exp = plr_arms_exptab[player.arm][lv];
      if (player.equip & EQUIP_WHIMSICAL_STAR) {
        if (player.star < 3)
          ++player.star;
      }
    }
  } else {
    for (; lv < 2; ++lv) {
      if (player.arms[player.arm].exp >= plr_arms_exptab[player.arm][lv]) {
        ++player.arms[player.arm].level;
        player.arms[player.arm].exp = 0;
        if (player.arm != 13) {
          snd_play_sound(PRIO_NORMAL, 27, FALSE);
          caret_spawn(player.x, player.y, CARET_LEVEL_UP, DIR_LEFT);
        }
      }
    }
  }

  if (player.arm != 13) {
    player.exp_count += val;
    player.exp_wait = 30;
  } else {
    player.exp_wait = 10;
  }
}

void plr_add_ammo(int arm, int val) {
  // if player has missile launcher, give missile launcher ammo;
  // otherwise give super missile launcher ammo (if player has one)
  int a;
  if (player.arms[5].owned)
    a = 5;
  else if (player.arms[10].owned)
    a = 10;
  else
    return;
  player.arms[a].ammo += val;
  if (player.arms[a].ammo > player.arms[a].max_ammo)
    player.arms[a].ammo = player.arms[a].max_ammo;
}

void plr_item_equip(const u32 item, const bool equip) {
  if (equip)
    player.equip |= item;
  else
    player.equip &= ~item;
}

void plr_item_give(const u32 item) {
  player.items[item] = TRUE;
}

void plr_item_take(const u32 item) {
  player.items[item] = FALSE;
}
