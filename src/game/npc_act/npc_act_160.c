#include "game/npc_act/npc_act.h"

// Puu Black
void npc_act_160(npc_t *npc) {
  int i;

  switch (npc->act) {
    case 0:
      npc->bits &= ~NPC_SOLID_SOFT;
      npc->act = 1;
      // Fallthrough
    case 1:
      if (npc->x < player.x)
        npc->dir = 2;
      else
        npc->dir = 0;

      npc->yvel = 0xA00;

      if (npc->y < 128 * 0x200) {
        ++npc->count1;
      } else {
        npc->bits &= ~NPC_IGNORE_SOLIDITY;
        npc->act = 2;
      }

      break;

    case 2:
      npc->yvel = 0xA00;

      if (npc->flags & 8) {
        npc_delete_by_class(161, TRUE);

        for (i = 0; i < 4; ++i)
          npc_spawn(4, npc->x + (m_rand(-12, 12) * 0x200), npc->y + (m_rand(-12, 12) * 0x200), m_rand(-341, 341),
            m_rand(-0x600, 0), 0, NULL, 0x100);

        npc->act = 3;
        npc->act_wait = 0;
        cam_start_quake_small(30);
        snd_play_sound(PRIO_NORMAL, 26, FALSE);
        snd_play_sound(PRIO_NORMAL, 72, FALSE);
      }

      if (npc->y < player.y && player.flags & 8)
        npc->damage = 20;
      else
        npc->damage = 0;

      break;

    case 3:
      npc->damage = 20;  // Overwritten by the following line
      npc->damage = 0;

      if (++npc->act_wait > 24) {
        npc->act = 4;
        npc->count1 = 0;
        npc->count2 = 0;
      }

      break;

    case 4:
      npc_doctor_state.crystal_x = npc->x;
      npc_doctor_state.crystal_y = npc->y;

      if (npc->shock % 2 == 1) {
        npc_spawn(161, npc->x + (m_rand(-12, 12) * 0x200), npc->y + (m_rand(-12, 12) * 0x200), m_rand(-0x600, 0x600),
                  m_rand(-0x600, 0x600), 0, NULL, 0x100);

        if (++npc->count1 > 30) {
          npc->count1 = 0;
          npc->act = 5;
          npc->yvel = -0xC00;
          npc->bits |= NPC_IGNORE_SOLIDITY;
        }
      }

      break;

    case 5:
      npc_doctor_state.crystal_x = npc->x;
      npc_doctor_state.crystal_y = npc->y;

      if (++npc->count1 > 60) {
        npc->count1 = 0;
        npc->act = 6;
      }

      break;

    case 6:
      npc_doctor_state.crystal_x = player.x;
      npc_doctor_state.crystal_y = 400 * 0x10 * 0x200;

      if (++npc->count1 > 110) {
        npc->count1 = 10;
        npc->x = player.x;
        npc->y = 0;
        npc->yvel = 0x5FF;
        npc->act = 1;
      }

      break;
  }

  npc->y += npc->yvel;

  switch (npc->act) {
    case 0:
    case 1:
      npc->anim = 3;
      break;

    case 2:
      npc->anim = 3;
      break;

    case 3:
      npc->anim = 2;
      break;

    case 4:
      npc->anim = 0;
      break;

    case 5:
      npc->anim = 3;
      break;

    case 6:
      npc->anim = 3;
      break;
  }

  static const rect_t rect_left[4] = {
      {0, 0, 40, 24},
      {40, 0, 80, 24},
      {80, 0, 120, 24},
      {120, 0, 160, 24},
  };

  static const rect_t rect_right[4] = {
      {0, 24, 40, 48},
      {40, 24, 80, 48},
      {80, 24, 120, 48},
      {120, 24, 160, 48},
  };

  if (npc->dir == 0)
    npc->rect = &rect_left[npc->anim];
  else
    npc->rect = &rect_right[npc->anim];
}

// Puu Black projectile
void npc_act_161(npc_t *npc) {
  npc->exp = 0;

  if (npc->x < npc_doctor_state.crystal_x)
    npc->xvel += 0x40;
  else
    npc->xvel -= 0x40;

  if (npc->y < npc_doctor_state.crystal_y)
    npc->yvel += 0x40;
  else
    npc->yvel -= 0x40;

  if (npc->xvel < -4605) npc->xvel = -4605;
  if (npc->xvel > 4605) npc->xvel = 4605;

  if (npc->yvel < -4605) npc->yvel = -4605;
  if (npc->yvel > 4605) npc->yvel = 4605;

  if (npc->life < 100) {
    npc->bits &= ~NPC_SHOOTABLE;
    npc->bits &= ~NPC_INVULNERABLE;
    npc->damage = 0;
    npc->anim = 2;
  }

  npc->x += npc->xvel;
  npc->y += npc->yvel;

  if (npc->anim < 2) {
    if (m_rand(0, 10) == 2)
      npc->anim = 0;
    else
      npc->anim = 1;
  }

  static const rect_t rect[3] = {
      {0, 48, 16, 64},
      {16, 48, 32, 64},
      {32, 48, 48, 64},
  };

  npc->rect = &rect[npc->anim];
}

// Puu Black (dead)
void npc_act_162(npc_t *npc) {
  int i;

  static const rect_t rect_left = {40, 0, 80, 24};
  static const rect_t rect_right = {40, 24, 80, 48};
  static const rect_t rect_end = {0, 0, 0, 0};

  switch (npc->act) {
    case 0:
      npc_delete_by_class(161, TRUE);
      snd_play_sound(PRIO_NORMAL, 72, FALSE);

      for (i = 0; i < 10; ++i)
        npc_spawn(4, npc->x + (m_rand(-12, 12) * 0x200), npc->y + (m_rand(-12, 12) * 0x200), m_rand(-0x600, 0x600),
           m_rand(-0x600, 0x600), 0, NULL, 0x100);

      if (npc->x > player.x)
        npc->dir = 0;
      else
        npc->dir = 2;

      if (npc->dir == 0)
        npc->rect = &rect_left;
      else
        npc->rect = &rect_right;

      npc->count1 = 0;
      npc->act = 1;
      // Fallthrough
    case 1:
      ++npc->count1;

      if (npc->count1 % 4 == 0)
        npc_spawn(161, npc->x + (m_rand(-12, 12) * 0x200), npc->y + (m_rand(-12, 12) * 0x200), 0, 0, 0, NULL, 0x100);

      if (npc->count1 > 160) {
        npc->count1 = 0;
        npc->act = 2;
        npc->tgt_y = npc->y;
      }

      break;

    case 2:
      cam_start_quake_small(2);

      ++npc->count1;

      if (npc->count1 <= 240) {
        if (npc->dir == 0)
          npc->rect = &rect_left;
        else
          npc->rect = &rect_right;

        npc->rect_delta.top = npc->count1 / 8;
        npc->y = npc->tgt_y + ((npc->count1 / 8) * 0x200);
        npc->rect_delta.left = -(npc->count1 / 2) % 2;
        npc->rect_prev = NULL; // force texrect update
      } else {
        npc->rect = &rect_end;

        npc->count1 = 0;
        npc->act = 3;
      }

      if (npc->count1 % 3 == 2)
        npc_spawn(161, npc->x + (m_rand(-12, 12) * 0x200), npc->y - (12 * 0x200), m_rand(-0x200, 0x200), 0x100, 0, NULL,
                  0x100);

      if (npc->count1 % 4 == 2) snd_play_sound(PRIO_NORMAL, 21, FALSE);

      break;

    case 3:
      if (++npc->count1 < 60) break;

      npc_delete_by_class(161, TRUE);
      npc->cond = 0;

      break;
  }

  npc_doctor_state.crystal_x = npc->x;
  npc_doctor_state.crystal_y = -1000 * 0x200;
}

// Dr Gero
void npc_act_163(npc_t *npc) {
  static const rect_t rc_left[2] = {
      {192, 0, 208, 16},
      {208, 0, 224, 16},
  };

  static const rect_t rc_right[2] = {
      {192, 16, 208, 32},
      {208, 16, 224, 32},
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

      break;

    case 2:
      if (++npc->act_wait > 8) {
        npc->act = 1;
        npc->anim = 0;
      }

      break;
  }

  if (npc->dir == 0)
    npc->rect = &rc_left[npc->anim];
  else
    npc->rect = &rc_right[npc->anim];
}

// Nurse Hasumi
void npc_act_164(npc_t *npc) {
  static const rect_t rc_left[2] = {
      {224, 0, 240, 16},
      {240, 0, 256, 16},
  };

  static const rect_t rc_right[2] = {
      {224, 16, 240, 32},
      {240, 16, 256, 32},
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

      break;

    case 2:
      if (++npc->act_wait > 8) {
        npc->act = 1;
        npc->anim = 0;
      }

      break;
  }

  if (npc->dir == 0)
    npc->rect = &rc_left[npc->anim];
  else
    npc->rect = &rc_right[npc->anim];
}

// Curly (collapsed)
void npc_act_165(npc_t *npc) {
  static const rect_t rc_right[2] = {
    {192, 96, 208, 112},
    {208, 96, 224, 112},
  };

  static const rect_t rc_left = {144, 96, 160, 112};

  switch (npc->act) {
    case 0:
      npc->act = 1;
      npc->y += 10 * 0x200;
      // Fallthrough
    case 1:
      if (npc->dir == 2 && player.x > npc->x - (32 * 0x200) && player.x < npc->x + (32 * 0x200) &&
          player.y > npc->y - (16 * 0x200) && player.y < npc->y + (16 * 0x200))
        npc->anim = 1;
      else
        npc->anim = 0;

      break;
  }

  if (npc->dir == 0)
    npc->rect = &rc_left;
  else
    npc->rect = &rc_right[npc->anim];
}

// Chaba
void npc_act_166(npc_t *npc) {
  static const rect_t rc_left[2] = {
    {144, 104, 184, 128},
    {184, 104, 224, 128},
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

      break;

    case 2:
      if (++npc->act_wait > 8) {
        npc->act = 1;
        npc->anim = 0;
      }

      break;
  }

  npc->rect = &rc_left[npc->anim];
}

// Professor Booster (falling)
void npc_act_167(npc_t *npc) {
  int i;

  static const rect_t rect[3] = {
    {304, 0, 320, 16},
    {304, 16, 320, 32},
    {0, 0, 0, 0},
  };

  switch (npc->act) {
    case 0:
      npc->act = 1;
      npc->anim = 1;
      break;

    case 10:
      npc->anim = 0;

      npc->yvel += 0x40;
      if (npc->yvel > 0x5FF) npc->yvel = 0x5FF;

      npc->y += npc->yvel;
      break;

    case 20:
      npc->act = 21;
      npc->act_wait = 0;
      npc->anim = 0;
      snd_play_sound(PRIO_NORMAL, 29, FALSE);
      // Fallthrough
    case 21:
      if (++npc->anim > 2) npc->anim = 1;

      if (++npc->act_wait > 100) {
        for (i = 0; i < 4; ++i)
          npc_spawn(4, npc->x + (m_rand(-12, 12) * 0x200), npc->y + (m_rand(-12, 12) * 0x200), m_rand(-341, 341),
                    m_rand(-0x600, 0), 0, NULL, 0x100);

        npc->cond = 0;
      }

      break;
  }

  npc->rect = &rect[npc->anim];
}

// Boulder
void npc_act_168(npc_t *npc) {
  static const rect_t rect = {264, 56, 320, 96};

  switch (npc->act) {
    case 0:
      npc->act = 1;
      break;

    case 10:
      npc->act = 11;
      npc->act_wait = 0;
      npc->tgt_x = npc->x;
      // Fallthrough
    case 11:
      ++npc->act_wait;
      npc->x = npc->tgt_x;

      if (npc->act_wait / 3 % 2) npc->x += 1 * 0x200;

      break;

    case 20:
      npc->act = 21;
      npc->act_wait = 0;
      npc->yvel = -0x400;
      npc->xvel = 0x100;
      snd_play_sound(PRIO_NORMAL, 25, FALSE);
      // Fallthrough
    case 21:
      npc->yvel += 0x10;

      npc->x += npc->xvel;
      npc->y += npc->yvel;

      if (npc->act_wait && npc->flags & 8) {
        snd_play_sound(PRIO_NORMAL, 35, FALSE);
        cam_start_quake_small(40);
        npc->act = 0;
      }

      if (npc->act_wait == 0) ++npc->act_wait;

      break;
  }

  npc->rect = &rect;
}

// Balrog (missile)
void npc_act_169(npc_t *npc) {
  switch (npc->act) {
    case 0:
      npc->act = 1;
      npc->anim = 0;
      npc->act_wait = 30;

      if (npc->x > player.x)
        npc->dir = 0;
      else
        npc->dir = 2;
      // Fallthrough
    case 1:
      if (--npc->act_wait != 0) break;

      npc->act = 2;
      ++npc->count1;
      break;

    case 2:
      npc->act = 3;
      npc->act_wait = 0;
      npc->anim = 1;
      npc->anim_wait = 0;
      // Fallthrough
    case 3:
      ++npc->anim_wait;

      if (npc->anim_wait > 3) {
        npc->anim_wait = 0;
        ++npc->anim;

        if (npc->anim == 2 || npc->anim == 4) snd_play_sound(PRIO_NORMAL, 23, FALSE);
      }

      if (npc->anim > 4) npc->anim = 1;

      if (npc->dir == 0)
        npc->xvel -= 0x20;
      else
        npc->xvel += 0x20;

      if (npc->act_wait >= 8 && npc->x - (12 * 0x200) < player.x && npc->x + (12 * 0x200) > player.x &&
          npc->y - (12 * 0x200) < player.y && npc->y + (8 * 0x200) > player.y) {
        npc->act = 10;
        npc->anim = 5;
        player.cond |= PLRCOND_INVISIBLE;
        plr_damage(5);
        break;
      }

      ++npc->act_wait;

      if (npc->act_wait > 75) {
        npc->act = 9;
        npc->anim = 0;
        break;
      }

      if (npc->flags & 5) {
        if (npc->count2 < 5) {
          ++npc->count2;
        } else {
          npc->act = 4;
          npc->act_wait = 0;
          npc->anim = 7;
          npc->yvel = -0x5FF;
        }
      } else {
        npc->count2 = 0;
      }

      if (npc->count1 % 2 == 0 && npc->act_wait > 25) {
        npc->act = 4;
        npc->act_wait = 0;
        npc->anim = 7;
        npc->yvel = -0x5FF;
        break;
      }

      break;

    case 4:
      if (npc->x > player.x)
        npc->dir = 0;
      else
        npc->dir = 2;

      ++npc->act_wait;

      if (npc->act_wait < 30 && npc->act_wait % 6 == 1) {
        snd_play_sound(PRIO_NORMAL, 39, FALSE);
        npc_spawn(170, npc->x, npc->y, 0, 0, npc->dir, NULL, 0x100);
      }

      if (npc->flags & 8) {
        npc->act = 9;
        npc->anim = 8;
        cam_start_quake_small(30);
        snd_play_sound(PRIO_NORMAL, 26, FALSE);
      }

      if (npc->act_wait >= 8 && npc->x - (12 * 0x200) < player.x && npc->x + (12 * 0x200) > player.x &&
          npc->y - (12 * 0x200) < player.y && npc->y + (8 * 0x200) > player.y) {
        npc->act = 10;
        npc->anim = 5;
        player.cond |= PLRCOND_INVISIBLE;
        plr_damage(10);
      }

      break;

    case 9:
      npc->xvel = (npc->xvel * 4) / 5;

      if (npc->xvel != 0) break;

      npc->act = 0;
      break;

    case 10:
      player.x = npc->x;
      player.y = npc->y;

      npc->xvel = (npc->xvel * 4) / 5;

      if (npc->xvel != 0) break;

      npc->act = 11;
      npc->act_wait = 0;
      npc->anim = 5;
      npc->anim_wait = 0;
      break;

    case 11:
      player.x = npc->x;
      player.y = npc->y;

      ++npc->anim_wait;

      if (npc->anim_wait > 2) {
        npc->anim_wait = 0;
        ++npc->anim;
      }

      if (npc->anim > 6) npc->anim = 5;

      ++npc->act_wait;

      if (npc->act_wait > 100) npc->act = 20;

      break;

    case 20:
      snd_play_sound(PRIO_NORMAL, 25, FALSE);
      player.cond &= ~PLRCOND_INVISIBLE;

      if (npc->dir == 0) {
        player.x += 4 * 0x200;
        player.y -= 8 * 0x200;
        player.xvel = 0x5FF;
        player.yvel = -0x200;
        player.dir = 2;
        npc->dir = 2;
      } else {
        player.x -= 4 * 0x200;
        player.y -= 8 * 0x200;
        player.xvel = -0x5FF;
        player.yvel = -0x200;
        player.dir = 0;
        npc->dir = 0;
      }

      npc->act = 21;
      npc->act_wait = 0;
      npc->anim = 7;
      // Fallthrough
    case 21:
      ++npc->act_wait;

      if (npc->act_wait < 50) break;

      npc->act = 0;
      break;
  }

  npc->yvel += 0x20;

  if (npc->xvel < -0x300) npc->xvel = -0x300;
  if (npc->xvel > 0x300) npc->xvel = 0x300;

  if (npc->yvel > 0x5FF) npc->yvel = 0x5FF;

  npc->x += npc->xvel;
  npc->y += npc->yvel;

  static const rect_t rect_left[9] = {
    {0, 0, 40, 24},    {0, 48, 40, 72},    {0, 0, 40, 24},    {40, 48, 80, 72}, {0, 0, 40, 24},
    {80, 48, 120, 72}, {120, 48, 160, 72}, {120, 0, 160, 24}, {80, 0, 120, 24},
  };

  static const rect_t rect_right[9] = {
    {0, 24, 40, 48},   {0, 72, 40, 96},    {0, 24, 40, 48},    {40, 72, 80, 96},  {0, 24, 40, 48},
    {80, 72, 120, 96}, {120, 72, 160, 96}, {120, 24, 160, 48}, {80, 24, 120, 48},
  };

  if (npc->dir == 0)
    npc->rect = &rect_left[npc->anim];
  else
    npc->rect = &rect_right[npc->anim];
}

// Balrog missile
void npc_act_170(npc_t *npc) {
  static const rect_t rc_left[2] = {
    {112, 96, 128, 104},
    {128, 96, 144, 104},
  };

  static const rect_t rc_right[2] = {
    {112, 104, 128, 112},
    {128, 104, 144, 112},
  };

  bool hit = FALSE;

  if (npc->dir == 0 && npc->flags & 1) hit = TRUE;
  if (npc->dir == 2 && npc->flags & 4) hit = TRUE;

  if (hit) {
    snd_play_sound(PRIO_NORMAL, 44, FALSE);
    npc_spawn_death_fx(npc->x, npc->y, 0, 3, 0);
    npc_show_death_damage(npc);
    return;
  }

  switch (npc->act) {
    case 0:
      npc->act = 1;

      if (npc->dir == 0)
        npc->xvel = m_rand(1, 2) * 0x200;
      else
        npc->xvel = m_rand(-2, -1) * 0x200;

      npc->yvel = m_rand(-2, 0) * 0x200;
      // Fallthrough
    case 1:
      ++npc->count1;

      if (npc->dir == 0) {
        npc->xvel -= 0x20;
        if (npc->count1 % 3 == 1)
          caret_spawn(npc->x + (8 * 0x200), npc->y, CARET_EXHAUST, DIR_RIGHT);
      } else {
        npc->xvel += 0x20;
        if (npc->count1 % 3 == 1)
          caret_spawn(npc->x - (8 * 0x200), npc->y, CARET_EXHAUST, DIR_LEFT);
      }

      if (npc->count1 < 50) {
        if (npc->y < player.y)
          npc->yvel += 0x20;
        else
          npc->yvel -= 0x20;
      } else {
        npc->yvel = 0;
      }

      if (++npc->anim > 1) npc->anim = 0;

      break;
  }

  if (npc->xvel < -0x400) npc->xvel = -0x600;
  if (npc->xvel > 0x400) npc->xvel = 0x600;

  npc->x += npc->xvel;
  npc->y += npc->yvel;

  if (npc->dir == 0)
    npc->rect = &rc_left[npc->anim];
  else
    npc->rect = &rc_right[npc->anim];
}

// Fire Whirrr
void npc_act_171(npc_t *npc) {
  static const rect_t rc_left[2] = {
    {120, 48, 152, 80},
    {152, 48, 184, 80},
  };

  static const rect_t rc_right[2] = {
    {184, 48, 216, 80},
    {216, 48, 248, 80},
  };

  if (npc->x > player.x)
    npc->dir = 0;
  else
    npc->dir = 2;

  switch (npc->act) {
    case 0:
      npc->act = 1;
      npc->act_wait = m_rand(0, 50);
      npc->tgt_y = npc->y;
      // Fallthrough
    case 1:
      if (npc->act_wait != 0) {
        --npc->act_wait;
      } else {
        npc->act = 10;
        npc->yvel = 0x200;
      }
      // Fallthrough
    case 10:
      if (++npc->anim_wait > 0) {
        npc->anim_wait = 0;
        ++npc->anim;
      }

      if (npc->anim > 1) npc->anim = 0;

      if (npc->y < npc->tgt_y)
        npc->yvel += 0x10;
      else
        npc->yvel -= 0x10;

      if (npc->yvel > 0x200) npc->yvel = 0x200;
      if (npc->yvel < -0x200) npc->yvel = -0x200;

      npc->y += npc->yvel;

      if (npc->dir == 0) {
        if (player.y < npc->y + (80 * 0x200) && player.y > npc->y - (80 * 0x200) && player.x < npc->x &&
            player.x > npc->x - (160 * 0x200))
          ++npc->count1;
      } else {
        if (player.y < npc->y + (80 * 0x200) && player.y > npc->y - (80 * 0x200) && player.x < npc->x + (160 * 0x200) &&
            player.x > npc->x)
          ++npc->count1;
      }

      if (npc->count1 > 20) {
        npc_spawn(172, npc->x, npc->y, 0, 0, npc->dir, NULL, 0x100);
        npc->count1 = -100;
        npc_curly_state.shoot_wait = m_rand(80, 100);
        npc_curly_state.shoot_x = npc->x;
        npc_curly_state.shoot_y = npc->y;
      }

      break;
  }

  if (npc->dir == 0)
    npc->rect = &rc_left[npc->anim];
  else
    npc->rect = &rc_right[npc->anim];
}

// Fire Whirr projectile
void npc_act_172(npc_t *npc) {
  static const rect_t rect[3] = {
    {248, 48, 264, 80},
    {264, 48, 280, 80},
    {280, 48, 296, 80},
  };

  switch (npc->act) {
    case 0:
      npc->act = 1;
      // Fallthrough
    case 1:
      if (++npc->anim_wait > 1) {
        npc->anim_wait = 0;
        ++npc->anim;
      }

      if (npc->anim > 2) npc->anim = 0;

      if (npc->dir == 0)
        npc->x -= 1 * 0x200;
      else
        npc->x += 1 * 0x200;

      if (npc->flags & 1 || npc->flags & 4) {
        caret_spawn(npc->x, npc->y, CARET_PROJECTILE_DISSIPATION, DIR_LEFT);
        npc_show_death_damage(npc);
        return;
      }

      break;
  }

  npc->rect = &rect[npc->anim];
}

// Gaudi (armoured)
void npc_act_173(npc_t *npc) {
  u8 deg;
  int xvel, yvel;

  static const rect_t rc_left[4] = {
    {0, 128, 24, 152},
    {24, 128, 48, 152},
    {48, 128, 72, 152},
    {72, 128, 96, 152},
  };

  static const rect_t rc_right[4] = {
    {0, 152, 24, 176},
    {24, 152, 48, 176},
    {48, 152, 72, 176},
    {72, 152, 96, 176},
  };

  if (npc->x > player.x + (((VID_WIDTH / 2) + 160) * 0x200) || npc->x < player.x - (((VID_WIDTH / 2) + 160) * 0x200) ||
      npc->y > player.y + (((VID_HEIGHT / 2) + 120) * 0x200) || npc->y < player.y - (((VID_HEIGHT / 2) + 120) * 0x200))
    return;

  switch (npc->act) {
    case 0:
      npc->tgt_x = npc->x;
      npc->act = 1;
      // Fallthrough
    case 1:
      npc->anim = 0;
      npc->xvel = 0;

      if (npc->act_wait < 5) {
        ++npc->act_wait;
      } else {
        if (npc->x - (192 * 0x200) < player.x && npc->x + (192 * 0x200) > player.x &&
            npc->y - (160 * 0x200) < player.y && npc->y + (160 * 0x200) > player.y) {
          npc->act = 10;
          npc->act_wait = 0;
          npc->anim = 1;
        }
      }

      break;

    case 10:
      if (++npc->act_wait > 3) {
        if (++npc->count1 == 3) {
          snd_play_sound(PRIO_NORMAL, 30, FALSE);
          npc->count1 = 0;
          npc->act = 25;
          npc->act_wait = 0;
          npc->anim = 2;
          npc->yvel = -0x600;

          if (npc->x < npc->tgt_x)
            npc->xvel = 0x80;
          else
            npc->xvel = -0x80;
        } else {
          snd_play_sound(PRIO_NORMAL, 30, FALSE);
          npc->act = 20;
          npc->anim = 2;
          npc->yvel = -0x200;

          if (npc->x < npc->tgt_x)
            npc->xvel = 0x200;
          else
            npc->xvel = -0x200;
        }
      }

      break;

    case 20:
      ++npc->act_wait;

      if (npc->flags & 8) {
        snd_play_sound(PRIO_NORMAL, 23, FALSE);
        npc->anim = 1;
        npc->act = 30;
        npc->act_wait = 0;
      }

      break;

    case 25:
      if (++npc->act_wait == 30 || npc->act_wait == 40) {
        deg = m_atan2(npc->x - player.x, npc->y - player.y);
        deg += (u8)m_rand(-6, 6);
        yvel = m_sin(deg) * 3;
        xvel = m_cos(deg) * 3;

        npc_spawn(174, npc->x, npc->y, xvel, yvel, 0, NULL, 0x100);

        snd_play_sound(PRIO_NORMAL, 39, FALSE);
        npc->anim = 3;

        npc_curly_state.shoot_wait = m_rand(80, 100);
        npc_curly_state.shoot_x = npc->x;
        npc_curly_state.shoot_y = npc->y;
      }

      if (npc->act_wait == 35 || npc->act_wait == 45) npc->anim = 2;

      if (npc->flags & 8) {
        snd_play_sound(PRIO_NORMAL, 23, FALSE);
        npc->anim = 1;
        npc->act = 30;
        npc->act_wait = 0;
      }

      break;

    case 30:
      npc->xvel = 7 * npc->xvel / 8;

      if (++npc->act_wait > 3) {
        npc->anim = 0;
        npc->act = 1;
        npc->act_wait = 0;
      }

      break;
  }

  npc->yvel += 51;

  if (player.x < npc->x)
    npc->dir = 0;
  else
    npc->dir = 2;

  if (npc->yvel > 0x5FF) npc->yvel = 0x5FF;
  if (npc->yvel < -0x5FF) npc->yvel = 0x5FF;

  npc->x += npc->xvel;
  npc->y += npc->yvel;

  if (npc->dir == 0)
    npc->rect = &rc_left[npc->anim];
  else
    npc->rect = &rc_right[npc->anim];

  if (npc->life <= 985) {
    npc_spawn_death_fx(npc->x, npc->y, 0, 2, 0);
    npc->class_num = 154;
    npc->act = 0;
  }
}

// Armoured-Gaudi projectile
void npc_act_174(npc_t *npc) {
  bool hit;

  switch (npc->act) {
    case 0:
      if (npc->dir == 2) npc->act = 2;
      // Fallthrough
    case 1:
      npc->x += npc->xvel;
      npc->y += npc->yvel;

      hit = FALSE;

      if (npc->flags & 1) {
        hit = TRUE;
        npc->xvel = 0x200;
      }

      if (npc->flags & 4) {
        hit = TRUE;
        npc->xvel = -0x200;
      }

      if (npc->flags & 2) {
        hit = TRUE;
        npc->yvel = 0x200;
      }

      if (npc->flags & 8) {
        hit = TRUE;
        npc->yvel = -0x200;
      }

      if (hit) {
        npc->act = 2;
        ++npc->count1;
        snd_play_sound(PRIO_NORMAL, 31, FALSE);
      }

      break;

    case 2:
      npc->yvel += 0x40;

      npc->x += npc->xvel;
      npc->y += npc->yvel;

      if (npc->flags & 8) {
        if (++npc->count1 > 1) {
          caret_spawn(npc->x, npc->y, CARET_PROJECTILE_DISSIPATION, DIR_LEFT);
          npc->cond = 0;
        }
      }

      break;
  }

  if (npc->yvel > 0x5FF) npc->yvel = 0x5FF;
  if (npc->yvel < -0x5FF) npc->yvel = -0x5FF;

  static const rect_t rect_left[3] = {
    {120, 80, 136, 96},
    {136, 80, 152, 96},
    {152, 80, 168, 96},
  };

  if (++npc->anim > 2) npc->anim = 0;

  npc->rect = &rect_left[npc->anim];
}

// Gaudi egg
void npc_act_175(npc_t *npc) {
  static const rect_t rc_left[2] = {
    {168, 80, 192, 104},
    {192, 80, 216, 104},
  };

  static const rect_t rc_right[2] = {
    {216, 80, 240, 104},
    {240, 80, 264, 104},
  };

  if (npc->act < 3 && npc->life < 90) {
    npc_kill(npc, FALSE);
    npc->act = 10;
    npc->anim = 1;
    npc->bits &= ~NPC_SHOOTABLE;
    npc->damage = 0;
  }

  switch (npc->act) {
    case 0:
      npc->anim = 0;
      npc->act = 1;
      break;
  }

  if (npc->dir == 0)
    npc->yvel += 0x20;
  else
    npc->yvel -= 0x20;

  if (npc->yvel < -0x5FF) npc->yvel = -0x5FF;
  if (npc->yvel > 0x5FF) npc->yvel = 0x5FF;

  npc->y += npc->yvel;

  if (npc->dir == 0)
    npc->rect = &rc_left[npc->anim];
  else
    npc->rect = &rc_right[npc->anim];
}

// BuyoBuyo Base
void npc_act_176(npc_t *npc) {
  static const rect_t rc_left[3] = {
    {96, 128, 128, 144},
    {128, 128, 160, 144},
    {160, 128, 192, 144},
  };

  static const rect_t rc_right[3] = {
    {96, 144, 128, 160},
    {128, 144, 160, 160},
    {160, 144, 192, 160},
  };

  if (npc->act < 3 && npc->life < 940) {
    npc_kill(npc, FALSE);
    npc->act = 10;
    npc->anim = 2;
    npc->bits &= ~NPC_SHOOTABLE;
    npc->damage = 0;
  }

  switch (npc->act) {
    case 0:
      npc->act = 1;
      npc->anim = 0;
      npc->anim_wait = 0;
      // Fallthrough
    case 1:
      if (npc->dir == 0) {
        if (npc->x < player.x + (160 * 0x200) && npc->x > player.x - (160 * 0x200) &&
            npc->y < player.y + (160 * 0x200) && npc->y > player.y - (16 * 0x200))
          ++npc->count1;
      } else {
        if (npc->x < player.x + (160 * 0x200) && npc->x > player.x - (160 * 0x200) &&
            npc->y < player.y + (16 * 0x200) && npc->y > player.y - (160 * 0x200))
          ++npc->count1;
      }

      if (npc->count1 > 10) {
        npc->act = 2;
        npc->act_wait = 0;
      }

      break;

    case 2:
      if (++npc->anim_wait > 3) {
        npc->anim_wait = 0;
        ++npc->anim;
      }

      if (npc->anim > 1) npc->anim = 0;

      if (++npc->act_wait > 10) {
        if (++npc->count2 > 2) {
          npc->count2 = 0;
          npc->count1 = -90;
        } else {
          npc->count1 = -10;
        }

        if (npc->dir == 0)
          npc_spawn(177, npc->x, npc->y - (8 * 0x200), 0, 0, 0, NULL, 0x100);
        else
          npc_spawn(177, npc->x, npc->y + (8 * 0x200), 0, 0, 2, NULL, 0x100);

        snd_play_sound(PRIO_NORMAL, 39, FALSE);

        npc->act = 0;
        npc->anim = 0;

        npc_curly_state.shoot_wait = m_rand(80, 100);
        npc_curly_state.shoot_x = npc->x;
        npc_curly_state.shoot_y = npc->y;
      }

      break;
  }

  if (npc->dir == 0)
    npc->rect = &rc_left[npc->anim];
  else
    npc->rect = &rc_right[npc->anim];
}

// BuyoBuyo
void npc_act_177(npc_t *npc) {
  static const rect_t rc[2] = {
    {192, 128, 208, 144},
    {208, 128, 224, 144},
  };

  if (npc->flags & 0xFF) {
    caret_spawn(npc->x, npc->y, CARET_SHOOT, DIR_LEFT);
    npc->cond = 0;
    return;
  }

  switch (npc->act) {
    case 0:
      npc->act = 1;

      if (npc->dir == 0)
        npc->yvel = -0x600;
      else
        npc->yvel = 0x600;
      // Fallthrough
    case 1:
      if (npc->y < player.y + (16 * 0x200) && npc->y > player.y - (16 * 0x200)) {
        npc->act = 10;

        npc->tgt_x = npc->x;
        npc->tgt_y = npc->y;

        if (npc->x > player.x)
          npc->dir = 0;
        else
          npc->dir = 2;

        npc->xvel = ((m_rand(0, 1) * 0x200) - 0x100) * 2;
        npc->yvel = ((m_rand(0, 1) * 0x200) - 0x100) * 2;
      }

      break;

    case 10:
      if (npc->x < npc->tgt_x)
        npc->xvel += 0x20;
      else
        npc->xvel -= 0x20;

      if (npc->y < npc->tgt_y)
        npc->yvel += 0x20;
      else
        npc->yvel -= 0x20;

      if (++npc->act_wait > 300) {
        caret_spawn(npc->x, npc->y, CARET_SHOOT, DIR_LEFT);
        npc->cond = 0;
        return;
      }

      if (npc->dir == 0)
        npc->tgt_x -= 1 * 0x200;
      else
        npc->tgt_x += 1 * 0x200;

      break;
  }

  if (npc->xvel > 0x400) npc->xvel = 0x400;
  if (npc->xvel < -0x400) npc->xvel = -0x400;

  if (npc->yvel > 0x400) npc->yvel = 0x400;
  if (npc->yvel < -0x400) npc->yvel = -0x400;

  npc->x += npc->xvel;
  npc->y += npc->yvel;

  if (++npc->anim_wait > 6) {
    npc->anim_wait = 0;
    ++npc->anim;
  }

  if (npc->anim > 1) npc->anim = 0;

  npc->rect = &rc[npc->anim];
}

// Core blade projectile
void npc_act_178(npc_t *npc) {
  if (npc->flags & 0xFF) {
    caret_spawn(npc->x, npc->y, CARET_PROJECTILE_DISSIPATION, DIR_LEFT);
    npc->cond = 0;
  }

  if (npc->flags & 0x100) {
    npc->y += npc->yvel / 2;
    npc->x += npc->xvel / 2;
  } else {
    npc->y += npc->yvel;
    npc->x += npc->xvel;
  }

  static const rect_t rect_left[3] = {
    {0, 224, 16, 240},
    {16, 224, 32, 240},
    {32, 224, 48, 240},
  };

  if (++npc->anim_wait > 1) {
    npc->anim_wait = 0;
    ++npc->anim;
  }

  if (npc->anim > 2) npc->anim = 0;

  npc->rect = &rect_left[npc->anim];

  if (++npc->count1 > 150) {
    npc_show_death_damage(npc);
    caret_spawn(npc->x, npc->y, CARET_PROJECTILE_DISSIPATION, DIR_LEFT);
  }
}

// Core wisp projectile
void npc_act_179(npc_t *npc) {
  if (npc->flags & 0xFF) {
    npc->cond = 0;
    caret_spawn(npc->x, npc->y, CARET_PROJECTILE_DISSIPATION, DIR_LEFT);
  }

  npc->xvel -= 0x20;
  npc->yvel = 0;

  if (npc->xvel < -0x400) npc->xvel = -0x400;

  npc->y += npc->yvel;
  npc->x += npc->xvel;

  static const rect_t rect_left[3] = {
    {48, 224, 72, 240},
    {72, 224, 96, 240},
    {96, 224, 120, 240},
  };

  if (++npc->anim_wait > 1) {
    npc->anim_wait = 0;
    ++npc->anim;
  }

  if (npc->anim > 2) npc->anim = 0;

  npc->rect = &rect_left[npc->anim];

  if (++npc->count1 > 300) {
    npc_show_death_damage(npc);
    caret_spawn(npc->x, npc->y, CARET_PROJECTILE_DISSIPATION, DIR_LEFT);
  }
}
