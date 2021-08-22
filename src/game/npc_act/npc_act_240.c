#include "game/npc_act/npc_act.h"

// Mimiga (jailed)
void npc_act_240(npc_t *npc) {
  static const rect_t rc_left[6] = {
    {160, 64, 176, 80}, {176, 64, 192, 80}, {192, 64, 208, 80},
    {160, 64, 176, 80}, {208, 64, 224, 80}, {160, 64, 176, 80},
  };

  static const rect_t rc_right[6] = {
    {160, 80, 176, 96}, {176, 80, 192, 96}, {192, 80, 208, 96},
    {160, 80, 176, 96}, {208, 80, 224, 96}, {160, 80, 176, 96},
  };

  switch (npc->act) {
    case 0:
      npc->act = 1;
      npc->anim = 0;
      npc->anim_wait = 0;
      npc->xvel = 0;
      // Fallthrough
    case 1:
      if (m_rand(0, 60) == 1) {
        npc->act = 2;
        npc->act_wait = 0;
        npc->anim = 1;
      }

      if (m_rand(0, 60) == 1) {
        npc->act = 10;
        npc->act_wait = 0;
        npc->anim = 1;
      }

      break;

    case 2:
      if (++npc->act_wait > 8) {
        npc->act = 1;
        npc->anim = 0;
      }

      break;

    case 10:
      npc->act = 11;
      npc->act_wait = m_rand(0, 16);
      npc->anim = 2;
      npc->anim_wait = 0;

      if (m_rand(0, 9) % 2)
        npc->dir = 0;
      else
        npc->dir = 2;
      // Fallthrough
    case 11:
      if (npc->dir == 0 && npc->flags & 1)
        npc->dir = 2;
      else if (npc->dir == 2 && npc->flags & 4)
        npc->dir = 0;

      if (npc->dir == 0)
        npc->xvel = -0x200;
      else
        npc->xvel = 0x200;

      if (++npc->anim_wait > 4) {
        npc->anim_wait = 0;
        ++npc->anim;
      }

      if (npc->anim > 5) npc->anim = 2;

      if (++npc->act_wait > 32) npc->act = 0;

      break;
  }

  npc->yvel += 0x20;
  if (npc->yvel > 0x5FF) npc->yvel = 0x5FF;

  npc->x += npc->xvel;
  npc->y += npc->yvel;

  if (npc->dir == 0)
    npc->rect = &rc_left[npc->anim];
  else
    npc->rect = &rc_right[npc->anim];
}

// Critter (Last Cave)
void npc_act_241(npc_t *npc) {
  static const rect_t rc_left[3] = {
    {0, 0, 16, 16},
    {16, 0, 32, 16},
    {32, 0, 48, 16},
  };

  static const rect_t rc_right[3] = {
    {0, 16, 16, 32},
    {16, 16, 32, 32},
    {32, 16, 48, 32},
  };

  switch (npc->act) {
    case 0:
      npc->y += 3 * 0x200;
      npc->act = 1;
      // Fallthrough
    case 1:
      if (npc->x > player.x)
        npc->dir = 0;
      else
        npc->dir = 2;

      if (npc->act_wait >= 8 && npc->x - (144 * 0x200) < player.x && npc->x + (144 * 0x200) > player.x &&
          npc->y - (80 * 0x200) < player.y && npc->y + (80 * 0x200) > player.y) {
        npc->anim = 1;
      } else {
        if (npc->act_wait < 8) ++npc->act_wait;

        npc->anim = 0;
      }

      if (npc->shock) {
        npc->act = 2;
        npc->anim = 0;
        npc->act_wait = 0;
      }

      if (npc->act_wait >= 8 && npc->x - (96 * 0x200) < player.x && npc->x + (96 * 0x200) > player.x &&
          npc->y - (80 * 0x200) < player.y && npc->y + (96 * 0x200) > player.y) {
        npc->act = 2;
        npc->anim = 0;
        npc->act_wait = 0;
      }

      break;

    case 2:
      if (++npc->act_wait > 8) {
        npc->act = 3;
        npc->anim = 2;
        npc->yvel = -0x5FF;
        snd_play_sound(PRIO_NORMAL, 30, FALSE);

        if (npc->dir == 0)
          npc->xvel = -0x200;
        else
          npc->xvel = 0x200;
      }

      break;

    case 3:
      if (npc->flags & 8) {
        npc->xvel = 0;
        npc->act_wait = 0;
        npc->anim = 0;
        npc->act = 1;
        snd_play_sound(PRIO_NORMAL, 23, FALSE);
      }

      break;
  }

  npc->yvel += 0x55;
  if (npc->yvel > 0x5FF) npc->yvel = 0x5FF;

  npc->x += npc->xvel;
  npc->y += npc->yvel;

  if (npc->dir == 0)
    npc->rect = &rc_left[npc->anim];
  else
    npc->rect = &rc_right[npc->anim];
}

// Bat (Last Cave)
void npc_act_242(npc_t *npc) {
  if (npc->x < 0 || npc->x > stage_data->width * 0x10 * 0x200) {
    npc_show_death_damage(npc);
    return;
  }

  switch (npc->act) {
    case 0:
      npc->act = 1;
      npc->tgt_x = npc->x;
      npc->tgt_y = npc->y;
      npc->act_wait = m_rand(0, 50);
      // Fallthrough
    case 1:
      if (npc->act_wait != 0) {
        --npc->act_wait;
        break;
      }

      npc->act = 2;
      npc->yvel = 0x400;
      // Fallthrough
    case 2:
      if (npc->dir == 0)
        npc->xvel = -0x100;
      else
        npc->xvel = 0x100;

      if (npc->tgt_y < npc->y) npc->yvel -= 0x10;
      if (npc->tgt_y > npc->y) npc->yvel += 0x10;

      if (npc->yvel > 0x300) npc->yvel = 0x300;
      if (npc->yvel < -0x300) npc->yvel = -0x300;

      break;
  }

  npc->x += npc->xvel;
  npc->y += npc->yvel;

  static const rect_t rect_left[4] = {
    {32, 32, 48, 48},
    {48, 32, 64, 48},
    {64, 32, 80, 48},
    {80, 32, 96, 48},
  };

  static const rect_t rect_right[4] = {
    {32, 48, 48, 64},
    {48, 48, 64, 64},
    {64, 48, 80, 64},
    {80, 48, 96, 64},
  };

  if (++npc->anim_wait > 1) {
    npc->anim_wait = 0;
    ++npc->anim;
  }

  if (npc->anim > 2) npc->anim = 0;

  if (npc->dir == 0)
    npc->rect = &rect_left[npc->anim];
  else
    npc->rect = &rect_right[npc->anim];
}

// Bat generator (Last Cave)
void npc_act_243(npc_t *npc) {
  switch (npc->act) {
    case 0:
      npc->act = 1;
      npc->act_wait = m_rand(0, 500);
      // Fallthrough
    case 1:
      if (npc->act_wait != 0) {
        --npc->act_wait;
      } else {
        npc->act = 0;
        npc_spawn(242, npc->x, npc->y + (m_rand(-0x20, 0x20) * 0x200), 0, 0, npc->dir, NULL, 0x100);
      }

      break;
  }
}

// Lava drop
void npc_act_244(npc_t *npc) {
  static const rect_t rc = {96, 0, 104, 16};
  bool hit;

  int i;

  npc->yvel += 0x40;

  hit = FALSE;

  if (npc->flags & 0xFF) hit = TRUE;

  if (npc->act_wait > 10 && npc->flags & 0x100) hit = TRUE;

  if (hit) {
    for (i = 0; i < 3; ++i) caret_spawn(npc->x, npc->y + (4 * 0x200), CARET_BUBBLE, DIR_RIGHT);

    if (npc->x > player.x - (256 * 0x200) && npc->x < player.x + (256 * 0x200) && npc->y > player.y - (160 * 0x200) &&
        npc->y < player.y + (160 * 0x200))
      snd_play_sound(PRIO_NORMAL, 21, FALSE);

    npc->cond = 0;
  } else {
    if (npc->yvel > 0x5FF) npc->yvel = 0x5FF;

    npc->y += npc->yvel;

    npc->rect = &rc;
  }
}

// Lava drop generator
void npc_act_245(npc_t *npc) {
  static const rect_t rc[4] = {
    {0, 0, 0, 0},
    {104, 0, 112, 16},
    {112, 0, 120, 16},
    {120, 0, 128, 16},
  };

  switch (npc->act) {
    case 0:
      npc->act = 1;
      npc->tgt_x = npc->x;
      npc->act_wait = npc->event_num;
      // Fallthrough
    case 1:
      npc->anim = 0;

      if (npc->act_wait != 0) {
        --npc->act_wait;
        return;
      }

      npc->act = 10;
      npc->anim_wait = 0;
      break;

    case 10:
      if (++npc->anim_wait > 10) {
        npc->anim_wait = 0;
        ++npc->anim;
      }

      if (npc->anim > 3) {
        npc->anim = 0;
        npc->act = 1;
        npc->act_wait = npc->event_flag;
        npc_spawn(244, npc->x, npc->y, 0, 0, 0, NULL, 0x100);
      }

      break;
  }

  if (npc->anim_wait / 2 % 2)
    npc->x = npc->tgt_x;
  else
    npc->x = npc->tgt_x + 0x200;

  npc->rect = &rc[npc->anim];
}

// Press (proximity)
void npc_act_246(npc_t *npc) {
  static const rect_t rc_left[3] = {
    {144, 112, 160, 136},
    {160, 112, 176, 136},
    {176, 112, 192, 136},
  };

  int i;

  switch (npc->act) {
    case 0:
      npc->act = 1;
      npc->y -= 4 * 0x200;
      // Fallthrough
    case 1:
      if (player.x < npc->x + (8 * 0x200) && player.x > npc->x - (8 * 0x200) && player.y > npc->y + (8 * 0x200) &&
          player.y < npc->y + (128 * 0x200))
        npc->act = 5;

      break;

    case 5:
      if (npc->flags & 8) {
        // Another place where this blank space is needed for ASM-accuracy.
        // Chances are there used to be commented-out code here.
      } else {
        npc->act = 10;
        npc->anim_wait = 0;
        npc->anim = 1;
      }

      break;

    case 10:
      if (++npc->anim_wait > 2) {
        npc->anim_wait = 0;
        ++npc->anim;
      }

      if (npc->anim > 2) npc->anim = 2;

      if (player.y > npc->y) {
        npc->bits &= ~NPC_SOLID_HARD;
        npc->damage = 0x7F;
      } else {
        npc->bits |= NPC_SOLID_HARD;
        npc->damage = 0;
      }

      if (npc->flags & 8) {
        if (npc->anim > 1) {
          for (i = 0; i < 4; ++i) npc_spawn(4, npc->x, npc->y, m_rand(-341, 341), m_rand(-0x600, 0), 0, NULL, 0x100);

          snd_play_sound(PRIO_NORMAL, 26, FALSE);
          cam_start_quake_small(10);
        }

        npc->act = 20;
        npc->anim = 0;
        npc->anim_wait = 0;
        npc->bits |= NPC_SOLID_HARD;
        npc->damage = 0;
      }

      break;
  }

  if (npc->act >= 5) {
    npc->yvel += 0x80;
    if (npc->yvel > 0x5FF) npc->yvel = 0x5FF;

    npc->y += npc->yvel;
  }

  npc->rect = &rc_left[npc->anim];
}

// Misery (boss)
void npc_act_247(npc_t *npc) {
  u8 deg;
  int xvel, yvel;

  static const rect_t rc_left[9] = {
    {0, 0, 16, 16},  {16, 0, 32, 16},  {32, 0, 48, 16}, {48, 0, 64, 16},   {64, 0, 80, 16},
    {80, 0, 96, 16}, {96, 0, 112, 16}, {0, 0, 0, 0},    {112, 0, 128, 16},
  };

  static const rect_t rc_right[9] = {
    {0, 16, 16, 32},  {16, 16, 32, 32},  {32, 16, 48, 32}, {48, 16, 64, 32},   {64, 16, 80, 32},
    {80, 16, 96, 32}, {96, 16, 112, 32}, {0, 0, 0, 0},     {112, 16, 128, 32},
  };

  switch (npc->act) {
    case 0:
      npc->act = 1;
      npc->y += 6 * 0x200;
      npc->tgt_y = 64 * 0x200;
      // Fallthrough
    case 1:
      if (m_rand(0, 120) == 10) {
        npc->act = 2;
        npc->act_wait = 0;
        npc->anim = 1;
      }

      break;

    case 2:
      if (++npc->act_wait > 8) {
        npc->act = 1;
        npc->anim = 0;
      }

      break;

    case 20:
      npc->xvel = 0;
      npc->yvel += 0x40;

      if (npc->flags & 8) {
        npc->act = 21;
        npc->anim = 2;
      }

      break;

    case 21:
      if (m_rand(0, 120) == 10) {
        npc->act = 22;
        npc->act_wait = 0;
        npc->anim = 3;
      }

      break;

    case 22:
      if (++npc->act_wait > 8) {
        npc->act = 21;
        npc->anim = 2;
      }

      break;

    case 100:
      npc->act = 101;
      npc->act_wait = 0;
      npc->anim = 0;
      npc->xvel = 0;
      npc->bits |= NPC_SHOOTABLE;
      npc->count2 = npc->life;
      // Fallthrough
    case 101:
      if (player.x < npc->x)
        npc->dir = 0;
      else
        npc->dir = 2;

      if (npc->y < npc->tgt_y)
        npc->yvel += 0x20;
      else
        npc->yvel -= 0x20;

      if (npc->yvel < -0x200) npc->yvel = -0x200;
      if (npc->yvel > 0x200) npc->yvel = 0x200;

      if (++npc->act_wait > 200 || npc->life <= npc->count2 - 80) {
        npc->act_wait = 0;
        npc->act = 110;
      }

      break;

    case 110:
      npc->act = 111;
      npc->act_wait = 0;
      npc->xvel = 0;
      npc->yvel = 0;
      npc->bits &= ~NPC_SHOOTABLE;
      // Fallthrough
    case 111:
      if (++npc->act_wait % 2)
        npc->anim = 5;
      else
        npc->anim = 6;

      if (npc->act_wait > 30) {
        npc->act_wait = 0;

        if (++npc->count1 % 3 == 0)
          npc->act = 113;
        else
          npc->act = 112;

        npc->anim = 4;
      }

      break;

    case 112:
      if (++npc->act_wait % 6 == 0) {
        deg = m_atan2(npc->x - player.x, npc->y - player.y);
        deg += (u8)m_rand(-4, 4);
        yvel = m_sin(deg) * 4;
        xvel = m_cos(deg) * 4;

        npc_spawn(248, npc->x, npc->y + (4 * 0x200), xvel, yvel, 0, NULL, 0x100);
        snd_play_sound(PRIO_NORMAL, 34, FALSE);
      }

      if (npc->act_wait > 30) {
        npc->act_wait = 0;
        npc->act = 150;
      }

      break;

    case 113:
      if (++npc->act_wait == 10) {
        yvel = player.y - (64 * 0x200);
        npc_spawn(279, player.x, yvel, 0, 0, 1, NULL, 0x100);
      }

      if (npc->act_wait > 30) {
        npc->act_wait = 0;
        npc->act = 150;
      }

      break;

    case 150:
      npc->act = 151;
      npc->act_wait = 0;
      npc->anim = 7;

      npc_spawn(249, npc->x, npc->y, 0, 0, 0, NULL, 0x100);
      npc_spawn(249, npc->x, npc->y, 0, 0, 2, NULL, 0x100);

      npc->tgt_x = m_rand(9, 31) * 0x200 * 0x10;
      npc->tgt_y = m_rand(5, 7) * 0x200 * 0x10;

      snd_play_sound(PRIO_NORMAL, 29, FALSE);
      // Fallthrough
    case 151:
      if (++npc->act_wait == 42) {
        npc_spawn(249, npc->tgt_x + (16 * 0x200), npc->tgt_y, 0, 0, 0, NULL, 0x100);
        npc_spawn(249, npc->tgt_x - (16 * 0x200), npc->tgt_y, 0, 0, 2, NULL, 0x100);
      }

      if (npc->act_wait > 50) {
        npc->act_wait = 0;
        npc->yvel = -0x200;
        npc->bits |= NPC_SHOOTABLE;
        npc->x = npc->tgt_x;
        npc->y = npc->tgt_y;

        if (npc->life < 340) {
          npc_spawn(252, 0, 0, 0, 0, 0, npc, 0x100);
          npc_spawn(252, 0, 0, 0, 0, 0x80, npc, 0x100);
        }

        if (npc->life < 180) {
          npc_spawn(252, 0, 0, 0, 0, 0x40, npc, 0x100);
          npc_spawn(252, 0, 0, 0, 0, 0xC0, npc, 0x100);
        }

        if (player.x < npc->x - (112 * 0x200) || player.x > npc->x + (112 * 0x200))
          npc->act = 160;
        else
          npc->act = 100;
      }

      break;

    case 160:
      npc->act = 161;
      npc->act_wait = 0;
      npc->anim = 4;

      if (player.x < npc->x)
        npc->dir = 0;
      else
        npc->dir = 2;
      // Fallthrough
    case 161:
      if (npc->y < npc->tgt_y)
        npc->yvel += 0x20;
      else
        npc->yvel -= 0x20;

      if (npc->yvel < -0x200) npc->yvel = -0x200;
      if (npc->yvel > 0x200) npc->yvel = 0x200;

      if (++npc->act_wait % 24 == 0) {
        npc_spawn(250, npc->x, npc->y + (4 * 0x200), 0, 0, 0, NULL, 0x100);
        snd_play_sound(PRIO_NORMAL, 34, FALSE);
      }

      if (npc->act_wait > 72) {
        npc->act_wait = 0;
        npc->act = 100;
      }

      break;

    case 1000:
      npc->bits &= ~NPC_SHOOTABLE;
      npc->act = 1001;
      npc->act_wait = 0;
      npc->anim = 4;

      npc->tgt_x = npc->x;
      npc->tgt_y = npc->y;

      npc->xvel = 0;
      npc->yvel = 0;

      npc_delete_by_class(252, TRUE);

      npc_spawn(4, npc->x, npc->y, 0, 0, 0, NULL, 0x100);
      npc_spawn(4, npc->x, npc->y, 0, 0, 0, NULL, 0x100);
      npc_spawn(4, npc->x, npc->y, 0, 0, 0, NULL, 0x100);
      // Fallthrough
    case 1001:
      if (++npc->act_wait / 2 % 2)
        npc->x = npc->tgt_x + (1 * 0x200);
      else
        npc->x = npc->tgt_x;

      break;

    case 1010:
      npc->yvel += 0x10;

      if (npc->flags & 8) {
        npc->act = 1020;
        npc->anim = 8;
      }

      break;
  }

  if (npc->xvel < -0x200) npc->xvel = -0x200;
  if (npc->xvel > 0x200) npc->xvel = 0x200;

  if (npc->yvel < -0x400) npc->yvel = -0x400;
  if (npc->yvel > 0x400) npc->yvel = 0x400;

  npc->x += npc->xvel;
  npc->y += npc->yvel;

  if (npc->dir == 0)
    npc->rect = &rc_left[npc->anim];
  else
    npc->rect = &rc_right[npc->anim];
}

// Boss Misery (vanishing)
void npc_act_248(npc_t *npc) {
  if (npc->flags & 0xFF) {
    npc->cond = 0;
    caret_spawn(npc->x, npc->y, CARET_PROJECTILE_DISSIPATION, DIR_LEFT);
  }

  npc->y += npc->yvel;
  npc->x += npc->xvel;

  static const rect_t rect_left[3] = {
    {0, 48, 16, 64},
    {16, 48, 32, 64},
    {32, 48, 48, 64},
  };

  if (++npc->anim_wait > 1) {
    npc->anim_wait = 0;

    if (++npc->anim > 2) npc->anim = 0;
  }

  npc->rect = &rect_left[npc->anim];

  if (++npc->count1 > 300) {
    npc->cond = 0;
    caret_spawn(npc->x, npc->y, CARET_PROJECTILE_DISSIPATION, DIR_LEFT);
  }
}

// Boss Misery energy shot
void npc_act_249(npc_t *npc) {
  static const rect_t rc[2] = {
    {48, 48, 64, 64},
    {64, 48, 80, 64},
  };

  if (++npc->act_wait > 8) npc->cond = 0;

  if (npc->dir == 0) {
    npc->rect = &rc[0];
    npc->x -= 2 * 0x200;
  } else {
    npc->rect = &rc[1];
    npc->x += 2 * 0x200;
  }
}

// Boss Misery lightning ball
void npc_act_250(npc_t *npc) {
  static const rect_t rc[3] = {
    {0, 32, 16, 48},
    {16, 32, 32, 48},
    {32, 32, 48, 48},
  };

  switch (npc->act) {
    case 0:
      npc->act = 1;
      npc->tgt_y = npc->y;
      npc->xvel = 0;
      npc->yvel = -0x200;
      // Fallthrough
    case 1:
      if (npc->x < player.x)
        npc->xvel += 0x10;
      else
        npc->xvel -= 0x10;

      if (npc->y < npc->tgt_y)
        npc->yvel += 0x20;
      else
        npc->yvel -= 0x20;

      if (npc->xvel > 0x200) npc->xvel = 0x200;
      if (npc->xvel < -0x200) npc->xvel = -0x200;

      if (npc->yvel > 0x200) npc->yvel = 0x200;
      if (npc->yvel < -0x200) npc->yvel = -0x200;

      npc->x += npc->xvel;
      npc->y += npc->yvel;

      if (++npc->anim_wait > 2) {
        npc->anim_wait = 0;
        ++npc->anim;
      }

      if (npc->anim > 1) npc->anim = 0;

      if (player.x > npc->x - (8 * 0x200) && player.x < npc->x + (8 * 0x200) && player.y > npc->y) npc->act = 10;

      break;

    case 10:
      npc->act = 11;
      npc->act_wait = 0;
      // Fallthrough
    case 11:
      if (++npc->act_wait > 10) {
        npc_spawn(251, npc->x, npc->y, 0, 0, 0, NULL, 0x100);
        snd_play_sound(PRIO_NORMAL, 101, FALSE);
        npc->cond = 0;
        return;
      }

      if (npc->act_wait / 2 % 2)
        npc->anim = 2;
      else
        npc->anim = 1;
  }

  npc->rect = &rc[npc->anim];
}

// Boss Misery lightning
void npc_act_251(npc_t *npc) {
  static const rect_t rc[2] = {
    {80, 32, 96, 64},
    {96, 32, 112, 64},
  };

  switch (npc->act) {
    case 0:
      npc->act = 1;
      // Fallthrough
    case 1:
      if (++npc->anim > 1) npc->anim = 0;

      npc->y += 8 * 0x200;

      if (npc->flags & 0xFF) {
        npc_spawn_death_fx(npc->x, npc->y, npc->view.back, 3, 0);
        npc->cond = 0;
      }

      break;
  }

  npc->rect = &rc[npc->anim];
}

// Boss Misery bats
void npc_act_252(npc_t *npc) {
  static const rect_t rc_left[4] = {
    {48, 32, 64, 48},
    {112, 32, 128, 48},
    {128, 32, 144, 48},
    {144, 32, 160, 48},
  };

  static const rect_t rc_right[4] = {
    {48, 32, 64, 48},
    {112, 48, 128, 64},
    {128, 48, 144, 64},
    {144, 48, 160, 64},
  };

  u8 deg;

  switch (npc->act) {
    case 0:
      npc->act = 1;
      npc->act_wait = 0;
      npc->count1 = npc->dir;
      // Fallthrough
    case 1:
      npc->count1 += 2;
      npc->count1 %= 0x100;

      deg = npc->count1;

      if (npc->act_wait < 192) ++npc->act_wait;

      npc->x = npc->parent->x + (m_cos(deg) * npc->act_wait) / 4;
      npc->y = npc->parent->y + (m_sin(deg) * npc->act_wait) / 4;

      if (npc->parent->act == 151) {
        npc->act = 10;
        npc->anim = 0;
      }

      break;

    case 10:
      npc->act = 11;
      npc->bits |= NPC_SHOOTABLE;
      npc->bits &= ~NPC_INVULNERABLE;
      npc->bits &= ~NPC_IGNORE_SOLIDITY;

      deg = m_atan2(npc->x - player.x, npc->y - player.y);
      deg += (u8)m_rand(-3, 3);
      npc->xvel = m_cos(deg);
      npc->yvel = m_sin(deg);

      npc->anim = 1;
      npc->anim_wait = 0;

      if (npc->x > player.x)
        npc->dir = 0;
      else
        npc->dir = 2;
      // Fallthrough
    case 11:
      npc->x += npc->xvel;
      npc->y += npc->yvel;

      if (npc->flags & 0xFF) {
        npc_spawn(4, npc->x, npc->y, 0, 0, 0, NULL, 0x100);
        npc_spawn(4, npc->x, npc->y, 0, 0, 0, NULL, 0x100);
        npc_spawn(4, npc->x, npc->y, 0, 0, 0, NULL, 0x100);
        npc->cond = 0;
      }

      if (++npc->anim_wait > 4) {
        npc->anim_wait = 0;
        ++npc->anim;
      }

      if (npc->anim > 3) npc->anim = 1;

      break;
  }

  if (npc->dir == 0)
    npc->rect = &rc_left[npc->anim];
  else
    npc->rect = &rc_right[npc->anim];
}

// EXP capsule
void npc_act_253(npc_t *npc) {
  switch (npc->act) {
    case 0:
      npc->act = 1;
      // Fallthrough
    case 1:
      if (++npc->anim_wait > 4) {
        npc->anim_wait = 0;
        ++npc->anim;
      }

      if (npc->anim > 1) npc->anim = 0;

      break;
  }

  if (npc->life <= 100) {
    npc_spawn_exp(npc->x, npc->y, npc->event_flag);
    npc_spawn_death_fx(npc->x, npc->y, npc->view.back, 8, 0);
    snd_play_sound(PRIO_NORMAL, 25, FALSE);
    npc->cond = 0;
  }

  static const rect_t rc[2] = {
    {0, 64, 16, 80},
    {16, 64, 32, 80},
  };

  npc->rect = &rc[npc->anim];
}

// Helicopter
void npc_act_254(npc_t *npc) {
  static const rect_t rc[2] = {
    {0, 0, 128, 64},
    {0, 64, 128, 128},
  };

  switch (npc->act) {
    case 0:
      npc->act = 1;
      npc_spawn(255, npc->x + (18 * 0x200), npc->y - (57 * 0x200), 0, 0, 0, npc, 0x100);
      npc_spawn(255, npc->x - (32 * 0x200), npc->y - (52 * 0x200), 0, 0, 2, npc, 0x100);
      break;

    case 20:
      npc->act_wait = 0;
      npc->count1 = 60;
      npc->act = 21;
      break;

    case 30:
      npc->act = 21;
      npc_spawn(223, npc->x - (11 * 0x200), npc->y - (14 * 0x200), 0, 0, 0, NULL, 0x100);
      break;

    case 40:
      npc->act = 21;
      npc_spawn(223, npc->x - (9 * 0x200), npc->y - (14 * 0x200), 0, 0, 0, NULL, 0x100);
      npc_spawn(40, npc->x - (22 * 0x200), npc->y - (14 * 0x200), 0, 0, 0, NULL, 0x100);
      npc_spawn(93, npc->x - (35 * 0x200), npc->y - (14 * 0x200), 0, 0, 0, NULL, 0x100);
      break;
  }

  if (npc->dir == 0)
    npc->rect = &rc[0];
  else
    npc->rect = &rc[1];
}

// Helicopter blades
void npc_act_255(npc_t *npc) {
  static const rect_t rc_left[4] = {
    {128, 0, 240, 16},
    {128, 16, 240, 32},
    {128, 32, 240, 48},
    {128, 16, 240, 32},
  };

  static const rect_t rc_right[4] = {
    {240, 0, 320, 16},
    {240, 16, 320, 32},
    {240, 32, 320, 48},
    {240, 16, 320, 32},
  };

  switch (npc->act) {
    case 0:
      npc->act = 1;

      if (npc->dir == 0) {
        npc->view.front = (56 * 0x200);
        npc->view.back = (56 * 0x200);
      } else {
        npc->view.front = (40 * 0x200);
        npc->view.back = (40 * 0x200);
      }
      // Fallthrough
    case 1:
      if (npc->parent->act >= 20) npc->act = 10;

      break;

    case 10:
      npc->act = 11;
      // Fallthrough
    case 11:
      if (++npc->anim > 3) npc->anim = 0;

      break;
  }

  if (npc->dir == 0) {
    npc->x = npc->parent->x + (18 * 0x200);
    npc->y = npc->parent->y - (57 * 0x200);
  } else {
    npc->x = npc->parent->x - (32 * 0x200);
    npc->y = npc->parent->y - (52 * 0x200);
  }

  if (npc->dir == 0)
    npc->rect = &rc_left[npc->anim];
  else
    npc->rect = &rc_right[npc->anim];
}

// Doctor (facing away)
void npc_act_256(npc_t *npc) {
  static const rect_t rc_left[6] = {
    {48, 160, 72, 192}, {72, 160, 96, 192}, {0, 128, 24, 160},
    {24, 128, 48, 160}, {0, 160, 24, 192},  {24, 160, 48, 192},
  };

  switch (npc->act) {
    case 0:
      npc_doctor_state.crystal_x = 0;
      npc->act = 1;
      npc->y -= 8 * 0x200;
      // Fallthrough
    case 1:
      npc->anim = 0;
      break;

    case 10:
      npc->act = 11;
      npc->anim_wait = 0;
      npc->anim = 0;
      npc->count1 = 0;
      // Fallthrough
    case 11:
      if (++npc->anim_wait > 5) {
        npc->anim_wait = 0;
        ++npc->anim;
      }

      if (npc->anim > 1) {
        npc->anim = 0;
        ++npc->count1;
      }

      if (npc->count1 > 5) npc->act = 1;

      break;

    case 20:
      npc->act = 21;
      // Fallthrough
    case 21:
      npc->anim = 2;
      break;

    case 40:
      npc->act = 41;
      npc_spawn(257, npc->x - (14 * 0x200), npc->y - (16 * 0x200), 0, 0, 0, NULL, 0x100);
      npc_spawn(257, npc->x - (14 * 0x200), npc->y - (16 * 0x200), 0, 0, 2, NULL, 0xAA);
      // Fallthrough
    case 41:
      npc->anim = 4;
      break;

    case 50:
      npc->act = 51;
      npc->anim_wait = 0;
      npc->anim = 4;
      npc->count1 = 0;
      // Fallthrough
    case 51:
      if (++npc->anim_wait > 5) {
        npc->anim_wait = 0;
        ++npc->anim;
      }

      if (npc->anim > 5) {
        npc->anim = 4;
        ++npc->count1;
      }

      if (npc->count1 > 5) npc->act = 41;

      break;
  }

  npc->rect = &rc_left[npc->anim];
}

// Red crystal
void npc_act_257(npc_t *npc) {
  static const rect_t rc[3] = {
    {176, 32, 184, 48},
    {184, 32, 192, 48},
    {0, 0, 0, 0},
  };

  switch (npc->act) {
    case 0:
      npc->act = 1;
      // Fallthrough
    case 1:
      if (npc_doctor_state.crystal_x != 0) npc->act = 10;

      break;

    case 10:
      if (npc->x < npc_doctor_state.crystal_x) npc->xvel += 0x55;
      if (npc->x > npc_doctor_state.crystal_x) npc->xvel -= 0x55;

      if (npc->y < npc_doctor_state.crystal_y) npc->yvel += 0x55;
      if (npc->y > npc_doctor_state.crystal_y) npc->yvel -= 0x55;

      if (npc->xvel > 0x400) npc->xvel = 0x400;
      if (npc->xvel < -0x400) npc->xvel = -0x400;

      if (npc->yvel > 0x400) npc->yvel = 0x400;
      if (npc->yvel < -0x400) npc->yvel = -0x400;

      npc->x += npc->xvel;
      npc->y += npc->yvel;

      break;
  }

  if (++npc->anim_wait > 3) {
    npc->anim_wait = 0;
    ++npc->anim;
  }

  if (npc->anim > 1) npc->anim = 0;

  if (npc->dir == 0 && npc->xvel > 0) npc->anim = 2;
  if (npc->dir == 2 && npc->xvel < 0) npc->anim = 2;

  npc->rect = &rc[npc->anim];
}

// Mimiga (sleeping)
void npc_act_258(npc_t *npc) {
  static const rect_t rc = {48, 32, 64, 48};
  npc->rect = &rc;
}

// Curly (carried and unconcious)
void npc_act_259(npc_t *npc) {
  static const rect_t rc_left = {224, 96, 240, 112};
  static const rect_t rc_right = {224, 112, 240, 128};

  switch (npc->act) {
    case 0:
      npc->bits &= ~NPC_INTERACTABLE;
      npc->act = 1;
      // Fallthrough
    case 1:
      if (player.dir == 0)
        npc->dir = 0;
      else
        npc->dir = 2;

      npc->y = player.y - (4 * 0x200);

      if (npc->dir == 0) {
        npc->x = player.x + (3 * 0x200);
        npc->rect = &rc_left;
      } else {
        npc->x = player.x - (3 * 0x200);
        npc->rect = &rc_right;
      }

      if (player.anim % 2) {
        npc->rect_delta.top = 1;
        npc->rect_prev = NULL; // force texrect update
      }

      break;

    case 10:
      npc->act = 11;
      npc->xvel = 0x40;
      npc->yvel = -0x20;

      npc->rect = &rc_left;
      break;

    case 11:
      if (npc->y < (64 * 0x200)) npc->yvel = 0x20;
      npc->x += npc->xvel;
      npc->y += npc->yvel;
      break;

    case 20:
      npc_show_death_damage(npc);
      npc_spawn_death_fx(npc->x, npc->y, 0x2000, 0x40, 1);
      break;
  }
}
