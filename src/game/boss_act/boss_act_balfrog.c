#include "game/boss_act/boss_act.h"

enum balfrog_sprite {
  BALFROG_SPRITE_NOTHING = 0,
  BALFROG_SPRITE_STANDING_STILL = 1,
  BALFROG_SPRITE_MOUTH_BARELY_OPEN_CROUCHING = 2,
  BALFROG_SPRITE_MOUTH_OPEN_CROUCHING = 3,
  BALFROG_SPRITE_MOUTH_OPEN_CROUCHING_FLASHING = 4,
  BALFROG_SPRITE_JUMPING = 5,
  BALFROG_SPRITE_BALROG_WHITE = 6,
  BALFROG_SPRITE_BALROG_CROUCHING = 7,
  BALFROG_SPRITE_BALROG_JUMPING = 8
};

enum balfrog_state {
  BALFROG_INITIALIZE = 0,
  BALFROG_START = 10,
  BALFROG_INITIALIZE_FLICKER = 20,
  BALFROG_FLICKER = 21,
  BALFROG_WAIT = 100,
  BALFROG_INITIALIZE_HOP_1 = 101,
  BALFROG_INITIALIZE_HOP_2 = 102,
  BALFROG_HOP = 103,
  BALFROG_MIDAIR = 104,
  BALFROG_INITIALIZE_LAND = 110,
  BALFROG_LAND = 111,
  BALFROG_INITIALIZE_SHOOT = 112,
  BALFROG_SHOOT = 113,
  BALFROG_AFTER_SHOOT_WAIT = 114,
  BALFROG_INITIALIZE_LEAP_1 = 120,
  BALFROG_INITIALIZE_LEAP_2 = 121,
  BALFROG_INITIALIZE_LEAP_3 = 122,
  BALFROG_LEAP = 123,
  BALFROG_LEAP_MIDAIR = 124,
  BALFROG_DIE = 130,
  BALFROG_DIE_FLASHING = 131,
  BALFROG_REVERT = 132,
  BALFROG_NOP_START = 140,
  BALFROG_NOP = 141,
  BALFROG_GO_INTO_CEILING = 142,
  BALFROG_GONE_INTO_CEILING = 143
};

static inline void boss_act_balfrog_sub01(void) {
  npc_t *boss;
  int minus;

  if (npc_boss[0].dir == DIR_LEFT)
    minus = 1;
  else
    minus = -1;

  boss = &npc_boss[1];

  switch (npc_boss[0].anim) {
    case BALFROG_SPRITE_NOTHING:
      boss->snd_hit = 52;
      boss->hit.front = 16 * 0x200;
      boss->hit.top = 16 * 0x200;
      boss->hit.back = 16 * 0x200;
      boss->hit.bottom = 16 * 0x200;
      boss->size = 3;
      boss->bits = NPC_INVULNERABLE;
      break;

    case BALFROG_SPRITE_STANDING_STILL:
      boss->x = npc_boss[0].x + -24 * 0x200 * minus;
      boss->y = npc_boss[0].y - 24 * 0x200;
      break;

    case BALFROG_SPRITE_MOUTH_BARELY_OPEN_CROUCHING:
      boss->x = npc_boss[0].x + -24 * 0x200 * minus;
      boss->y = npc_boss[0].y - 20 * 0x200;
      break;

    case BALFROG_SPRITE_MOUTH_OPEN_CROUCHING:
    case BALFROG_SPRITE_MOUTH_OPEN_CROUCHING_FLASHING:
      boss->x = npc_boss[0].x + -24 * 0x200 * minus;
      boss->y = npc_boss[0].y - 16 * 0x200;
      break;

    case BALFROG_SPRITE_JUMPING:
      boss->x = npc_boss[0].x + -24 * 0x200 * minus;
      boss->y = npc_boss[0].y - 43 * 0x200;
      break;
  }
}

static inline void boss_act_balfrog_sub02(void) {
  npc_t *boss = &npc_boss[2];

  switch (npc_boss[0].anim) {
    case BALFROG_SPRITE_NOTHING:
      boss->snd_hit = 52;
      boss->hit.front = 24 * 0x200;
      boss->hit.top = 16 * 0x200;
      boss->hit.back = 24 * 0x200;
      boss->hit.bottom = 16 * 0x200;
      boss->size = 3;
      boss->bits = NPC_INVULNERABLE;
      break;

    case BALFROG_SPRITE_STANDING_STILL:
    case BALFROG_SPRITE_MOUTH_BARELY_OPEN_CROUCHING:
    case BALFROG_SPRITE_MOUTH_OPEN_CROUCHING:
    case BALFROG_SPRITE_MOUTH_OPEN_CROUCHING_FLASHING:
    case BALFROG_SPRITE_JUMPING:
      boss->x = npc_boss[0].x;
      boss->y = npc_boss[0].y;
      break;
  }
}

void boss_act_balfrog(npc_t *root) {
  u8 deg;
  int xvel, yvel;
  int i;

  // Rects 1-4 are for when Balfrog is a frog, 5-8 for when he reverts into Balrog and goes into the ceiling
  static const rect_t rc_left[9] = {
    {0, 0, 0, 0},         // Nothing
    {0, 48, 80, 112},     // Balfrog standing still
    {0, 112, 80, 176},    // Balfrog with his mouth barely open, crouching
    {0, 176, 80, 240},    // Balfrog with his mouth open, crouching
    {160, 48, 240, 112},  // Balfrog with his mouth open, crouching, flashing
    {160, 112, 240, 200}, // Balfrog jumping
    {200, 0, 240, 24},    // Balrog completely white
    {80, 0, 120, 24},     // Balrog crouching
    {120, 0, 160, 24},    // Balrog jumping
  };

  // See above
  static const rect_t rc_right[9] = {
    {0, 0, 0, 0},
    {80, 48, 160, 112},
    {80, 112, 160, 176},
    {80, 176, 160, 240},
    {240, 48, 320, 112},
    {240, 112, 320, 200},
    {200, 24, 240, 48},
    {80, 24, 120, 48},
    {120, 24, 160, 48},
  };

  npc_t *boss = npc_boss;

  switch (boss->act) {
    case BALFROG_INITIALIZE:
      boss->x = 6 * (0x200 * 0x10);
      boss->y = 12 * (0x200 * 0x10) + 8 * 0x200;
      boss->dir = DIR_RIGHT;
      boss->view.front = 48 * 0x200;
      boss->view.top = 48 * 0x200;
      boss->view.back = 32 * 0x200;
      boss->view.bottom = 16 * 0x200;
      boss->snd_hit = 52;
      boss->hit.front = 24 * 0x200;
      boss->hit.top = 16 * 0x200;
      boss->hit.back = 24 * 0x200;
      boss->hit.bottom = 16 * 0x200;
      boss->size = 3;
      boss->exp = 1;
      boss->event_num = 1000;
      boss->bits |= (NPC_EVENT_WHEN_KILLED | NPC_SHOW_DAMAGE);
      boss->life = 300;
      npc_boss_max = 2; // 0, 1, 2
      npc_boss[0].surf = SURFACE_ID_LEVEL_SPRITESET_2;
      npc_boss[1].surf = SURFACE_ID_LEVEL_SPRITESET_2;
      npc_boss[2].surf = SURFACE_ID_LEVEL_SPRITESET_2;
      break;

    case BALFROG_START:
      boss->act = (BALFROG_START + 1);
      boss->anim = BALFROG_SPRITE_MOUTH_OPEN_CROUCHING;
      boss->cond = NPCCOND_ALIVE;
      boss->rect = &rc_right[0];

      npc_boss[1].cond = (NPCCOND_ALIVE | NPCCOND_DAMAGE_BOSS);
      npc_boss[1].event_num = 1000;
      npc_boss[2].cond = NPCCOND_ALIVE;

      npc_boss[1].damage = 5;
      npc_boss[2].damage = 5;

      for (i = 0; i < 8; ++i)
        npc_spawn(NPC_SMOKE, boss->x + m_rand(-12, 12) * 0x200, boss->y + m_rand(-12, 12) * 0x200, m_rand(-341, 341),
          m_rand(-3 * 0x200, 0), DIR_LEFT, NULL, 0x100);

      break;

    case BALFROG_INITIALIZE_FLICKER:
      boss->act = BALFROG_FLICKER;
      boss->act_wait = 0;
      // Fallthrough
    case BALFROG_FLICKER:
      ++boss->act_wait;

      if (boss->act_wait / 2 % 2)
        boss->anim = BALFROG_SPRITE_MOUTH_OPEN_CROUCHING;
      else
        boss->anim = BALFROG_SPRITE_NOTHING;

      break;

    case BALFROG_WAIT:
      boss->act = BALFROG_INITIALIZE_HOP_1;
      boss->act_wait = 0;
      boss->anim = BALFROG_SPRITE_STANDING_STILL;
      boss->xvel = 0;
      // Fallthrough
    case BALFROG_INITIALIZE_HOP_1:
      ++boss->act_wait;

      if (boss->act_wait > 50) {
        boss->act = BALFROG_INITIALIZE_HOP_2;
        boss->anim_wait = 0;
        boss->anim = BALFROG_SPRITE_MOUTH_BARELY_OPEN_CROUCHING;
      }

      break;

    case BALFROG_INITIALIZE_HOP_2:
      ++boss->anim_wait;

      if (boss->anim_wait > 10) {
        boss->act = BALFROG_HOP;
        boss->anim_wait = 0;
        boss->anim = BALFROG_SPRITE_STANDING_STILL;
      }

      break;

    case BALFROG_HOP:
      ++boss->anim_wait;

      if (boss->anim_wait > 4) {
        boss->act = BALFROG_MIDAIR;
        boss->anim = BALFROG_SPRITE_JUMPING;
        boss->yvel = -2 * 0x200;
        snd_play_sound(PRIO_NORMAL, 25, FALSE);

        if (boss->dir == DIR_LEFT)
          boss->xvel = -1 * 0x200;
        else
          boss->xvel = 1 * 0x200;

        boss->view.top = 64 * 0x200;
        boss->view.bottom = 24 * 0x200;
      }

      break;

    case BALFROG_MIDAIR:
      if (boss->dir == DIR_LEFT && boss->flags & COLL_LEFT_WALL) {
        boss->dir = DIR_RIGHT;
        boss->xvel = 1 * 0x200;
      }

      if (boss->dir == DIR_RIGHT && boss->flags & COLL_RIGHT_WALL) {
        boss->dir = DIR_LEFT;
        boss->xvel = -1 * 0x200;
      }

      if (boss->flags & COLL_GROUND) {
        snd_play_sound(PRIO_NORMAL, 26, FALSE);
        cam_start_quake_small(30);
        boss->act = BALFROG_WAIT;
        boss->anim = BALFROG_SPRITE_STANDING_STILL;
        boss->view.top = 48 * 0x200;
        boss->view.bottom = 16 * 0x200;

        if (boss->dir == DIR_LEFT && boss->x < player.x) {
          boss->dir = DIR_RIGHT;
          boss->act = BALFROG_INITIALIZE_LAND;
        }

        if (boss->dir == DIR_RIGHT && boss->x > player.x) {
          boss->dir = DIR_LEFT;
          boss->act = BALFROG_INITIALIZE_LAND;
        }

        npc_spawn(110, m_rand(4, 16) * (0x200 * 0x10), m_rand(0, 4) * (0x200 * 0x10), 0, 0, DIR_AUTO, NULL, 0x80);

        for (i = 0; i < 4; ++i)
          npc_spawn(NPC_SMOKE, boss->x + m_rand(-12, 12) * 0x200, boss->y + boss->hit.bottom, m_rand(-341, 341),
            m_rand(-3 * 0x200, 0), DIR_LEFT, NULL, 0x100);
      }

      break;

    case BALFROG_INITIALIZE_LAND:
      boss->anim = BALFROG_SPRITE_STANDING_STILL;
      boss->act_wait = 0;
      boss->act = BALFROG_LAND;
      // Fallthrough
    case BALFROG_LAND:
      ++boss->act_wait;

      boss->xvel = (boss->xvel * 8) / 9;

      if (boss->act_wait > 50) {
        boss->anim = BALFROG_SPRITE_MOUTH_BARELY_OPEN_CROUCHING;
        boss->anim_wait = 0;
        boss->act = BALFROG_INITIALIZE_SHOOT;
      }

      break;

    case BALFROG_INITIALIZE_SHOOT:
      ++boss->anim_wait;

      if (boss->anim_wait > 4) {
        boss->act = BALFROG_SHOOT;
        boss->act_wait = 0;
        boss->anim = BALFROG_SPRITE_MOUTH_OPEN_CROUCHING;
        boss->count1 = 16;
        npc_boss[1].bits |= NPC_SHOOTABLE;
        boss->tgt_x = boss->life;
      }

      break;

    case BALFROG_SHOOT:
      if (boss->shock != 0) {
        if (boss->count2++ / 2 % 2)
          boss->anim = BALFROG_SPRITE_MOUTH_OPEN_CROUCHING_FLASHING;
        else
          boss->anim = BALFROG_SPRITE_MOUTH_OPEN_CROUCHING;
      } else {
        boss->count2 = 0;
        boss->anim = BALFROG_SPRITE_MOUTH_OPEN_CROUCHING;
      }

      boss->xvel = (boss->xvel * 10) / 11;

      ++boss->act_wait;

      if (boss->act_wait > 16) {
        boss->act_wait = 0;
        --boss->count1;

        if (boss->dir == DIR_LEFT)
          deg = m_atan2(boss->x - 2 * (0x200 * 0x10) - player.x, boss->y - 8 * 0x200 - player.y);
        else
          deg = m_atan2(boss->x + 2 * (0x200 * 0x10) - player.x, boss->y - 8 * 0x200 - player.y);

        deg += (unsigned char)m_rand(-0x10, 0x10);

        yvel = m_sin(deg);
        xvel = m_cos(deg);

        if (boss->dir == DIR_LEFT)
          npc_spawn(NPC_PROJECTILE_BALFROG_SPITBALL, boss->x - 2 * (0x200 * 0x10), boss->y - 8 * 0x200, xvel, yvel,
                    DIR_LEFT, NULL, 0x100);
        else
          npc_spawn(NPC_PROJECTILE_BALFROG_SPITBALL, boss->x + 2 * (0x200 * 0x10), boss->y - 8 * 0x200, xvel, yvel,
                    DIR_LEFT, NULL, 0x100);

        snd_play_sound(PRIO_NORMAL, 39, FALSE);

        if (boss->count1 == 0 || boss->life < boss->tgt_x - 90) {
          boss->act = BALFROG_AFTER_SHOOT_WAIT;
          boss->act_wait = 0;
          boss->anim = BALFROG_SPRITE_MOUTH_BARELY_OPEN_CROUCHING;
          boss->anim_wait = 0;
          npc_boss[1].bits &= ~NPC_SHOOTABLE;
        }
      }

      break;

    case BALFROG_AFTER_SHOOT_WAIT:
      ++boss->anim_wait;

      if (boss->anim_wait > 10) {
        if (++npc_boss[1].count1 > 2) {
          npc_boss[1].count1 = 0;
          boss->act = BALFROG_INITIALIZE_LEAP_1;
        } else {
          boss->act = BALFROG_WAIT;
        }

        boss->anim_wait = 0;
        boss->anim = BALFROG_SPRITE_STANDING_STILL;
      }

      break;

    case BALFROG_INITIALIZE_LEAP_1:
      boss->act = BALFROG_INITIALIZE_LEAP_2;
      boss->act_wait = 0;
      boss->anim = BALFROG_SPRITE_STANDING_STILL;
      boss->xvel = 0;
      // Fallthrough
    case BALFROG_INITIALIZE_LEAP_2:
      ++boss->act_wait;

      if (boss->act_wait > 50) {
        boss->act = BALFROG_INITIALIZE_LEAP_3;
        boss->anim_wait = 0;
        boss->anim = BALFROG_SPRITE_MOUTH_BARELY_OPEN_CROUCHING;
      }

      break;

    case BALFROG_INITIALIZE_LEAP_3:
      ++boss->anim_wait;

      if (boss->anim_wait > 20) {
        boss->act = BALFROG_LEAP;
        boss->anim_wait = 0;
        boss->anim = BALFROG_SPRITE_STANDING_STILL;
      }

      break;

    case BALFROG_LEAP:
      ++boss->anim_wait;

      if (boss->anim_wait > 4) {
        boss->act = BALFROG_LEAP_MIDAIR;
        boss->anim = BALFROG_SPRITE_JUMPING;
        boss->yvel = -5 * 0x200;
        boss->view.top = 64 * 0x200;
        boss->view.bottom = 24 * 0x200;
        snd_play_sound(PRIO_NORMAL, 25, FALSE);
      }

      break;

    case BALFROG_LEAP_MIDAIR:
      if (boss->flags & COLL_GROUND) {
        snd_play_sound(PRIO_NORMAL, 26, FALSE);
        cam_start_quake_small(60);
        boss->act = BALFROG_WAIT;
        boss->anim = BALFROG_SPRITE_STANDING_STILL;
        boss->view.top = 48 * 0x200;
        boss->view.bottom = 16 * 0x200;

        for (i = 0; i < 2; ++i)
          npc_spawn(NPC_ENEMY_FROG, m_rand(4, 16) * (0x200 * 0x10), m_rand(0, 4) * (0x200 * 0x10), 0, 0, DIR_AUTO, NULL, 0x80);

        for (i = 0; i < 6; ++i)
          npc_spawn(NPC_ENEMY_PUCHI, m_rand(4, 16) * (0x200 * 0x10), m_rand(0, 4) * (0x200 * 0x10), 0, 0, DIR_AUTO, NULL, 0x80);

        for (i = 0; i < 8; ++i)
          npc_spawn(NPC_SMOKE, boss->x + m_rand(-12, 12) * 0x200, boss->y + boss->hit.bottom, m_rand(-341, 341),
            m_rand(-3 * 0x200, 0), DIR_LEFT, NULL, 0x100);

        if (boss->dir == DIR_LEFT && boss->x < player.x) {
          boss->dir = DIR_RIGHT;
          boss->act = BALFROG_INITIALIZE_LAND;
        }

        if (boss->dir == DIR_RIGHT && boss->x > player.x) {
          boss->dir = DIR_LEFT;
          boss->act = BALFROG_INITIALIZE_LAND;
        }
      }

      break;

    case BALFROG_DIE:
      boss->act = BALFROG_DIE_FLASHING;
      boss->anim = BALFROG_SPRITE_MOUTH_OPEN_CROUCHING;
      boss->act_wait = 0;
      boss->xvel = 0;
      snd_play_sound(PRIO_NORMAL, 72, FALSE);

      for (i = 0; i < 8; ++i)
        npc_spawn(NPC_SMOKE, boss->x + m_rand(-12, 12) * 0x200, boss->y + m_rand(-12, 12) * 0x200, m_rand(-341, 341),
          m_rand(-3 * 0x200, 0), DIR_LEFT, NULL, 0x100);

      npc_boss[1].cond = 0;
      npc_boss[2].cond = 0;
      // Fallthrough
    case BALFROG_DIE_FLASHING:
      ++boss->act_wait;

      if (boss->act_wait % 5 == 0)
        npc_spawn(NPC_SMOKE, boss->x + m_rand(-12, 12) * 0x200, boss->y + m_rand(-12, 12) * 0x200, m_rand(-341, 341),
          m_rand(-3 * 0x200, 0), DIR_LEFT, NULL, 0x100);

      if (boss->act_wait / 2 % 2)
        boss->x -= 1 * 0x200;
      else
        boss->x += 1 * 0x200;

      if (boss->act_wait > 100) {
        boss->act_wait = 0;
        boss->act = BALFROG_REVERT;
      }

      break;

    case BALFROG_REVERT:
      ++boss->act_wait;

      if (boss->act_wait / 2 % 2) {
        boss->view.front = 20 * 0x200;
        boss->view.top = 12 * 0x200;
        boss->view.back = 20 * 0x200;
        boss->view.bottom = 12 * 0x200;
        boss->anim = BALFROG_SPRITE_BALROG_WHITE;
      } else {
        boss->view.front = 48 * 0x200;
        boss->view.top = 48 * 0x200;
        boss->view.back = 32 * 0x200;
        boss->view.bottom = 16 * 0x200;
        boss->anim = BALFROG_SPRITE_MOUTH_OPEN_CROUCHING;
      }

      if (boss->act_wait % 9 == 0)
        npc_spawn(NPC_SMOKE, boss->x + m_rand(-12, 12) * 0x200, boss->y + m_rand(-12, 12) * 0x200, m_rand(-341, 341),
          m_rand(-3 * 0x200, 0), DIR_LEFT, NULL, 0x100);

      if (boss->act_wait > 150) {
        boss->act = BALFROG_NOP_START;
        boss->hit.bottom = 12 * 0x200;
      }

      break;

    case BALFROG_NOP_START:
      boss->act = BALFROG_NOP;
      // Fallthrough
    case BALFROG_NOP:
      if (boss->flags & COLL_GROUND) {
        boss->act = BALFROG_GO_INTO_CEILING;
        boss->act_wait = 0;
        boss->anim = BALFROG_SPRITE_BALROG_CROUCHING;
      }

      break;

    case BALFROG_GO_INTO_CEILING:
      ++boss->act_wait;

      if (boss->act_wait > 30) {
        boss->anim = BALFROG_SPRITE_BALROG_JUMPING;
        boss->yvel = -5 * 0x200;
        boss->bits |= NPC_IGNORE_SOLIDITY;
        boss->act = BALFROG_GONE_INTO_CEILING;
      }

      break;

    case BALFROG_GONE_INTO_CEILING:
      boss->yvel = -5 * 0x200;

      if (boss->y < 0) {
        boss->cond = 0;
        snd_play_sound(PRIO_NORMAL, 26, FALSE);
        cam_start_quake_small(30);
      }

      npc_boss_max = 0;

      break;
  }

  boss->yvel += 0x40;
  if (boss->yvel > 0x5FF) boss->yvel = 0x5FF;

  boss->x += boss->xvel;
  boss->y += boss->yvel;

  if (boss->dir == DIR_LEFT)
    boss->rect = &rc_left[boss->anim];
  else
    boss->rect = &rc_right[boss->anim];

  boss_act_balfrog_sub01();
  boss_act_balfrog_sub02();
}
