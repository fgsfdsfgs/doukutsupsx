#include "game/npc_act/npc_act.h"

// Shovel Brigade (caged)
void npc_act_260(npc_t *npc) {
  static const rect_t rc_left[3] = {
    {128, 64, 144, 80},
    {144, 64, 160, 80},
    {224, 64, 240, 80},
  };

  static const rect_t rc_right[3] = {
    {128, 80, 144, 96},
    {144, 80, 160, 96},
    {224, 80, 240, 96},
  };

  switch (npc->act) {
    case 0:
      npc->x += 0x200;
      npc->y -= 0x400;
      npc->act = 1;
      npc->anim = 0;
      npc->anim_wait = 0;
      // Fallthrough
    case 1:
      if (m_rand(0, 160) == 1) {
        npc->act = 2;
        npc->act_wait = 0;
        npc->anim = 1;
      }

      break;

    case 2:
      if (++npc->act_wait > 12) {
        npc->act = 1;
        npc->anim = 0;
      }

      break;

    case 10:
      npc->act = 11;
      npc->anim = 2;
      npc_spawn(87, npc->x, npc->y - (16 * 0x200), 0, 0, 0, NULL, 0x100);

      break;
  }

  if (player.x < npc->x)
    npc->dir = 0;
  else
    npc->dir = 2;

  if (npc->dir == 0)
    npc->rect = &rc_left[npc->anim];
  else
    npc->rect = &rc_right[npc->anim];
}

// Chie (caged)
void npc_act_261(npc_t *npc) {
  static const rect_t rc_left[2] = {
    {112, 32, 128, 48},
    {128, 32, 144, 48},
  };

  static const rect_t rc_right[2] = {
    {112, 48, 128, 64},
    {128, 48, 144, 64},
  };

  switch (npc->act) {
    case 0:
      npc->x -= 1 * 0x200;
      npc->y -= 2 * 0x200;
      npc->act = 1;
      npc->anim = 0;
      npc->anim_wait = 0;
      // Fallthrough
    case 1:
      if (m_rand(0, 160) == 1) {
        npc->act = 2;
        npc->act_wait = 0;
        npc->anim = 1;
      }

      break;

    case 2:
      if (++npc->act_wait > 12) {
        npc->act = 1;
        npc->anim = 0;
      }

      break;
  }

  if (player.x < npc->x)
    npc->dir = 0;
  else
    npc->dir = 2;

  if (npc->dir == 0)
    npc->rect = &rc_left[npc->anim];
  else
    npc->rect = &rc_right[npc->anim];
}

// Chaco (caged)
void npc_act_262(npc_t *npc) {
  static const rect_t rc_left[2] = {
    {128, 0, 144, 16},
    {144, 0, 160, 16},
  };

  static const rect_t rc_right[2] = {
    {128, 16, 144, 32},
    {144, 16, 160, 32},
  };

  switch (npc->act) {
    case 0:
      npc->x -= 1 * 0x200;
      npc->y -= 2 * 0x200;
      npc->act = 1;
      npc->anim = 0;
      npc->anim_wait = 0;
      // Fallthrough
    case 1:
      if (m_rand(0, 160) == 1) {
        npc->act = 2;
        npc->act_wait = 0;
        npc->anim = 1;
      }

      break;

    case 2:
      if (++npc->act_wait > 12) {
        npc->act = 1;
        npc->anim = 0;
      }

      break;
  }

  if (player.x < npc->x)
    npc->dir = 0;
  else
    npc->dir = 2;

  if (npc->dir == 0)
    npc->rect = &rc_left[npc->anim];
  else
    npc->rect = &rc_right[npc->anim];
}

// Doctor (boss)
void npc_act_263(npc_t *npc) {
  int deg;
  int xvel, yvel;

  static const rect_t rc_left[9] = {
    {0, 0, 24, 32},   {24, 0, 48, 32},   {48, 0, 72, 32},   {0, 0, 0, 0},      {72, 0, 96, 32},
    {96, 0, 120, 32}, {120, 0, 144, 32}, {144, 0, 168, 32}, {264, 0, 288, 32},
  };

  static const rect_t rc_right[9] = {
    {0, 32, 24, 64},   {24, 32, 48, 64},   {48, 32, 72, 64},   {0, 0, 0, 0},       {72, 32, 96, 64},
    {96, 32, 120, 64}, {120, 32, 144, 64}, {144, 32, 168, 64}, {264, 32, 288, 64},
  };

  switch (npc->act) {
    case 0:
      npc->act = 1;
      npc->y += 8 * 0x200;
      npc->anim = 3;
      break;

    case 2:
      if (++npc->act_wait / 2 % 2)
        npc->anim = 0;
      else
        npc->anim = 3;

      if (npc->act_wait > 50) npc->act = 10;

      break;

    case 10:
      npc->yvel += 0x80;
      npc->bits |= NPC_SHOOTABLE;
      npc->damage = 3;

      if (npc->flags & 8) {
        npc->act = 20;
        npc->act_wait = 0;
        npc->anim = 0;
        npc->count2 = npc->life;

        if (npc->x > player.x)
          npc->dir = 0;
        else
          npc->dir = 2;
      }

      break;

    case 20:
      if (++npc->act_wait < 50 && npc->life < npc->count2 - 20) npc->act_wait = 50;

      if (npc->act_wait == 50) {
        if (npc->x > player.x)
          npc->dir = 0;
        else
          npc->dir = 2;

        npc->anim = 4;
      }

      if (npc->act_wait == 80) {
        npc->anim = 5;
        snd_play_sound(PRIO_NORMAL, 25, FALSE);

        if (npc->dir == 0) {
          npc_spawn(264, npc->x - (16 * 0x200), npc->y, 0, 0, 0, NULL, 0x100);
          npc_spawn(264, npc->x - (16 * 0x200), npc->y, 0, 0, 0x400, NULL, 0x100);
        } else {
          npc_spawn(264, npc->x + (16 * 0x200), npc->y, 0, 0, 2, NULL, 0x100);
          npc_spawn(264, npc->x + (16 * 0x200), npc->y, 0, 0, 2 + 0x400, NULL, 0x100);
        }
      }

      if (npc->act_wait == 120) npc->anim = 0;

      if (npc->act_wait > 130 && npc->life < npc->count2 - 50) npc->act_wait = 161;

      if (npc->act_wait > 160) {
        npc->act = 100;
        npc->anim = 0;
      }

      break;

    case 30:
      npc->act = 31;
      npc->act_wait = 0;
      npc->anim = 6;
      npc->tgt_x = npc->x;
      npc->bits |= NPC_SHOOTABLE;
      // Fallthrough
    case 31:
      if (++npc->act_wait / 2 % 2)
        npc->x = npc->tgt_x;
      else
        npc->x = npc->tgt_x + 0x200;

      if (npc->act_wait > 50) {
        npc->act = 32;
        npc->act_wait = 0;
        npc->anim = 7;
        snd_play_sound(PRIO_NORMAL, 101, FALSE);

        for (deg = 8; deg < 0x100; deg += 0x10) {
          xvel = m_cos(deg) * 2;
          yvel = m_sin(deg) * 2;
          npc_spawn(266, npc->x, npc->y, xvel, yvel, 0, NULL, 0x100);
        }
      }

      break;

    case 32:
      if (++npc->act_wait > 50) npc->act = 100;

      break;

    case 100:
      npc->act = 101;
      npc->bits &= ~NPC_SHOOTABLE;
      npc->damage = 0;
      npc->act_wait = 0;
      snd_play_sound(PRIO_NORMAL, 29, FALSE);
      // Fallthrough
    case 101:
      npc->act_wait += 2;

      if (npc->act_wait > 16) {
        npc->act = 102;
        npc->act_wait = 0;
        npc->anim = 3;
        npc->tgt_x = m_rand(5, 35) * 0x200 * 0x10;
        npc->tgt_y = m_rand(5, 7) * 0x200 * 0x10;
      }

      break;

    case 102:
      if (++npc->act_wait > 40) {
        npc->act = 103;
        npc->act_wait = 16;
        npc->anim = 2;
        npc->yvel = 0;
        npc->x = npc->tgt_x;
        npc->y = npc->tgt_y;

        if (npc->x > player.x)
          npc->dir = 0;
        else
          npc->dir = 2;
      }

      break;

    case 103:
      npc->act_wait -= 2;

      if (npc->act_wait <= 0) {
        npc->bits |= NPC_SHOOTABLE;
        npc->damage = 3;

        if (npc->count1 < 3) {
          ++npc->count1;
          npc->act = 10;
        } else {
          npc->count1 = 0;
          npc->act = 30;
        }
      }

      break;

    case 500:
      npc->bits &= ~NPC_SHOOTABLE;
      npc->anim = 6;
      npc->yvel += 0x10;

      if (npc->flags & 8) {
        npc->act = 501;
        npc->act_wait = 0;
        npc->tgt_x = npc->x;

        if (npc->x > player.x)
          npc->dir = 0;
        else
          npc->dir = 2;
      }

      break;

    case 501:
      if (npc->x > player.x)
        npc->dir = 0;
      else
        npc->dir = 2;

      npc->anim = 8;

      if (++npc->act_wait / 2 % 2)
        npc->x = npc->tgt_x;
      else
        npc->x = npc->tgt_x + (1 * 0x200);

      break;
  }

  if (npc->act >= 10) {
    if (npc->act == 102) {
      npc_doctor_state.crystal_x = npc->tgt_x;
      npc_doctor_state.crystal_y = npc->tgt_y;
    } else {
      npc_doctor_state.crystal_x = npc->x;
      npc_doctor_state.crystal_y = npc->y;
    }
  }

  if (npc->yvel > 0x5FF) npc->yvel = 0x5FF;

  npc->x += npc->xvel;
  npc->y += npc->yvel;

  if (npc->dir == 0)
    npc->rect = &rc_left[npc->anim];
  else
    npc->rect = &rc_right[npc->anim];

  if (npc->act == 101 || npc->act == 103) {
    npc->rect_delta.top = npc->act_wait;
    npc->rect_delta.bottom = -npc->act_wait;
    npc->rect_prev = NULL; // force texrect update
    npc->view.top = (16 - npc->act_wait) * 0x200;
  } else {
    npc->view.top = 16 * 0x200;
  }
}

// Doctor red wave (projectile)
void npc_act_264(npc_t *npc) {
  static const rect_t rc = {288, 0, 304, 16};
  u8 deg;

  if (npc->x < 0 || npc->x > stage_data->width * 0x10 * 0x200) {
    npc_show_death_damage(npc);
    return;
  }

  switch (npc->act) {
    case 0:
      npc->act = 1;
      npc->tgt_x = npc->x;
      npc->tgt_y = npc->y;
      npc->count1 = npc->dir / 8;
      npc->dir %= 8;
      // Fallthrough
    case 1:
      npc->count1 += 6;
      npc->count1 %= 0x100;
      deg = npc->count1;

      if (npc->act_wait < 128) ++npc->act_wait;

      if (npc->dir == 0)
        npc->xvel -= 21;
      else
        npc->xvel += 21;

      npc->tgt_x += npc->xvel;

      npc->x = npc->tgt_x + (m_cos(deg) * npc->act_wait) / 2 / 4;
      npc->y = npc->tgt_y + (m_sin(deg) * npc->act_wait) / 2;

      npc_spawn(265, npc->x, npc->y, 0, 0, 0, NULL, 0x100);

      break;
  }

  npc->rect = &rc;
}

// Doctor red ball projectile
void npc_act_265(npc_t *npc) {
  static const rect_t rc[3] = {
    {288, 16, 304, 32},
    {288, 32, 304, 48},
    {288, 48, 304, 64},
  };

  if (++npc->anim_wait > 3) {
    npc->anim_wait = 0;
    ++npc->anim;
  }

  if (npc->anim > 2)
    npc->cond = 0;
  else
    npc->rect = &rc[npc->anim];
}

// Doctor red ball projectile (bouncing)
void npc_act_266(npc_t *npc) {
  static const rect_t rc[2] = {
      {304, 16, 320, 32},
      {304, 32, 320, 48},
  };

  if (npc->flags & 1) npc->xvel *= -1;
  if (npc->flags & 4) npc->xvel *= -1;

  if (npc->flags & 2) npc->yvel = 0x200;
  if (npc->flags & 8) npc->yvel = -0x200;

  npc->x += npc->xvel;
  npc->y += npc->yvel;

  if (++npc->anim > 1) npc->anim = 0;

  npc->rect = &rc[npc->anim];

  if (++npc->act_wait % 4 == 1) npc_spawn(265, npc->x, npc->y, 0, 0, 0, NULL, 0x100);

  if (npc->act_wait > 250) npc_show_death_damage(npc);
}

// Muscle Doctor
void npc_act_267(npc_t *npc) {
  static const rect_t rc_left[10] = {
    {0, 0, 0, 0},        {0, 64, 40, 112},    {40, 64, 80, 112},   {80, 64, 120, 112},  {120, 64, 160, 112},
    {160, 64, 200, 112}, {200, 64, 240, 112}, {240, 64, 280, 112}, {280, 64, 320, 112}, {0, 160, 40, 208},
  };

  static const rect_t rc_right[10] = {
    {0, 0, 0, 0},         {0, 112, 40, 160},    {40, 112, 80, 160},   {80, 112, 120, 160},  {120, 112, 160, 160},
    {160, 112, 200, 160}, {200, 112, 240, 160}, {240, 112, 280, 160}, {280, 112, 320, 160}, {40, 160, 80, 208},
  };

  int xvel, yvel;
  int i;

  switch (npc->act) {
    case 0:
      if (npc_doctor_state.crystal_x > player.x)
        npc->dir = 0;
      else
        npc->dir = 2;

      if (npc->dir == 0)
        npc->x = npc_doctor_state.crystal_x - (6 * 0x200);
      else
        npc->x = npc_doctor_state.crystal_x + (6 * 0x200);

      npc->y = npc_doctor_state.crystal_y;
      // Fallthrough
    case 1:
      npc->act = 2;
      // Fallthrough
    case 2:
      npc->yvel += 0x80;

      if (++npc->act_wait / 2 % 2)
        npc->anim = 0;
      else
        npc->anim = 3;

      break;

    case 5:
      npc->act = 6;
      npc->anim = 1;
      npc->anim_wait = 0;
      // Fallthrough
    case 6:
      npc->yvel += 0x80;

      if (++npc->anim_wait > 40) {
        npc->anim_wait = 0;
        ++npc->anim;
      }

      if (npc->anim > 2) npc->anim = 1;

      break;

    case 7:
      npc->act = 8;
      npc->act_wait = 0;
      npc->anim = 3;
      // Fallthrough
    case 8:
      npc->yvel += 0x40;

      if (++npc->act_wait > 40) npc->act = 10;

      break;

    case 10:
      npc->bits |= NPC_INVULNERABLE;
      npc->xvel = 0;
      npc->act = 11;
      npc->act_wait = 0;
      npc->anim = 1;
      npc->anim_wait = 0;
      npc->count2 = npc->life;
      // Fallthrough
    case 11:
      npc->yvel += 0x80;

      if (npc->x > player.x)
        npc->dir = 0;
      else
        npc->dir = 2;

      if (npc->flags & 8) {
        if (npc->life < npc->count2 - 20) {
          if (player.flags & 8 && player.x > npc->x - (48 * 0x200) && player.x < npc->x + (48 * 0x200) &&
              npc->anim != 6) {
            npc->anim = 6;
            plr_damage(5);
            cam_start_quake_small(10);
            snd_play_sound(PRIO_NORMAL, 26, FALSE);
            player.yvel = -0x400;

            if (npc->x > player.x)
              player.xvel = -0x5FF;
            else
              player.xvel = 0x5FF;

            for (i = 0; i < 100; ++i)
              npc_spawn(270, npc->x + (m_rand(-0x10, 0x10) * 0x200), npc->y + (m_rand(-0x10, 0x10) * 0x200),
                m_rand(-0x200, 0x200) * 3, m_rand(-0x200, 0x200) * 3, 3, NULL, 0xAA);
          }
        } else {
          if (++npc->anim_wait > 10) {
            npc->anim_wait = 0;

            if (++npc->anim > 2) npc->anim = 1;
          }
        }
      } else {
        npc->anim = 4;
      }

      if (++npc->act_wait > 30 || npc->life < npc->count2 - 20) {
        if (++npc->count1 > 10) npc->count1 = 0;

        switch (npc->count1) {
          case 8:
            npc->act = 20;
            break;

          case 2:
          case 7:
            npc->act = 100;
            break;

          case 3:
          case 6:
            npc->act = 30;
            break;

          case 1:
          case 9:
            npc->act = 40;
            break;

          default:
            npc->act = 15;
            npc->act_wait = 0;
            break;
        }
      }

      break;

    case 15:
      npc->anim = 3;
      ++npc->act_wait;

      if (npc->x > player.x)
        npc->dir = 0;
      else
        npc->dir = 2;

      if (npc->act_wait > 20) {
        npc->act = 16;
        npc->anim = 4;
        npc->anim_wait = 0;
        npc->yvel = -0x600;

        if (npc->dir == 0)
          npc->xvel = -0x400;
        else
          npc->xvel = 0x400;
      }

      break;

    case 16:
      npc->yvel += 0x40;

      if (++npc->anim_wait > 1) {
        npc->anim_wait = 0;
        ++npc->anim;
      }

      if (npc->anim > 5) npc->anim = 4;

      if (npc->x > player.x)
        npc->dir = 0;
      else
        npc->dir = 2;

      if (npc->yvel > 0 && npc->flags & 8) npc->act = 17;

      break;

    case 17:
      npc->act = 18;
      npc->act_wait = 0;
      cam_start_quake_small(10);
      snd_play_sound(PRIO_NORMAL, 26, FALSE);
      // Fallthrough
    case 18:
      npc->anim = 3;
      ++npc->act_wait;

      npc->xvel = (npc->xvel * 7) / 8;
      npc->yvel += 0x80;

      if (npc->act_wait > 10) npc->act = 10;

      break;

    case 20:
      npc->act = 21;
      npc->act_wait = 0;
      // Fallthrough
    case 21:
      ++npc->act_wait;
      npc->anim = 6;

      if (npc->act_wait > 20 && npc->act_wait % 3 == 1) {
        yvel = m_rand(-0x200, 0x200);
        xvel = m_rand(0x100, 0x200) * 4;

        if (npc->dir == 0)
          npc_spawn(269, npc->x - 0x1000, npc->y - 0x800, -xvel, yvel, 0, NULL, 0x100);
        else
          npc_spawn(269, npc->x + 0x1000, npc->y - 0x800, xvel, yvel, 2, NULL, 0x100);

        snd_play_sound(PRIO_NORMAL, 39, FALSE);
      }

      if (npc->act_wait > 90) npc->act = 10;

      break;

    case 30:
      npc->act = 31;
      npc->act_wait = 0;
      npc->bits |= NPC_SOLID_SOFT;
      npc->bits &= ~NPC_SHOOTABLE;
      // Fallthrough
    case 31:
      npc->anim = 3;

      if (++npc->act_wait > 20) {
        npc->act = 32;
        npc->act_wait = 0;
        npc->anim = 7;
        npc->bits |= NPC_REAR_AND_TOP_DONT_HURT;
        npc->damage = 10;
        snd_play_sound(PRIO_NORMAL, 25, FALSE);

        if (npc->dir == 0)
          npc->xvel = -0x5FF;
        else
          npc->xvel = 0x5FF;
      }

      break;

    case 32:
      ++npc->act_wait;
      npc->yvel = 0;

      if (npc->act_wait / 2 % 2)
        npc->anim = 7;
      else
        npc->anim = 8;

      if (npc->act_wait > 30) {
        npc->act_wait = 0;
        npc->act = 18;
        npc->damage = 5;
        npc->bits &= ~(NPC_SOLID_SOFT | NPC_REAR_AND_TOP_DONT_HURT);
        npc->bits |= NPC_SHOOTABLE;
      }

      if (npc->flags & 1 || npc->flags & 4) {
        npc->act = 15;
        npc->act_wait = 0;
        npc->damage = 5;
        npc->bits &= ~(NPC_SOLID_SOFT | NPC_REAR_AND_TOP_DONT_HURT);
        npc->bits |= NPC_SHOOTABLE;
      }

      break;

    case 40:
      npc->anim = 3;
      ++npc->act_wait;

      if (npc->x > player.x)
        npc->dir = 0;
      else
        npc->dir = 2;

      if (npc->act_wait > 20) {
        npc->act = 41;
        npc->anim = 4;
        npc->anim_wait = 0;
        npc->yvel = -0x800;

        if (npc->dir == 0)
          npc->xvel = -0x400;
        else
          npc->xvel = 0x400;
      }

      break;

    case 41:
      npc->yvel += 0x40;

      if (++npc->anim_wait > 1) {
        npc->anim_wait = 0;
        ++npc->anim;
      }

      if (npc->anim > 5) npc->anim = 4;

      if (player.y > npc->y && player.x > npc->x - (8 * 0x200) && player.x < npc->x + (8 * 0x200)) {
        npc->act = 16;
        npc->yvel = 0x5FF;
        npc->xvel = 0;
      }

      if (npc->yvel > 0 && npc->flags & 8) npc->act = 17;

      break;

    case 100:
      npc->act = 101;
      npc->act_wait = 0;
      npc->bits &= ~(NPC_INVULNERABLE | NPC_SHOOTABLE);
      npc->damage = 0;
      snd_play_sound(PRIO_NORMAL, 29, FALSE);
      // Fallthrough
    case 101:
      npc->act_wait += 2;

      if (npc->act_wait > 28) {
        npc->act = 102;
        npc->act_wait = 0;
        npc->anim = 0;

        npc->tgt_x = player.x;
        npc->tgt_y = player.y - (32 * 0x200);

        if (npc->tgt_y < (64 * 0x200)) npc->tgt_y = (64 * 0x200);

        if (npc->tgt_x < (64 * 0x200)) npc->tgt_x = (64 * 0x200);
        if (npc->tgt_x > (576 * 0x200)) npc->tgt_x = (576 * 0x200);
      }

      break;

    case 102:
      if (++npc->act_wait > 40) {
        npc->act = 103;
        npc->act_wait = 28;
        npc->anim = 4;
        npc->yvel = 0;
        npc->x = npc->tgt_x;
        npc->y = npc->tgt_y;

        if (npc->x > player.x)
          npc->dir = 0;
        else
          npc->dir = 2;
      }

      break;

    case 103:
      npc->act_wait -= 2;

      if (npc->act_wait <= 0) {
        npc->bits |= (NPC_INVULNERABLE | NPC_SHOOTABLE);
        npc->damage = 5;
        npc->act = 16;
        npc->yvel = -0x200;
        npc->xvel = 0;
      }

      break;

    case 500:
      npc_delete_by_class(269, TRUE);
      npc->bits &= ~NPC_SHOOTABLE;
      npc->anim = 4;
      npc->yvel += 0x20;
      npc->xvel = 0;

      if (npc->flags & 8) {
        npc->act = 501;
        npc->act_wait = 0;
        npc->tgt_x = npc->x;

        if (npc->x > player.x)
          npc->dir = 0;
        else
          npc->dir = 2;
      }

      break;

    case 501:
      npc->anim = 9;

      if (++npc->act_wait / 2 % 2)
        npc->x = npc->tgt_x;
      else
        npc->x = npc->tgt_x + (1 * 0x200);

      break;

    case 510:
      npc->act = 511;
      npc->act_wait = 0;
      npc->anim = 9;
      npc->tgt_x = npc->x;
      npc->y += 16 * 0x200;
      npc->bits |= NPC_IGNORE_SOLIDITY;
      // Fallthrough
    case 511:
      cam_start_quake_small(2);

      if (++npc->act_wait % 6 == 3) snd_play_sound(PRIO_NORMAL, 25, FALSE);

      if (npc->act_wait / 2 % 2)
        npc->x = npc->tgt_x;
      else
        npc->x = npc->tgt_x + (1 * 0x200);

      if (npc->act_wait > 352) {
        npc->anim = 0;
        npc->act = 0x200;
      }

      break;

    case 520:
      npc->damage = 0;
      npc_doctor_state.crystal_y = -32 * 0x200;
      break;
  }

  if (npc->act >= 11 && npc->act < 501) {
    if (npc->act == 102) {
      npc_doctor_state.crystal_x = npc->tgt_x;
      npc_doctor_state.crystal_y = npc->tgt_y;
    } else {
      npc_doctor_state.crystal_x = npc->x;
      npc_doctor_state.crystal_y = npc->y;
    }
  }

  if (npc->yvel > 0x5FF) npc->yvel = 0x5FF;

  npc->x += npc->xvel;
  npc->y += npc->yvel;

  if (npc->act < 510) {
    if (npc->act != 102 && npc->act != 103 && m_rand(0, 3) == 2)
      npc_spawn(270, npc->x + (m_rand(-0x10, 0x10) * 0x200), npc->y + (m_rand(-8, 4) * 0x200), npc->xvel, 0, 3, 0,
                0x100);
  } else if (npc->act < 512) {
    npc_spawn(270, npc->x + (m_rand(-0x10, 0x10) * 0x200), npc->y - (((336 - npc->act_wait) / 8) * 0x200),
      m_rand(-0x200, 0x200), m_rand(-0x200, 0) * 2, 3, NULL, 0xAA);
    npc_spawn(270, npc->x + (m_rand(-0x10, 0x10) * 0x200), npc->y - (((336 - npc->act_wait) / 8) * 0x200),
      m_rand(-0x200, 0x200), m_rand(-0x200, 0) * 2, 3, NULL, 0xAA);
    npc_spawn(270, npc->x + (m_rand(-0x10, 0x10) * 0x200), npc->y - (((336 - npc->act_wait) / 8) * 0x200), 0,
      2 * m_rand(-0x200, 0), 3, NULL, 0xAA);
    npc_spawn(270, npc->x + (m_rand(-0x10, 0x10) * 0x200), npc->y - (((336 - npc->act_wait) / 8) * 0x200), 0,
      2 * m_rand(-0x200, 0), 3, NULL, 0xAA);
  }

  if (npc->dir == 0)
    npc->rect = &rc_left[npc->anim];
  else
    npc->rect = &rc_right[npc->anim];

  if (npc->act == 511) {
    npc->rect_delta.top = npc->act_wait / 8;
    npc->view.top = (44 - (npc->act_wait / 8)) * 0x200;
    npc->view.bottom = 4 * 0x200;
    npc->rect_prev = NULL; // force texrect update
  } else if (npc->act == 101 || npc->act == 103) {
    npc->rect_delta.top = npc->act_wait;
    npc->rect_delta.bottom = -npc->act_wait;
    npc->view.top = (28 - npc->act_wait) * 0x200;
    npc->rect_prev = NULL; // force texrect update
  } else {
    npc->view.top = 28 * 0x200;
  }
}

// Igor (enemy)
void npc_act_268(npc_t *npc) {
  u8 deg;
  int xvel, yvel;

  static const rect_t rc_left[10] = {
    {0, 0, 40, 40}, {40, 0, 80, 40},   {80, 0, 120, 40}, {0, 0, 40, 40},    {120, 0, 160, 40},
    {0, 0, 40, 40}, {40, 80, 80, 120}, {0, 80, 40, 120}, {240, 0, 280, 40}, {280, 0, 320, 40},
  };

  static const rect_t rc_right[10] = {
    {0, 40, 40, 80}, {40, 40, 80, 80},    {80, 40, 120, 80},   {0, 40, 40, 80},    {120, 40, 160, 80},
    {0, 40, 40, 80}, {160, 80, 200, 120}, {120, 80, 160, 120}, {240, 40, 280, 80}, {280, 40, 320, 80},
  };

  if (npc->x < player.x - (320 * 0x200) || npc->x > player.x + (320 * 0x200) || npc->y < player.y - (240 * 0x200) ||
      npc->y > player.y + (240 * 0x200))
    npc->act = 1;

  switch (npc->act) {
    case 0:
      npc->act = 1;
      npc->y += 8 * 0x200;
      // Fallthrough
    case 1:
      if (++npc->anim_wait > 20) {
        npc->anim_wait = 0;
        ++npc->anim;
      }

      if (npc->anim > 1) npc->anim = 0;

      if (npc->x < player.x + (112 * 0x200) && npc->x > player.x - (112 * 0x200) && npc->y < player.y + (48 * 0x200) &&
          npc->y > player.y - (112 * 0x200))
        npc->act = 10;

      if (npc->shock) npc->act = 10;

      break;

    case 10:
      npc->act = 11;
      npc->act_wait = 0;
      npc->anim = 0;
      npc->anim_wait = 0;

      if (player.x < npc->x)
        npc->dir = 0;
      else
        npc->dir = 2;
      // Fallthrough
    case 11:
      if (npc->dir == 0)
        npc->xvel = -0x200;
      else
        npc->xvel = 0x200;

      if (npc->x < player.x + (64 * 0x200) && npc->x > player.x - (64 * 0x200)) {
        npc->act = 20;
        npc->act_wait = 0;
      }

      if (npc->xvel < 0 && npc->flags & 1) {
        npc->act = 20;
        npc->act_wait = 0;
      }

      if (npc->xvel > 0 && npc->flags & 4) {
        npc->act = 20;
        npc->act_wait = 0;
      }

      if (++npc->anim_wait > 4) {
        npc->anim_wait = 0;
        ++npc->anim;
      }

      if (npc->anim > 5) npc->anim = 2;

      break;

    case 20:
      npc->xvel = 0;
      npc->anim = 6;

      if (++npc->act_wait > 10) {
        npc->act = 30;
        npc->yvel = -0x5FF;

        if (npc->dir == 0)
          npc->xvel = -0x200;
        else
          npc->xvel = 0x200;

        snd_play_sound(PRIO_NORMAL, 108, FALSE);
      }

      break;

    case 30:
      npc->anim = 7;

      if (npc->flags & 8) {
        npc->act = 40;
        npc->act_wait = 0;
        cam_start_quake_small(20);
        snd_play_sound(PRIO_NORMAL, 26, FALSE);
      }

      break;

    case 40:
      npc->xvel = 0;
      npc->anim = 6;

      if (++npc->act_wait > 30) npc->act = 50;

      break;

    case 50:
      npc->act = 51;
      npc->act_wait = 0;

      if (player.x < npc->x)
        npc->dir = 0;
      else
        npc->dir = 2;
      // Fallthrough
    case 51:
      if (++npc->act_wait > 30 && npc->act_wait % 4 == 1) {
        if (npc->dir == 0)
          deg = 0x88;
        else
          deg = 0xF8;

        deg += (u8)m_rand(-0x10, 0x10);
        yvel = m_sin(deg) * 5;
        xvel = m_cos(deg) * 5;
        npc_spawn(11, npc->x, npc->y + (4 * 0x200), xvel, yvel, 0, NULL, 0x100);
        snd_play_sound(PRIO_NORMAL, 12, FALSE);
      }

      if (npc->act_wait < 50 && npc->act_wait / 2 % 2)
        npc->anim = 9;
      else
        npc->anim = 8;

      if (npc->act_wait > 82) {
        npc->act = 10;

        if (player.x < npc->x)
          npc->dir = 0;
        else
          npc->dir = 2;
      }

      break;
  }

  npc->yvel += 0x200 / 10;
  if (npc->yvel > 0x5FF) npc->yvel = 0x5FF;

  npc->x += npc->xvel;
  npc->y += npc->yvel;

  if (npc->dir == 0)
    npc->rect = &rc_left[npc->anim];
  else
    npc->rect = &rc_right[npc->anim];
}

// Red Bat (bouncing)
void npc_act_269(npc_t *npc) {
  static const rect_t rc_left[3] = {
    {232, 0, 248, 16},
    {248, 0, 264, 16},
    {248, 16, 264, 32},
  };

  static const rect_t rc_right[3] = {
    {232, 32, 248, 48},
    {248, 32, 264, 48},
    {248, 48, 264, 64},
  };

  switch (npc->act) {
    case 0:
      npc->act = 1;
      npc->xvel2 = npc->xvel;
      npc->yvel2 = npc->yvel;
      // Fallthrough
    case 1:
      if (npc->xvel2 < 0 && npc->flags & 1) {
        npc->dir = 2;
        npc->xvel2 *= -1;
      } else if (npc->xvel2 > 0 && npc->flags & 4) {
        npc->dir = 0;
        npc->xvel2 *= -1;
      } else if (npc->yvel2 < 0 && npc->flags & 2) {
        npc->yvel2 *= -1;
      } else if (npc->yvel2 > 0 && npc->flags & 8) {
        npc->yvel2 *= -1;
      }

      npc->x += npc->xvel2;
      npc->y += npc->yvel2;

      if (++npc->anim_wait > 2) {
        npc->anim_wait = 0;
        ++npc->anim;
      }

      if (npc->anim > 2) npc->anim = 0;

      break;
  }

  if (npc->dir == 0)
    npc->rect = &rc_left[npc->anim];
  else
    npc->rect = &rc_right[npc->anim];
}

// Doctor's blood (or """"red energy"""")
void npc_act_270(npc_t *npc) {
  static const rect_t rc[2] = {
    {170, 34, 174, 38},
    {170, 42, 174, 46},
  };

  if (npc->dir == 3 || npc->dir == 1) {
    if (npc->dir == 3) npc->yvel += 0x40;
    if (npc->dir == 1) npc->yvel -= 0x40;

    ++npc->act_wait;

    if (npc->yvel > 0x5FF) npc->yvel = 0x5FF;

    npc->x += npc->xvel;
    npc->y += npc->yvel;

    if (npc->act_wait > 50) npc->cond = 0;

    if (npc->flags & 0xFF) npc->cond = 0;
  } else if (npc->dir == 2) {
    switch (npc->act) {
      case 0:
        npc->act = 1;
        npc->bits |= NPC_IGNORE_SOLIDITY;

        npc->xvel = m_rand(-0x200, 0x200) * 3;
        npc->yvel = m_rand(-0x200, 0x200) * 3;

        npc->count1 = m_rand(0x10, 0x33);
        npc->count2 = m_rand(0x80, 0x100);

        break;
    }

    if (npc->x < npc->parent->x) npc->xvel += 0x200 / npc->count1;
    if (npc->x > npc->parent->x) npc->xvel -= 0x200 / npc->count1;

    if (npc->y < npc->parent->y) npc->yvel += 0x200 / npc->count1;
    if (npc->y > npc->parent->y) npc->yvel -= 0x200 / npc->count1;

    if (npc->xvel > npc->count2 * 2) npc->xvel = npc->count2 * 2;
    if (npc->xvel < -npc->count2 * 2) npc->xvel = -npc->count2 * 2;

    if (npc->yvel > npc->count2 * 3) npc->yvel = npc->count2 * 3;
    if (npc->yvel < -npc->count2 * 3) npc->yvel = -npc->count2 * 3;

    npc->x += npc->xvel;
    npc->y += npc->yvel;
  }

  npc->rect = &rc[m_rand(0, 1)];
}

// Ironhead block
void npc_act_271(npc_t *npc) {
  static const rect_t rc[] = {
    {112, 0,  128, 16}, {128, 0,  144, 16}, {144, 0,  160, 16},
    {112, 16, 128, 32}, {128, 16, 144, 32}, {144, 16, 160, 32},
    {112, 32, 128, 48}, {128, 32, 144, 48}, {144, 32, 160, 48},
    {0x00, 0x40, 0x20, 0x60}
  };

  int a;

  if (npc->xvel < 0 && npc->x < -16 * 0x200) {
    npc_show_death_damage(npc);
    return;
  }

  if (npc->xvel > 0 && npc->x > (stage_data->width * 0x200 * 0x10) + (1 * 0x200 * 0x10)) {
    npc_show_death_damage(npc);
    return;
  }

  if (npc->act == 0) {
    npc->act = 1;

    a = m_rand(0, 9);

    npc->rect = &rc[a];

    if (a == 9) {
      npc->view.front = 16 * 0x200;
      npc->view.back = 16 * 0x200;
      npc->view.top = 16 * 0x200;
      npc->view.bottom = 16 * 0x200;

      npc->hit.front = 12 * 0x200;
      npc->hit.back = 12 * 0x200;
      npc->hit.top = 12 * 0x200;
      npc->hit.bottom = 12 * 0x200;
    }

    if (npc->dir == 0)
      npc->xvel = m_rand(0x100, 0x200) * -2;
    else
      npc->xvel = m_rand(0x100, 0x200) * 2;

    npc->yvel = m_rand(-0x200, 0x200);
  }

  if (npc->yvel < 0 && npc->y - npc->hit.top < 8 * 0x200) {
    npc->yvel *= -1;
    caret_spawn(npc->x, npc->y - (8 * 0x200), CARET_TINY_PARTICLES, DIR_LEFT);
    caret_spawn(npc->x, npc->y - (8 * 0x200), CARET_TINY_PARTICLES, DIR_LEFT);
  }

  if (npc->yvel > 0 && npc->y + npc->hit.bottom > 232 * 0x200) {
    npc->yvel *= -1;
    caret_spawn(npc->x, npc->y + (8 * 0x200), CARET_TINY_PARTICLES, DIR_LEFT);
    caret_spawn(npc->x, npc->y + (8 * 0x200), CARET_TINY_PARTICLES, DIR_LEFT);
  }

  npc->x += npc->xvel;
  npc->y += npc->yvel;
}

// Ironhead block generator
void npc_act_272(npc_t *npc) {
  switch (npc->act) {
    case 0:
      npc->act = 1;
      npc->act_wait = m_rand(0, 200);
      // Fallthrough
    case 1:
      if (npc->act_wait) {
        --npc->act_wait;
      } else {
        npc->act = 0;
        npc_spawn(271, npc->x, npc->y + (m_rand(-32, 32) * 0x200), 0, 0, npc->dir, NULL, 0x100);
      }

      break;
  }
}

// Droll projectile
void npc_act_273(npc_t *npc) {
  static const rect_t rc[3] = {
    {248, 40, 272, 64},
    {272, 40, 296, 64},
    {296, 40, 320, 64},
  };

  switch (npc->act) {
    case 0:
      npc->act = 1;
      // Fallthrough
    case 1:
      npc->x += npc->xvel;
      npc->y += npc->yvel;

      if (npc->flags & 0xFF) {
        npc_spawn(4, npc->x, npc->y, 0, 0, 0, NULL, 0x100);
        npc_spawn(4, npc->x, npc->y, 0, 0, 0, NULL, 0x100);
        npc_spawn(4, npc->x, npc->y, 0, 0, 0, NULL, 0x100);
        npc_show_death_damage(npc);
        return;
      }

      if (++npc->act_wait % 5 == 0) snd_play_sound(PRIO_NORMAL, 110, FALSE);

      if (++npc->anim > 2) npc->anim = 0;

      break;
  }

  npc->rect = &rc[npc->anim];
}

// Droll
void npc_act_274(npc_t *npc) {
  static const rect_t rc_left[6] = {
    {0, 0, 32, 40},    {32, 0, 64, 40},    {64, 0, 96, 40},
    {64, 80, 96, 120}, {96, 80, 128, 120}, {96, 0, 128, 40},
  };

  static const rect_t rc_right[6] = {
    {0, 40, 32, 80},    {32, 40, 64, 80},    {64, 40, 96, 80},
    {64, 120, 96, 160}, {96, 120, 128, 160}, {96, 40, 128, 80},
  };

  u8 deg;
  int xvel, yvel;

  switch (npc->act) {
    case 0:
      npc->act = 1;
      npc->y -= 8 * 0x200;
      npc->tgt_x = npc->x;
      // Fallthrough
    case 1:
      npc->xvel = 0;
      npc->act = 2;
      npc->anim = 0;
      // Fallthrough
    case 2:
      if (npc->x > player.x)
        npc->dir = 0;
      else
        npc->dir = 2;

      if (++npc->anim_wait > 40) {
        npc->anim_wait = 0;
        ++npc->anim;
      }

      if (npc->anim > 1) npc->anim = 0;

      if (npc->shock) npc->act = 10;

      break;

    case 10:
      npc->act = 11;
      npc->anim = 2;
      npc->act_wait = 0;
      // Fallthrough
    case 11:
      if (++npc->act_wait > 10) {
        npc->act = 12;
        npc->anim = 3;
        npc->yvel = -0x600;
        npc->count1 = 0;

        if (npc->tgt_x > npc->x)
          npc->xvel = 0x200;
        else
          npc->xvel = -0x200;
      }

      break;

    case 12:
      if (npc->yvel > 0) {
        npc->anim = 4;

        if (npc->count1 == 0) {
          ++npc->count1;
          deg = m_atan2(npc->x - player.x, npc->y - (10 * 0x200) - player.y);
          yvel = m_sin(deg) * 4;
          xvel = m_cos(deg) * 4;
          npc_spawn(273, npc->x, npc->y - (10 * 0x200), xvel, yvel, 0, NULL, 0x100);
          snd_play_sound(PRIO_NORMAL, 39, FALSE);
        }
      }

      if (npc->yvel > 0x200) npc->anim = 5;

      if (npc->flags & 8) {
        npc->anim = 2;
        npc->act = 13;
        npc->act_wait = 0;
        npc->xvel = 0;
      }

      break;

    case 13:
      npc->xvel /= 2;

      if (++npc->act_wait > 10) npc->act = 1;

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

// Puppy (plantation)
void npc_act_275(npc_t *npc) {
  static const rect_t rc_right[4] = {
      {272, 80, 288, 96},
      {288, 80, 304, 96},
      {272, 80, 288, 96},
      {304, 80, 320, 96},
  };

  switch (npc->act) {
    case 0:
      npc->act = 1;
      npc->anim = 0;
      npc->anim_wait = 0;
      // Fallthrough
    case 1:
      if (m_rand(0, 120) == 10) {
        npc->act = 2;
        npc->act_wait = 0;
        npc->anim = 1;
      }

      if (npc->x - (64 * 0x200) < player.x && npc->x + (64 * 0x200) > player.x && npc->y - (32 * 0x200) < player.y &&
          npc->y + (16 * 0x200) > player.y) {
        if (++npc->anim_wait > 3) {
          npc->anim_wait = 0;
          ++npc->anim;
        }

        if (npc->anim > 3) npc->anim = 2;
      }

      break;

    case 2:
      if (++npc->act_wait > 8) {
        npc->act = 1;
        npc->anim = 0;
      }

      break;
  }

  npc->yvel += 0x40;
  if (npc->yvel > 0x5FF) npc->yvel = 0x5FF;

  npc->x += npc->xvel;
  npc->y += npc->yvel;

  npc->rect = &rc_right[npc->anim];
}

// Red Demon
void npc_act_276(npc_t *npc) {
  static const rect_t rc_left[9] = {
    {0, 64, 32, 104},    {32, 64, 64, 104},   {64, 64, 96, 104},   {96, 64, 128, 104},  {128, 64, 160, 104},
    {160, 64, 192, 104}, {192, 64, 224, 104}, {224, 64, 256, 104}, {256, 64, 288, 104},
  };

  static const rect_t rc_right[9] = {
    {0, 104, 32, 144},    {32, 104, 64, 144},   {64, 104, 96, 144},   {96, 104, 128, 144},  {128, 104, 160, 144},
    {160, 104, 192, 144}, {192, 104, 224, 144}, {224, 104, 256, 144}, {256, 104, 288, 144},
  };

  u8 deg;
  int xvel, yvel;

  switch (npc->act) {
    case 0:
      npc->act = 1;
      npc->y -= 8 * 0x200;
      // Fallthrough
    case 1:
      npc->xvel = 0;
      npc->act = 2;
      npc->anim = 0;
      // Fallthrough
    case 2:
      if (npc->x > player.x)
        npc->dir = 0;
      else
        npc->dir = 2;

      if (++npc->anim_wait > 20) {
        npc->anim_wait = 0;
        ++npc->anim;
      }

      if (npc->anim > 1) npc->anim = 0;

      if (npc->shock) npc->act = 10;

      break;

    case 10:
      npc->act = 11;
      npc->act_wait = 0;
      npc->anim = 3;
      npc->bits |= NPC_SHOOTABLE;
      // Fallthrough
    case 11:
      switch (++npc->act_wait) {
        case 30:
        case 40:
        case 50:
          npc->anim = 4;

          deg = m_atan2(npc->x - player.x, npc->y - player.y);
          yvel = m_sin(deg) * 4;
          xvel = m_cos(deg) * 4;

          npc_spawn(277, npc->x, npc->y, xvel, yvel, 0, NULL, 0x100);
          snd_play_sound(PRIO_NORMAL, 39, FALSE);

          break;

        case 34:
        case 44:
        case 54:
          npc->anim = 3;
          break;
      }

      if (npc->act_wait > 60) {
        npc->act = 20;
        npc->act_wait = 0;
        npc->anim = 2;
      }

      break;

    case 20:
      if (++npc->act_wait > 20) {
        npc->act = 21;
        npc->act_wait = 0;
        npc->anim = 5;
        npc->yvel = -0x5FF;

        if (npc->x < player.x)
          npc->xvel = 0x100;
        else
          npc->xvel = -0x100;
      }

      break;

    case 21:
      switch (++npc->act_wait) {
        case 30:
        case 40:
        case 50:
          npc->anim = 6;

          deg = m_atan2(npc->x - player.x, npc->y - (10 * 0x200) - player.y);
          yvel = m_sin(deg) * 4;
          xvel = m_cos(deg) * 4;

          npc_spawn(277, npc->x, npc->y - (10 * 0x200), xvel, yvel, 0, NULL, 0x100);
          snd_play_sound(PRIO_NORMAL, 39, FALSE);

          break;

        case 34:
        case 44:
          npc->anim = 5;
          break;
      }

      if (npc->act_wait > 53) npc->anim = 7;

      if (npc->flags & 8) {
        npc->act = 22;
        npc->act_wait = 0;
        npc->anim = 2;
        cam_start_quake_small(10);
        snd_play_sound(PRIO_NORMAL, 26, FALSE);
      }

      break;

    case 22:
      npc->xvel /= 2;

      if (++npc->act_wait > 22) npc->act = 10;

      break;

    case 50:
      npc->bits &= ~NPC_SHOOTABLE;
      npc->damage = 0;

      if (npc->flags & 8) {
        npc->act = 51;
        npc->anim = 2;
        cam_start_quake_small(10);
        npc_spawn_exp(npc->x, npc->y, 19);
        npc_spawn_death_fx(npc->x, npc->y, npc->view.back, 8, 0);
        snd_play_sound(PRIO_NORMAL, 72, FALSE);
      }

      break;

    case 51:
      npc->xvel = (npc->xvel * 7) / 8;
      npc->anim = 8;
      break;
  }

  npc->yvel += 0x20;
  if (npc->yvel > 0x5FF) npc->yvel = 0x5FF;

  npc->x += npc->xvel;
  npc->y += npc->yvel;

  if (npc->act < 50) {
    if (npc->x < player.x)
      npc->dir = 2;
    else
      npc->dir = 0;
  }

  if (npc->dir == 0)
    npc->rect = &rc_left[npc->anim];
  else
    npc->rect = &rc_right[npc->anim];
}

// Red Demon projectile
void npc_act_277(npc_t *npc) {
  static const rect_t rc[3] = {
    {128, 0, 152, 24},
    {152, 0, 176, 24},
    {176, 0, 200, 24},
  };

  switch (npc->act) {
    case 0:
      npc->act = 1;
      // Fallthrough
    case 1:
      npc->x += npc->xvel;
      npc->y += npc->yvel;

      if (npc->flags & 0xFF) {
        npc_spawn(4, npc->x, npc->y, 0, 0, 0, NULL, 0x100);
        npc_spawn(4, npc->x, npc->y, 0, 0, 0, NULL, 0x100);
        npc_spawn(4, npc->x, npc->y, 0, 0, 0, NULL, 0x100);
        npc_show_death_damage(npc);
        return;
      }

      if (++npc->act_wait % 5 == 0) snd_play_sound(PRIO_NORMAL, 110, FALSE);

      if (++npc->anim > 2) npc->anim = 0;

      break;
  }

  npc->rect = &rc[npc->anim];
}

// Little family
void npc_act_278(npc_t *npc) {
  static const rect_t rc_papa[2] = {
    {0, 120, 8, 128},
    {8, 120, 16, 128},
  };

  static const rect_t rc_mama[2] = {
    {16, 120, 24, 128},
    {24, 120, 32, 128},
  };

  static const rect_t rc_kodomo[2] = {
    {32, 120, 40, 128},
    {40, 120, 48, 128},
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
      npc->anim = 0;
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
        npc->xvel = -0x100;
      else
        npc->xvel = 0x100;

      if (++npc->anim_wait > 4) {
        npc->anim_wait = 0;
        ++npc->anim;
      }

      if (npc->anim > 1) npc->anim = 0;

      if (++npc->act_wait > 0x20) npc->act = 0;

      break;
  }

  npc->yvel += 0x20;
  if (npc->yvel > 0x5FF) npc->yvel = 0x5FF;

  npc->x += npc->xvel;
  npc->y += npc->yvel;

  switch (npc->event_num) {
    case 200:
      npc->rect = &rc_papa[npc->anim];
      break;

    case 210:
      npc->rect = &rc_mama[npc->anim];
      break;

    default:
      npc->rect = &rc_kodomo[npc->anim];
      break;
  }
}

// Falling block (large)
void npc_act_279(npc_t *npc) {
  static const rect_t rc[2] = {
      {0, 16, 32, 48},
      {16, 0, 32, 16},
  };

  int i;

  switch (npc->act) {
    case 0:
      switch (npc->dir) {
        case 0:
          npc->act = 100;
          npc->bits |= NPC_INVULNERABLE;
          npc->anim = 0;
          break;

        case 2:
          npc->act = 100;
          npc->bits |= NPC_INVULNERABLE;
          npc->anim = 1;

          npc->view.back = 8 * 0x200;
          npc->view.front = 8 * 0x200;
          npc->view.top = 8 * 0x200;
          npc->view.bottom = 8 * 0x200;

          npc->hit.back = 8 * 0x200;
          npc->hit.front = 8 * 0x200;
          npc->hit.top = 8 * 0x200;
          npc->hit.bottom = 8 * 0x200;

          break;

        case 1:
          npc->anim = 0;
          npc->act = 10;
          break;
      }

      if (npc->dir != 1) break;
      // Fallthrough
    case 10:
      npc->act = 11;
      npc->act_wait = 16;
      // Fallthrough
    case 11:
      npc->act_wait -= 2;

      if (npc->act_wait <= 0) {
        npc->act = 100;
        npc->bits |= NPC_INVULNERABLE;
      }

      break;

    case 100:
      npc->yvel += 0x40;
      if (npc->yvel > 0x700) npc->yvel = 0x700;

      if (npc->y > 128 * 0x200) npc->bits &= ~NPC_IGNORE_SOLIDITY;

      if (npc->flags & 8) {
        npc->yvel = -0x200;
        npc->act = 110;
        npc->bits |= NPC_IGNORE_SOLIDITY;
        snd_play_sound(PRIO_NORMAL, 26, FALSE);
        cam_start_quake_small(10);

        for (i = 0; i < 4; ++i)
          npc_spawn(4, npc->x + (m_rand(-12, 12) * 0x200), npc->y + (16 * 0x200), m_rand(-341, 341), m_rand(-0x600, 0),
            0, NULL, 0x100);
      }

      break;

    case 110:
      npc->yvel += 0x40;

      if (npc->y > (stage_data->height * 0x200 * 0x10) + (2 * 0x200 * 0x10)) {
        npc->cond = 0;
        return;
      }

      break;
  }

  if (player.y > npc->y)
    npc->damage = 10;
  else
    npc->damage = 0;

  npc->y += npc->yvel;
  npc->rect = &rc[npc->anim];

  if (npc->act == 11) {
    npc->rect_delta.top = npc->act_wait;
    npc->rect_delta.bottom = -npc->act_wait;
    npc->view.top = (16 - npc->act_wait) * 0x200;
    npc->rect_prev = NULL; // force texrect update
  }
}
