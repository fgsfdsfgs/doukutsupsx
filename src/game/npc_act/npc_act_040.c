#include "game/npc_act/npc_act.h"

// Santa
void npc_act_040(npc_t *npc) {
  static const rect_t rc_left[7] = {
    {0, 32, 16, 48},  {16, 32, 32, 48}, {32, 32, 48, 48}, {0, 32, 16, 48},
    {48, 32, 64, 48}, {0, 32, 16, 48},  {64, 32, 80, 48},
  };

  static const rect_t rc_right[7] = {
    {0, 48, 16, 64},  {16, 48, 32, 64}, {32, 48, 48, 64}, {0, 48, 16, 64},
    {48, 48, 64, 64}, {0, 48, 16, 64},  {64, 48, 80, 64},
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

      if (npc->x - (32 * 0x200) < player.x && npc->x + (32 * 0x200) > player.x && npc->y - (32 * 0x200) < player.y &&
          npc->y + (16 * 0x200) > player.y) {
        if (npc->x > player.x)
          npc->dir = 0;
        else
          npc->dir = 2;
      }
      break;

    case 2:
      if (++npc->act_wait > 8) {
        npc->act = 1;
        npc->anim = 0;
      }

      break;

    case 3:
      npc->act = 4;
      npc->anim = 2;
      npc->anim_wait = 0;
      // Fallthrough
    case 4:
      if (++npc->anim_wait > 4) {
        npc->anim_wait = 0;
        npc->anim++;
      }

      if (npc->anim > 5) npc->anim = 2;

      if (npc->dir == 0)
        npc->x -= 1 * 0x200;
      else
        npc->x += 1 * 0x200;

      break;

    case 5:
      npc->anim = 6;
      break;
  }

  if (npc->dir == 0)
    npc->rect = &rc_left[npc->anim];
  else
    npc->rect = &rc_right[npc->anim];
}

// Busted Door
void npc_act_041(npc_t *npc) {
  static const rect_t rect = {0, 80, 48, 112};

  if (npc->act == 0) {
    ++npc->act;
    npc->y -= 1 * 0x10 * 0x200;  // Move one tile up
  }

  npc->rect = &rect;
}

// Sue
void npc_act_042(npc_t *npc) {
  int n;

  static const rect_t rc_left[13] = {
    {0, 0, 16, 16}, {16, 0, 32, 16},    {32, 0, 48, 16},    {0, 0, 16, 16},    {48, 0, 64, 16},
    {0, 0, 16, 16}, {64, 0, 80, 16},    {80, 32, 96, 48},   {96, 32, 112, 48}, {128, 32, 144, 48},
    {0, 0, 16, 16}, {112, 32, 128, 48}, {160, 32, 176, 48},
  };

  static const rect_t rc_right[13] = {
    {0, 16, 16, 32}, {16, 16, 32, 32},   {32, 16, 48, 32},   {0, 16, 16, 32},   {48, 16, 64, 32},
    {0, 16, 16, 32}, {64, 16, 80, 32},   {80, 48, 96, 64},   {96, 48, 112, 64}, {128, 48, 144, 64},
    {0, 16, 16, 32}, {112, 48, 128, 64}, {160, 48, 176, 64},
  };

  switch (npc->act) {
    case 0:
      npc->act = 1;
      npc->anim = 0;
      npc->anim_wait = 0;
      npc->xvel = 0;
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

    case 3:
      npc->act = 4;
      npc->anim = 2;
      npc->anim_wait = 0;
      // Fallthrough
    case 4:
      if (++npc->anim_wait > 4) {
        npc->anim_wait = 0;
        ++npc->anim;
      }

      if (npc->anim > 5) npc->anim = 2;

      if (npc->dir == 0)
        npc->xvel = -0x200;
      else
        npc->xvel = 0x200;

      break;

    case 5:
      npc->anim = 6;
      npc->xvel = 0;
      break;

    case 6:
      snd_play_sound(-1, 50, SOUND_MODE_PLAY);
      npc->act_wait = 0;
      npc->act = 7;
      npc->anim = 7;
      // Fallthrough
    case 7:
      if (++npc->act_wait > 10) npc->act = 0;

      break;

    case 8:
      snd_play_sound(-1, 50, SOUND_MODE_PLAY);
      npc->act_wait = 0;
      npc->act = 9;
      npc->anim = 7;
      npc->yvel = -0x200;

      if (npc->dir == 0)
        npc->xvel = 0x400;
      else
        npc->xvel = -0x400;

      // Fallthrough
    case 9:
      if (++npc->act_wait > 3 && npc->flags & 8) {
        npc->act = 10;

        if (npc->dir == 0)
          npc->dir = 2;
        else
          npc->dir = 0;
      }

      break;

    case 10:
      npc->xvel = 0;
      npc->anim = 8;
      break;

    case 11:
      npc->act = 12;
      npc->act_wait = 0;
      npc->anim = 9;
      npc->anim_wait = 0;
      npc->xvel = 0;
      // Fallthrough
    case 12:
      if (++npc->anim_wait > 8) {
        npc->anim_wait = 0;
        ++npc->anim;
      }

      if (npc->anim > 10) npc->anim = 9;

      break;

    case 13:
      npc->anim = 11;
      npc->xvel = 0;
      npc->yvel = 0;
      npc->act = 14;

      for (n = 0; n <= npc_list_max; ++n)
        if (npc_list[n].event_num == 501)
          break;

      if (n == NPC_MAX) {
        npc->act = 0;
        break;
      }

      npc->parent = &npc_list[n];
      // Fallthrough
    case 14:
      if (npc->parent->dir == 0)
        npc->dir = 2;
      else
        npc->dir = 0;

      if (npc->parent->dir == 0)
        npc->x = npc->parent->x - (6 * 0x200);
      else
        npc->x = npc->parent->x + (6 * 0x200);

      npc->y = npc->parent->y + (4 * 0x200);

      if (npc->parent->anim == 2 || npc->parent->anim == 4) npc->y -= 1 * 0x200;

      break;

    case 15:
      npc->act = 16;
      npc_spawn(257, npc->x + (128 * 0x200), npc->y, 0, 0, 0, NULL, 0);
      npc_spawn(257, npc->x + (128 * 0x200), npc->y, 0, 0, 2, NULL, 0x80);
      npc->xvel = 0;
      npc->anim = 0;
      // Fallthrough
    case 16:
      // gSuperXpos = npc->x - (24 * 0x200);
      // gSuperYpos = npc->y - (8 * 0x200);
      break;

    case 17:
      npc->xvel = 0;
      npc->anim = 12;
      // gSuperXpos = npc->x;
      // gSuperYpos = npc->y - (8 * 0x200);
      break;

    case 20:
      npc->act = 21;
      npc->anim = 2;
      npc->anim_wait = 0;
      // Fallthrough
    case 21:
      if (++npc->anim_wait > 2) {
        npc->anim_wait = 0;
        ++npc->anim;
      }

      if (npc->anim > 5) npc->anim = 2;

      if (npc->dir == 0)
        npc->xvel = -0x400;
      else
        npc->xvel = 0x400;

      if (npc->x < player.x - (8 * 0x200)) {
        npc->dir = 2;
        npc->act = 0;
      }

      break;

    case 30:
      npc->act = 31;
      npc->anim = 2;
      npc->anim_wait = 0;
      // Fallthrough
    case 31:
      if (++npc->anim_wait > 2) {
        npc->anim_wait = 0;
        ++npc->anim;
      }

      if (npc->anim > 5) npc->anim = 2;

      if (npc->dir == 0)
        npc->xvel = -0x400;
      else
        npc->xvel = 0x400;

      break;

    case 40:
      npc->act = 41;
      npc->anim = 9;
      npc->yvel = -0x400;
      break;
  }

  if (npc->act != 14) {
    npc->yvel += 0x40;

    if (npc->xvel > 0x400) npc->xvel = 0x400;
    if (npc->xvel < -0x400) npc->xvel = -0x400;

    if (npc->yvel > 0x5FF) npc->yvel = 0x5FF;

    npc->x += npc->xvel;
    npc->y += npc->yvel;
  }

  if (npc->dir == 0)
    npc->rect = &rc_left[npc->anim];
  else
    npc->rect = &rc_right[npc->anim];
}

// Chalkboard
void npc_act_043(npc_t *npc) {
  static const rect_t rc_left = {128, 80, 168, 112};
  static const rect_t rc_right = {168, 80, 208, 112};

  switch (npc->act) {
    case 0:
      npc->act = 1;
      npc->y -= 1 * 0x10 * 0x200;
      break;
  }

  if (npc->dir == 0)
    npc->rect = &rc_left;
  else
    npc->rect = &rc_right;
}

// Polish
void npc_act_044(npc_t *npc) {
  // Yeah, Pixel defined these backwards for some reason.
  static const rect_t rc_right[3] = {
    {0, 0, 32, 32},
    {32, 0, 64, 32},
    {64, 0, 96, 32},
  };

  static const rect_t rc_left[3] = {
    {0, 0, 32, 32},
    {96, 0, 128, 32},
    {128, 0, 160, 32},
  };

  switch (npc->act) {
    case 0:
      npc->act = 1;
      // Fallthrough
    case 1:
      npc->anim = 0;

      if (npc->dir == 0)
        npc->act = 8;
      else
        npc->act = 2;
      // Fallthrough
    case 2:
      npc->yvel += 0x20;

      if (npc->yvel > 0 && npc->flags & 8) {
        npc->yvel = -0x100;
        npc->xvel += 0x100;
      }

      if (npc->flags & 4) npc->act = 3;

      break;

    case 3:
      npc->xvel += 0x20;

      if (npc->xvel > 0 && npc->flags & 4) {
        npc->xvel = -0x100;
        npc->yvel -= 0x100;
      }

      if (npc->flags & 2) npc->act = 4;

      break;

    case 4:
      npc->yvel -= 0x20;

      if (npc->yvel < 0 && npc->flags & 2) {
        npc->yvel = 0x100;
        npc->xvel -= 0x100;
      }

      if (npc->flags & 1) npc->act = 5;

      break;

    case 5:
      npc->xvel -= 0x20;

      if (npc->xvel < 0 && npc->flags & 1) {
        npc->xvel = 0x100;
        npc->yvel += 0x100;
      }

      if (npc->flags & 8) npc->act = 2;

      break;

    case 6:
      npc->yvel += 0x20;

      if (npc->yvel > 0 && npc->flags & 8) {
        npc->yvel = -0x100;
        npc->xvel -= 0x100;
      }

      if (npc->flags & 1) npc->act = 7;

      break;

    case 7:
      npc->xvel -= 0x20;

      if (npc->xvel < 0 && npc->flags & 1) {
        npc->xvel = 0x100;
        npc->yvel -= 0x100;
      }

      if (npc->flags & 2) npc->act = 8;

      break;

    case 8:
      npc->yvel -= 0x20;

      if (npc->yvel < 0 && npc->flags & 2) {
        npc->yvel = 0x100;
        npc->xvel += 0x100;
      }

      if (npc->flags & 4) npc->act = 9;

      break;

    case 9:
      npc->xvel += 0x20;

      if (npc->xvel > 0 && npc->flags & 4) {
        npc->xvel = -0x100;
        npc->yvel += 0x100;
      }

      if (npc->flags & 8) npc->act = 6;

      break;
  }

  if (npc->life <= 100) {
    int i;

    for (i = 0; i < 10; ++i) npc_spawn(45, npc->x, npc->y, 0, 0, 0, NULL, 0x100);

    npc_spawn_death_fx(npc->x, npc->y, npc->view.back, 8, 0);
    snd_play_sound(-1, 25, SOUND_MODE_PLAY);
    npc->cond = 0;
  }

  if (npc->xvel > 0x200) npc->xvel = 0x200;
  if (npc->xvel < -0x200) npc->xvel = -0x200;

  if (npc->yvel > 0x200) npc->yvel = 0x200;
  if (npc->yvel < -0x200) npc->yvel = -0x200;

  if (npc->shock) {
    npc->x += npc->xvel / 2;
    npc->y += npc->yvel / 2;
  } else {
    npc->x += npc->xvel;
    npc->y += npc->yvel;
  }

  if (npc->act >= 2 && npc->act <= 9 && ++npc->anim > 2) npc->anim = 1;

  if (npc->dir == 0)
    npc->rect = &rc_left[npc->anim];
  else
    npc->rect = &rc_right[npc->anim];
}

// Baby
void npc_act_045(npc_t *npc) {
  static const rect_t rect[3] = {
    {0, 32, 16, 48},
    {16, 32, 32, 48},
    {32, 32, 48, 48},
  };

  switch (npc->act) {
    case 0:
      npc->act = 2;

      if (m_rand(0, 1))
        npc->xvel = m_rand(-0x200, -0x100);
      else
        npc->xvel = m_rand(0x100, 0x200);

      if (m_rand(0, 1))
        npc->yvel = m_rand(-0x200, -0x100);
      else
        npc->yvel = m_rand(0x100, 0x200);

      npc->xvel2 = npc->xvel;
      npc->yvel2 = npc->yvel;
      // Fallthrough
    case 1:
    case 2:
      if (++npc->anim > 2) npc->anim = 1;

      break;
  }

  if (npc->xvel2 < 0 && npc->flags & 1) npc->xvel2 *= -1;
  if (npc->xvel2 > 0 && npc->flags & 4) npc->xvel2 *= -1;

  if (npc->yvel2 < 0 && npc->flags & 2) npc->yvel2 *= -1;
  if (npc->yvel2 > 0 && npc->flags & 8) npc->yvel2 *= -1;

  if (npc->xvel2 > 0x200) npc->xvel2 = 0x200;
  if (npc->xvel2 < -0x200) npc->xvel2 = -0x200;

  if (npc->yvel2 > 0x200) npc->yvel2 = 0x200;
  if (npc->yvel2 < -0x200) npc->yvel2 = -0x200;

  if (npc->shock) {
    npc->x += npc->xvel2 / 2;
    npc->y += npc->yvel2 / 2;
  } else {
    npc->x += npc->xvel2;
    npc->y += npc->yvel2;
  }

  npc->rect = &rect[npc->anim];
}

// H/V Trigger
void npc_act_046(npc_t *npc) {
  static const rect_t rect = {0, 0, 16, 16};

  npc->bits |= NPC_EVENT_WHEN_TOUCHED;

  if (npc->dir == 0) {
    if (npc->x < player.x)
      npc->x += 0x5FF;
    else
      npc->x -= 0x5FF;
  } else {
    if (npc->y < player.y)
      npc->y += 0x5FF;
    else
      npc->y -= 0x5FF;
  }

  npc->rect = &rect;
}

// Sandcroc
void npc_act_047(npc_t *npc) {
  switch (npc->act) {
    case 0:
      npc->anim = 0;
      npc->act = 1;
      npc->act_wait = 0;
      npc->tgt_y = npc->y;
      npc->bits &= ~NPC_SHOOTABLE;
      npc->bits &= ~NPC_INVULNERABLE;
      npc->bits &= ~NPC_SOLID_SOFT;
      npc->bits &= ~NPC_IGNORE_SOLIDITY;
      // Fallthrough
    case 1:
      if (player.x > npc->x - (8 * 0x200) && player.x < npc->x + (8 * 0x200) && player.y > npc->y &&
          player.y < npc->y + (8 * 0x200)) {
        npc->act = 2;
        npc->act_wait = 0;
        snd_play_sound(-1, 102, SOUND_MODE_PLAY);
      }

      if (npc->x < player.x) npc->x += 2 * 0x200;

      if (npc->x > player.x) npc->x -= 2 * 0x200;

      break;

    case 2:
      if (++npc->anim_wait > 3) {
        ++npc->anim;
        npc->anim_wait = 0;
      }

      if (npc->anim == 3) npc->damage = 10;

      if (npc->anim == 4) {
        npc->bits |= NPC_SHOOTABLE;
        npc->act = 3;
        npc->act_wait = 0;
      }

      break;

    case 3:
      npc->bits |= NPC_SOLID_SOFT;
      npc->damage = 0;
      ++npc->act_wait;

      if (npc->shock) {
        npc->act = 4;
        npc->act_wait = 0;
      }

      break;

    case 4:
      npc->bits |= NPC_IGNORE_SOLIDITY;
      npc->y += 1 * 0x200;

      if (++npc->act_wait == 32) {
        npc->bits &= ~NPC_SOLID_SOFT;
        npc->bits &= ~NPC_SHOOTABLE;
        npc->act = 5;
        npc->act_wait = 0;
      }

      break;

    case 5:
      if (npc->act_wait < 100) {
        ++npc->act_wait;
      } else {
        npc->y = npc->tgt_y;
        npc->anim = 0;
        npc->act = 0;
      }

      break;
  }

  static const rect_t rect[5] = {
    {0, 48, 48, 80}, {48, 48, 96, 80}, {96, 48, 144, 80},
    {144, 48, 192, 80}, {192, 48, 240, 80},
  };

  npc->rect = &rect[npc->anim];
}

// Omega projectiles
void npc_act_048(npc_t *npc) {
  if (npc->flags & 1 && npc->xvel < 0) {
    npc->xvel *= -1;
  } else if (npc->flags & 4 && npc->xvel > 0) {
    npc->xvel *= -1;
  } else if (npc->flags & 8) {
    if (++npc->count1 > 2 || npc->dir == 2) {
      npc_show_death_damage(npc);
      caret_spawn(npc->x, npc->y, CARET_PROJECTILE_DISSIPATION, DIR_LEFT);
    } else {
      npc->yvel = -0x100;
    }
  }

  if (npc->dir == 2) {
    npc->bits &= ~NPC_SHOOTABLE;
    npc->bits |= NPC_INVULNERABLE;
  }

  npc->yvel += 5;
  npc->y += npc->yvel;
  npc->x += npc->xvel;

  static const rect_t rc_left[2] = {
    {288, 88, 304, 104},
    {304, 88, 320, 104},
  };

  static const rect_t rc_right[2] = {
    {288, 104, 304, 120},
    {304, 104, 320, 120},
  };

  if (++npc->anim_wait > 2) {
    npc->anim_wait = 0;
    if (++npc->anim > 1) npc->anim = 0;
  }

  if (++npc->act_wait > 750) {
    caret_spawn(npc->x, npc->y, CARET_PROJECTILE_DISSIPATION, DIR_LEFT);
    npc->cond = 0;
  }

  if (npc->dir == 0)
    npc->rect = &rc_left[npc->anim];
  else
    npc->rect = &rc_right[npc->anim];
}

// Skullhead
void npc_act_049(npc_t *npc) {
  u8 deg;
  int xm, ym;

  if (npc->act >= 10 && npc->parent->class_num == 3) {
    npc->act = 3;
    npc->xvel = 0;
    npc->yvel = 0;
    npc->count2 = 1;
  }

  if (npc->flags & 1) {
    npc->dir = 2;
    npc->xvel = 0x100;
  }

  if (npc->flags & 4) {
    npc->dir = 0;
    npc->xvel = -0x100;
  }

  switch (npc->act) {
    case 0:
      if (npc->parent != NULL)
        npc->act = 10;
      else
        npc->act = 1;
      // Fallthrough
    case 1:
      if (++npc->act_wait > 3) {
        npc->yvel = -0x400;
        npc->act = 3;
        npc->anim = 2;

        if (npc->count2 != 0) {
          if (npc->dir == 0)
            npc->xvel = -0x200;
          else
            npc->xvel = 0x200;
        } else {
          if (npc->dir == 0)
            npc->xvel = -0x100;
          else
            npc->xvel = 0x100;
        }
      }

      npc->anim = 1;
      break;

    case 3:
      if (npc->flags & 8) {
        npc->act = 1;
        npc->act_wait = 0;
        npc->xvel = 0;
      }

      if (npc->flags & 8 || npc->yvel > 0)
        npc->anim = 1;
      else
        npc->anim = 2;

      break;

    case 10:
      if (npc->count1 < 50) {
        ++npc->count1;
      } else {
        if (npc->x - (128 * 0x200) < player.x && npc->x + (128 * 0x200) > player.x && npc->y - (96 * 0x200) < player.y &&
            npc->y + (96 * 0x200) > player.y) {
          npc->act = 11;
          npc->act_wait = 0;
          npc->anim = 2;
        }
      }

      break;

    case 11:
      if (++npc->act_wait == 30 || npc->act_wait == 35) {
        deg = m_atan2(npc->x - player.x, npc->y + (4 * 0x200) - player.y);
        ym = m_sin(deg) * 2;
        xm = m_cos(deg) * 2;
        npc_spawn(50, npc->x, npc->y, xm, ym, 0, NULL, 0x100);
        snd_play_sound(-1, 39, SOUND_MODE_PLAY);
      }

      if (npc->act_wait > 50) {
        npc->count1 = 0;
        npc->act = 10;
        npc->anim = 1;
      }

      break;
  }

  if (npc->act >= 10) {
    npc->x = npc->parent->x;
    npc->y = npc->parent->y + (16 * 0x200);
    npc->dir = npc->parent->dir;
    --npc->parent->count1;
  }

  npc->yvel += 0x40;
  if (npc->yvel > 0x5FF) npc->yvel = 0x5FF;

  npc->x += npc->xvel;
  npc->y += npc->yvel;

  static const rect_t rc_left[3] = {
    {0, 80, 32, 104},
    {32, 80, 64, 104},
    {64, 80, 96, 104},
  };

  static const rect_t rc_right[3] = {
    {0, 104, 32, 128},
    {32, 104, 64, 128},
    {64, 104, 96, 128},
  };

  if (npc->dir == 0)
    npc->rect = &rc_left[npc->anim];
  else
    npc->rect = &rc_right[npc->anim];
}

// Skeleton projectile
void npc_act_050(npc_t *npc) {
  switch (npc->act) {
    case 0:
      if (npc->dir == 2) npc->act = 2;
      // Fallthrough
    case 1:
      npc->x += npc->xvel;
      npc->y += npc->yvel;

      if (npc->flags & 1) {
        npc->act = 2;
        npc->xvel = 0x200;
        ++npc->count1;
      }

      if (npc->flags & 4) {
        npc->act = 2;
        npc->xvel = -0x200;
        ++npc->count1;
      }

      if (npc->flags & 2) {
        npc->act = 2;
        npc->yvel = 0x200;
        ++npc->count1;
      }

      if (npc->flags & 8) {
        npc->act = 2;
        npc->yvel = -0x200;
        ++npc->count1;
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

  static const rect_t rect[4] = {
    {48, 32, 64, 48},
    {64, 32, 80, 48},
    {80, 32, 96, 48},
    {96, 32, 112, 48},
  };

  if (npc->dir == 0) {
    if (++npc->anim_wait > 1) {
      npc->anim_wait = 0;
      ++npc->anim;
    }

    if (npc->anim > 3) npc->anim = 0;
  } else {
    if (++npc->anim_wait > 1) {
      npc->anim_wait = 0;
      --npc->anim;
    }

    if (npc->anim < 0) npc->anim = 3;
  }

  npc->rect = &rect[npc->anim];
}

// Crow & Skullhead
void npc_act_051(npc_t *npc) {
  switch (npc->act) {
    case 0:
      if (npc->x - (((VID_WIDTH / 2) + 160) * 0x200) < player.x &&
          npc->x + (((VID_WIDTH / 2) + 160) * 0x200) > player.x &&
          npc->y - (((VID_HEIGHT / 2) + 200) * 0x200) < player.y &&
          npc->y + (((VID_HEIGHT / 2) + 200) * 0x200) > player.y) {
        npc->tgt_x = npc->x;
        npc->tgt_y = npc->y;

        if (npc->dir == 0)  // Completely redundant as both the conditions are the same
          npc->yvel = 0x400;
        else
          npc->yvel = 0x400;

        npc->act = 1;
        npc_spawn(49, 0, 0, 0, 0, 0, npc, 0);
      } else {
        break;
      }

      // Fallthrough
    case 1:
      if (player.x < npc->x)
        npc->dir = 0;
      else
        npc->dir = 2;

      if (npc->tgt_y < npc->y) npc->yvel -= 10;
      if (npc->tgt_y > npc->y) npc->yvel += 10;

      if (npc->yvel > 0x200) npc->yvel = 0x200;
      if (npc->yvel < -0x200) npc->yvel = -0x200;

      if (npc->count1 < 10)
        ++npc->count1;
      else
        npc->act = 2;

      break;

    case 2:
      if (player.x < npc->x)
        npc->dir = 0;
      else
        npc->dir = 2;

      if (npc->y > player.y + (32 * 0x200)) {
        if (player.x < npc->x) npc->xvel += 0x10;
        if (player.x > npc->x) npc->xvel -= 0x10;
      } else {
        if (player.x < npc->x) npc->xvel -= 0x10;
        if (player.x > npc->x) npc->xvel += 0x10;
      }

      if (player.y < npc->y) npc->yvel -= 0x10;
      if (player.y > npc->y) npc->yvel += 0x10;

      if (npc->shock) {
        npc->yvel += 0x20;
        npc->xvel = 0;
      }

      break;
  }

  if (npc->xvel < 0 && npc->flags & 1) npc->xvel = 0x100;
  if (npc->xvel > 0 && npc->flags & 4) npc->xvel = -0x100;

  if (npc->yvel < 0 && npc->flags & 2) npc->yvel = 0x100;
  if (npc->yvel > 0 && npc->flags & 8) npc->yvel = -0x100;

  if (npc->xvel > 0x400) npc->xvel = 0x400;
  if (npc->xvel < -0x400) npc->xvel = -0x400;

  if (npc->yvel > 0x200) npc->yvel = 0x200;
  if (npc->yvel < -0x200) npc->yvel = -0x200;

  npc->x += npc->xvel;
  npc->y += npc->yvel;

  static const rect_t rect_left[5] = {
    {96, 80, 128, 112}, {128, 80, 160, 112}, {160, 80, 192, 112},
    {192, 80, 224, 112}, {224, 80, 256, 112},
  };

  static const rect_t rect_right[5] = {
    {96, 112, 128, 144}, {128, 112, 160, 144}, {160, 112, 192, 144},
    {192, 112, 224, 144}, {224, 112, 256, 144},
  };

  if (npc->shock) {
    npc->anim = 4;
  } else if (npc->act == 2 && npc->y < player.y - (32 * 0x200)) {
    npc->anim = 0;
  } else {
    if (npc->act != 0) {
      if (++npc->anim_wait > 1) {
        npc->anim_wait = 0;
        ++npc->anim;
      }

      if (npc->anim > 1) npc->anim = 0;
    }
  }

  if (npc->dir == 0)
    npc->rect = &rect_left[npc->anim];
  else
    npc->rect = &rect_right[npc->anim];
}

// Blue robot (sitting)
void npc_act_052(npc_t *npc) {
  static const rect_t rect = {240, 96, 256, 112};

  npc->rect = &rect;
}

// Skullstep leg
void npc_act_053(npc_t *npc) {
  u8 deg;

  static const rect_t rc_left[2] = {
    {0, 128, 24, 144},
    {24, 128, 48, 144},
  };

  static const rect_t rc_right[2] = {
    {48, 128, 72, 144},
    {72, 128, 96, 144},
  };

  if (npc->parent->class_num == 3) {
    npc_show_death_damage(npc);
    npc_spawn_death_fx(npc->x, npc->y, npc->view.back, 4, 0);
    return;
  }

  switch (npc->act) {
    case 0:
      npc->act = 1;
      npc->count1 = 10;
      // Fallthrough
    case 1:
      if (npc->dir == 0 && npc->flags & 0x20) {
        npc->parent->y -= 2 * 0x200;
        npc->parent->yvel -= 0x100;
      }

      if (npc->dir == 2 && npc->flags & 0x10) {
        npc->parent->y -= 2 * 0x200;
        npc->parent->yvel -= 0x100;
      }

      if (npc->flags & 8) {
        npc->parent->y -= 2 * 0x200;
        npc->parent->yvel -= 0x100;

        if (npc->parent->dir == 0)
          npc->parent->xvel -= 0x80;
        else
          npc->parent->xvel += 0x80;
      }

      deg = (u8)npc->xvel + (u8)npc->parent->count2;
      npc->x = npc->parent->x + npc->count1 * m_cos(deg);
      npc->y = npc->parent->y + npc->count1 * m_sin(deg);

      npc->dir = npc->parent->dir;

      break;
  }

  npc->dir = npc->parent->dir;

  if (deg >= 20 && deg <= 108)
    npc->anim = 0;
  else
    npc->anim = 1;

  if (npc->dir == 0)
    npc->rect = &rc_left[npc->anim];
  else
    npc->rect = &rc_right[npc->anim];
}

// Skullstep
void npc_act_054(npc_t *npc) {
  static const rect_t rc_left[3] = {
    {0, 80, 32, 104},
    {32, 80, 64, 104},
    {64, 80, 96, 104},
  };

  static const rect_t rc_right[3] = {
    {0, 104, 32, 128},
    {32, 104, 64, 128},
    {64, 104, 96, 128},
  };

  u8 deg;

  switch (npc->act) {
    case 0:
      npc_spawn(53, 0, 0, 0, 0, npc->dir, npc, 0x100);
      npc_spawn(53, 0, 0, 128, 0, npc->dir, npc, 0);
      npc->act = 1;
      npc->anim = 1;
      // Fallthrough
    case 1:
      deg = npc->count2;

      if (npc->dir == 0)
        deg -= 6;
      else
        deg += 6;

      npc->count2 = deg;

      if (npc->flags & 8) {
        npc->xvel = (npc->xvel * 3) / 4;

        if (++npc->act_wait > 60) {
          npc->act = 2;
          npc->act_wait = 0;
        }
      } else {
        npc->act_wait = 0;
      }

      if (npc->dir == 0 && npc->flags & 1) {
        if (++npc->count1 > 8) {
          npc->dir = 2;
          npc->xvel *= -1;
        }
      } else if (npc->dir == 2 && npc->flags & 4) {
        if (++npc->count1 > 8) {
          npc->dir = 0;
          npc->xvel *= -1;
        }
      } else {
        npc->count1 = 0;
      }

      break;

    case 2:
      ++npc->act_wait;
      npc->shock += (u8)npc->act_wait;

      if (npc->act_wait > 50) {
        npc_show_death_damage(npc);
        npc_spawn_death_fx(npc->x, npc->y, npc->view.back, 8, 0);
        snd_play_sound(-1, 25, SOUND_MODE_PLAY);
      }

      break;
  }

  npc->yvel += 0x80;

  if (npc->xvel > 0x2FF) npc->xvel = 0x2FF;
  if (npc->xvel < -0x2FF) npc->xvel = -0x2FF;

  if (npc->yvel > 0x2FF) npc->yvel = 0x2FF;
  if (npc->yvel < -0x2FF) npc->yvel = -0x2FF;

  npc->x += npc->xvel;
  npc->y += npc->yvel;

  if (npc->dir == 0)
    npc->rect = &rc_left[npc->anim];
  else
    npc->rect = &rc_right[npc->anim];
}

// Kazuma
void npc_act_055(npc_t *npc) {
  static const rect_t rc_left[6] = {
    {192, 192, 208, 216}, {208, 192, 224, 216}, {192, 192, 208, 216},
    {224, 192, 240, 216}, {192, 192, 208, 216}, {240, 192, 256, 216},
  };

  static const rect_t rc_right[6] = {
    {192, 216, 208, 240}, {208, 216, 224, 240}, {192, 216, 208, 240},
    {224, 216, 240, 240}, {192, 216, 208, 240}, {240, 216, 256, 240},
  };

  switch (npc->act) {
    case 0:
      npc->act = 1;
      npc->anim = 0;
      npc->anim_wait = 0;
      break;

    case 3:
      npc->act = 4;
      npc->anim = 1;
      npc->anim_wait = 0;
      // Fallthrough
    case 4:
      if (++npc->anim_wait > 4) {
        npc->anim_wait = 0;
        ++npc->anim;
      }

      if (npc->anim > 4) npc->anim = 1;

      if (npc->dir == 0)
        npc->x -= 1 * 0x200;
      else
        npc->x += 1 * 0x200;

      break;

    case 5:
      npc->anim = 5;
      break;
  }

  npc->yvel += 0x20;
  if (npc->yvel > 0x5FF) npc->yvel = 0x5FF;

  npc->y += npc->yvel;

  if (npc->dir == 0)
    npc->rect = &rc_left[npc->anim];
  else
    npc->rect = &rc_right[npc->anim];
}

// Beetle (Sand Zone)
void npc_act_056(npc_t *npc) {
  static const rect_t rc_left[3] = {
    {0, 144, 16, 160},
    {16, 144, 32, 160},
    {32, 144, 48, 160},
  };

  static const rect_t rc_right[3] = {
    {0, 160, 16, 176},
    {16, 160, 32, 176},
    {32, 160, 48, 176},
  };

  switch (npc->act) {
    case 0:
      npc->act = 1;

      if (npc->dir == 0)
        npc->act = 1;
      else
        npc->act = 3;

      break;

    case 1:
      npc->xvel -= 0x10;

      if (npc->xvel < -0x400) npc->xvel = -0x400;

      if (npc->shock)
        npc->x += npc->xvel / 2;
      else
        npc->x += npc->xvel;

      if (++npc->anim_wait > 1) {
        npc->anim_wait = 0;
        ++npc->anim;
      }

      if (npc->anim > 2) npc->anim = 1;

      if (npc->flags & 1) {
        npc->act = 2;
        npc->act_wait = 0;
        npc->anim = 0;
        npc->xvel = 0;
        npc->dir = 2;
      }

      break;

    case 2:
      if (npc->x < player.x && npc->x > player.x - (16 * 0x10 * 0x200) && npc->y < player.y + (8 * 0x200) &&
          npc->y > player.y - (8 * 0x200)) {
        npc->act = 3;
        npc->anim_wait = 0;
        npc->anim = 1;
      }

      break;

    case 3:
      npc->xvel += 0x10;

      if (npc->xvel > 0x400) npc->xvel = 0x400;

      if (npc->shock)
        npc->x += npc->xvel / 2;
      else
        npc->x += npc->xvel;

      if (++npc->anim_wait > 1) {
        npc->anim_wait = 0;
        ++npc->anim;
      }

      if (npc->anim > 2) npc->anim = 1;

      if (npc->flags & 4) {
        npc->act = 4;
        npc->act_wait = 0;
        npc->anim = 0;
        npc->xvel = 0;
        npc->dir = 0;
      }

      break;

    case 4:
      if (npc->x < player.x + (16 * 0x10 * 0x200) && npc->x > player.x && npc->y < player.y + (8 * 0x200) &&
          npc->y > player.y - (8 * 0x200)) {
        npc->act = 1;
        npc->anim_wait = 0;
        npc->anim = 1;
      }

      break;
  }

  if (npc->dir == 0)
    npc->rect = &rc_left[npc->anim];
  else
    npc->rect = &rc_right[npc->anim];
}

// Crow
void npc_act_057(npc_t *npc) {
  u8 deg;

  switch (npc->act) {
    case 0:
      deg = m_rand(0, 0xFF);
      npc->xvel = m_cos(deg);
      deg += 0x40;
      npc->tgt_x = npc->x + (m_cos(deg) * 8);

      deg = m_rand(0, 0xFF);
      npc->yvel = m_sin(deg);
      deg += 0x40;
      npc->tgt_y = npc->y + (m_sin(deg) * 8);

      npc->act = 1;
      npc->count1 = 120;
      npc->anim = m_rand(0, 1);
      npc->anim_wait = m_rand(0, 4);
      // Fallthrough
    case 1:
      if (player.x < npc->x)
        npc->dir = 0;
      else
        npc->dir = 2;

      if (npc->tgt_x < npc->x) npc->xvel -= 0x10;
      if (npc->tgt_x > npc->x) npc->xvel += 0x10;

      if (npc->tgt_y < npc->y) npc->yvel -= 0x10;
      if (npc->tgt_y > npc->y) npc->yvel += 0x10;

      if (npc->xvel > 0x200) npc->xvel = 0x200;
      if (npc->xvel < -0x200) npc->xvel = -0x200;

      if (npc->yvel > 0x200) npc->yvel = 0x200;
      if (npc->yvel < -0x200) npc->yvel = -0x200;

      if (npc->shock) {
        npc->act = 2;
        npc->act_wait = 0;

        if (npc->dir == 2)
          npc->xvel = -0x200;
        else
          npc->xvel = 0x200;

        npc->yvel = 0;
      }

      break;

    case 2:
      if (player.x < npc->x)
        npc->dir = 0;
      else
        npc->dir = 2;

      if (npc->y > player.y + (48 * 0x200)) {
        if (player.x < npc->x) npc->xvel += 0x10;
        if (player.x > npc->x) npc->xvel -= 0x10;
      } else {
        if (player.x < npc->x) npc->xvel -= 0x10;
        if (player.x > npc->x) npc->xvel += 0x10;
      }

      if (player.y < npc->y) npc->yvel -= 0x10;
      if (player.y > npc->y) npc->yvel += 0x10;

      if (npc->shock) {
        npc->yvel += 0x20;
        npc->xvel = 0;
      }

      if (npc->xvel < 0 && npc->flags & 1) npc->xvel = 0x200;
      if (npc->xvel > 0 && npc->flags & 4) npc->xvel = -0x200;

      if (npc->yvel < 0 && npc->flags & 2) npc->yvel = 0x200;
      if (npc->yvel > 0 && npc->flags & 8) npc->yvel = -0x200;

      if (npc->xvel > 0x5FF) npc->xvel = 0x5FF;
      if (npc->xvel < -0x5FF) npc->xvel = -0x5FF;

      if (npc->yvel > 0x5FF) npc->yvel = 0x5FF;
      if (npc->yvel < -0x5FF) npc->yvel = -0x5FF;

      break;
  }

  npc->x += npc->xvel;
  npc->y += npc->yvel;

  static const rect_t rect_left[5] = {
    {96, 80, 128, 112}, {128, 80, 160, 112}, {160, 80, 192, 112},
    {192, 80, 224, 112}, {224, 80, 256, 112},
  };

  static const rect_t rect_right[5] = {
    {96, 112, 128, 144}, {128, 112, 160, 144}, {160, 112, 192, 144},
    {192, 112, 224, 144}, {224, 112, 256, 144},
  };

  if (npc->shock) {
    npc->anim = 4;
  } else {
    if (++npc->anim_wait > 1) {
      npc->anim_wait = 0;
      ++npc->anim;
    }

    if (npc->anim > 1) npc->anim = 0;
  }

  if (npc->dir == 0)
    npc->rect = &rect_left[npc->anim];
  else
    npc->rect = &rect_right[npc->anim];
}

// Basu (Egg Corridor)
void npc_act_058(npc_t *npc) {
  static const rect_t rc_left[3] = {
    {192, 0, 216, 24},
    {216, 0, 240, 24},
    {240, 0, 264, 24},
  };

  static const rect_t rc_right[3] = {
    {192, 24, 216, 48},
    {216, 24, 240, 48},
    {240, 24, 264, 48},
  };

  switch (npc->act) {
    case 0:
      if (player.x < npc->x + (16 * 0x200) && player.x > npc->x - (16 * 0x200)) {
        npc->bits |= NPC_SHOOTABLE;
        npc->yvel = -0x100;
        npc->tgt_x = npc->x;
        npc->tgt_y = npc->y;
        npc->act = 1;
        npc->act_wait = 0;
        npc->count1 = npc->dir;
        npc->count2 = 0;
        npc->damage = 6;

        if (npc->dir == 0) {
          npc->x = player.x + (16 * 0x10 * 0x200);
          npc->xvel = -0x2FF;
        } else {
          npc->x = player.x - (16 * 0x10 * 0x200);
          npc->xvel = 0x2FF;
        }

        return;
      }

      npc->rect = NULL;
      npc->damage = 0;
      npc->xvel = 0;
      npc->yvel = 0;
      npc->bits &= ~NPC_SHOOTABLE;

      return;

    case 1:
      if (npc->x > player.x) {
        npc->dir = 0;
        npc->xvel -= 0x10;
      } else {
        npc->dir = 2;
        npc->xvel += 0x10;
      }

      if (npc->flags & 1) npc->xvel = 0x200;

      if (npc->flags & 4) npc->xvel = -0x200;

      if (npc->y < npc->tgt_y)
        npc->yvel += 8;
      else
        npc->yvel -= 8;

      if (npc->xvel > 0x2FF) npc->xvel = 0x2FF;
      if (npc->xvel < -0x2FF) npc->xvel = -0x2FF;

      if (npc->yvel > 0x100) npc->yvel = 0x100;
      if (npc->yvel < -0x100) npc->yvel = -0x100;

      if (npc->shock) {
        npc->x += npc->xvel / 2;
        npc->y += npc->yvel / 2;
      } else {
        npc->x += npc->xvel;
        npc->y += npc->yvel;
      }

      if (player.x > npc->x + (400 * 0x200) ||
          player.x < npc->x - (400 * 0x200))  // TODO: Maybe do something about this for widescreen
      {
        npc->act = 0;
        npc->xvel = 0;
        npc->dir = npc->count1;
        npc->x = npc->tgt_x;
        npc->rect = NULL;
        npc->damage = 0;
        return;
      }

      break;
  }

  if (npc->act != 0)  // This is always true
  {
    if (npc->act_wait < 150) ++npc->act_wait;

    if (npc->act_wait == 150) {
      if ((++npc->count2 % 8) == 0 && npc->x < player.x + (160 * 0x200) &&
          npc->x > player.x - (160 * 0x200))  // TODO: Maybe do something about this for widescreen
      {
        u8 deg;
        int xm;
        int ym;

        deg = m_atan2(npc->x - player.x, npc->y - player.y);
        deg += (u8)m_rand(-6, 6);
        ym = m_sin(deg) * 2;
        xm = m_cos(deg) * 2;
        npc_spawn(84, npc->x, npc->y, xm, ym, 0, NULL, 0x100);
        snd_play_sound(-1, 39, SOUND_MODE_PLAY);
      }

      if (npc->count2 > 8) {
        npc->act_wait = 0;
        npc->count2 = 0;
      }
    }
  }

  if (++npc->anim_wait > 1) {
    npc->anim_wait = 0;
    ++npc->anim;
  }

  if (npc->anim > 1) npc->anim = 0;

  if (npc->act_wait > 120 && npc->act_wait / 2 % 2 == 1 && npc->anim == 1) npc->anim = 2;

  if (npc->dir == 0)
    npc->rect = &rc_left[npc->anim];
  else
    npc->rect = &rc_right[npc->anim];
}

// Eye door
void npc_act_059(npc_t *npc) {
  static const rect_t rc_left[4] = {
    {224, 16, 240, 40},
    {208, 80, 224, 104},
    {224, 80, 240, 104},
    {240, 80, 256, 104},
  };

  switch (npc->act) {
    case 0:
      npc->act = 1;
      // Fallthrough
    case 1:
      if (npc->x - (64 * 0x200) < player.x && npc->x + (64 * 0x200) > player.x && npc->y - (64 * 0x200) < player.y &&
          npc->y + (64 * 0x200) > player.y) {
        npc->act = 2;
        npc->anim_wait = 0;
      }

      break;

    case 2:
      if (++npc->anim_wait > 2) {
        npc->anim_wait = 0;
        ++npc->anim;
      }

      if (npc->anim == 2) npc->act = 3;

      break;

    case 3:
      if (npc->x - (64 * 0x200) < player.x && npc->x + (64 * 0x200) > player.x && npc->y - (64 * 0x200) < player.y &&
          npc->y + (64 * 0x200) > player.y) {
        // There probably used to be some commented-out code here
      } else {
        npc->act = 4;
        npc->anim_wait = 0;
      }

      break;

    case 4:
      if (++npc->anim_wait > 2) {
        npc->anim_wait = 0;
        --npc->anim;
      }

      if (npc->anim == 0) npc->act = 1;

      break;
  }

  if (npc->shock)
    npc->rect = &rc_left[3];
  else
    npc->rect = &rc_left[npc->anim];
}
