#include "game/npc_act/npc_act.h"

// Demon crown (opening)
void npc_act_300(npc_t *npc) {
  static const rect_t rc = {192, 80, 208, 96};

  if (npc->act == 0) {
    npc->act = 1;
    npc->y += 6 * 0x200;
  }

  if (++npc->anim_wait % 8 == 1)
    caret_spawn(npc->x + (m_rand(-8, 8) * 0x200), npc->y + (8 * 0x200), CARET_TINY_PARTICLES, DIR_UP);

  npc->rect = &rc;
}

// Fish missile (Misery)
void npc_act_301(npc_t *npc) {
  int dir;

  static const rect_t rect[8] = {
    {144, 0, 160, 16},  {160, 0, 176, 16},  {176, 0, 192, 16},  {192, 0, 208, 16},
    {144, 16, 160, 32}, {160, 16, 176, 32}, {176, 16, 192, 32}, {192, 16, 208, 32},
  };

  switch (npc->act) {
    case 0:
      npc->act = 1;
      npc->count1 = npc->dir;
      // Fallthrough
    case 1:
      npc->xvel = m_cos(npc->count1) * 2;
      npc->yvel = m_sin(npc->count1) * 2;

      npc->y += npc->yvel;
      npc->x += npc->xvel;

      dir = m_atan2(npc->x - player.x, npc->y - player.y);

      if (dir < npc->count1) {
        if (npc->count1 - dir < 0x80)
          --npc->count1;
        else
          ++npc->count1;
      } else {
        if (dir - npc->count1 < 0x80)
          ++npc->count1;
        else
          --npc->count1;
      }

      if (npc->count1 > 0xFF) npc->count1 -= 0x100;
      if (npc->count1 < 0) npc->count1 += 0x100;

      break;
  }

  if (++npc->anim_wait > 2) {
    npc->anim_wait = 0;
    caret_spawn(npc->x, npc->y, CARET_EXHAUST, DIR_AUTO);
  }

  npc->anim = (npc->count1 + 0x10) / 0x20;
  if (npc->anim > 7) npc->anim = 7;

  npc->rect = &rect[npc->anim];
}

// Camera focus marker
void npc_act_302(npc_t *npc) {
  int n;

  switch (npc->act) {
    case 10:
      npc->x = player.x;
      npc->y = player.y - 32 * 0x200;
      break;

    case 20:
      switch (npc->dir) {
        case 0:
          npc->x -= 2 * 0x200;
          break;

        case 1:
          npc->y -= 2 * 0x200;
          break;

        case 2:
          npc->x += 2 * 0x200;
          break;

        case 3:
          npc->y += 2 * 0x200;
          break;
      }

      player.x = npc->x;
      player.y = npc->y;
      break;

    case 30:
      npc->x = player.x;
      npc->y = player.y + (80 * 0x200);
      break;

    case 100:
      npc->act = 101;

      if (npc->dir != 0) {
        for (n = 0xAA; n <= npc_list_max; ++n) {
          if (npc_list[n].cond & 0x80 && npc_list[n].event_num == npc->dir) {
            npc->parent = &npc_list[n];
            break;
          }
        }

        if (n == NPC_MAX) {
          npc->cond = 0;
          break;
        }
      } else {
        npc->parent = npc_boss;
      }
      // Fallthrough
    case 101:
      npc->x = (player.x + npc->parent->x) / 2;
      npc->y = (player.y + npc->parent->y) / 2;
      break;
  }
}

// Curly's machine gun
void npc_act_303(npc_t *npc) {
  static const rect_t rc_left[2] = {
    {216, 152, 232, 168},
    {232, 152, 248, 168},
  };

  static const rect_t rc_right[2] = {
    {216, 168, 232, 184},
    {232, 168, 248, 184},
  };

  if (npc->parent == NULL) return;

  // Set position
  if (npc->parent->dir == 0) {
    npc->dir = 0;
    npc->x = npc->parent->x - (8 * 0x200);
  } else {
    npc->dir = 2;
    npc->x = npc->parent->x + (8 * 0x200);
  }

  npc->y = npc->parent->y;

  // Animation
  npc->anim = 0;
  if (npc->parent->anim == 3 || npc->parent->anim == 5) npc->y -= 1 * 0x200;

  // Set framerect
  if (npc->dir == 0)
    npc->rect = &rc_left[npc->anim];
  else
    npc->rect = &rc_right[npc->anim];
}

// Gaudi in hospital
void npc_act_304(npc_t *npc) {
  static const rect_t rc[4] = {
    {0, 176, 24, 192},
    {24, 176, 48, 192},
    {48, 176, 72, 192},
    {72, 176, 96, 192},
  };

  switch (npc->act) {
    case 0:
      npc->act = 1;
      npc->y += 10 * 0x200;
      // Fallthrough
    case 1:
      npc->anim = 0;
      break;

    case 10:
      npc->anim = 1;
      break;

    case 20:
      npc->act = 21;
      npc->anim = 2;
      // Fallthrough
    case 21:
      if (++npc->anim_wait > 10) {
        npc->anim_wait = 0;
        ++npc->anim;
      }

      if (npc->anim > 3) npc->anim = 2;

      break;
  }

  npc->rect = &rc[npc->anim];
}

// Small puppy
void npc_act_305(npc_t *npc) {
  static const rect_t rc_left[2] = {
    {160, 144, 176, 160},
    {176, 144, 192, 160},
  };

  static const rect_t rc_right[2] = {
    {160, 160, 176, 176},
    {176, 160, 192, 176},
  };

  switch (npc->act) {
    case 0:
      npc->act = 1;
      npc->y -= 16 * 0x200;
      npc->anim_wait = m_rand(0, 6);
      // Fallthrough

    case 1:
      if (++npc->anim_wait > 6) {
        npc->anim_wait = 0;
        ++npc->anim;
      }

      if (npc->anim > 1) npc->anim = 0;

      break;
  }

  if (npc->dir == 0)
    npc->rect = &rc_left[npc->anim];
  else
    npc->rect = &rc_right[npc->anim];
}

// Balrog (nurse)
void npc_act_306(npc_t *npc) {
  static const rect_t rc_left[2] = {
    {240, 96, 280, 128},
    {280, 96, 320, 128},
  };

  static const rect_t rc_right[2] = {
    {160, 152, 200, 184},
    {200, 152, 240, 184},
  };

  switch (npc->act) {
    case 0:
      npc->act = 1;
      npc->anim = 0;
      npc->anim_wait = 0;
      npc->y += 4 * 0x200;
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

// Caged Santa
void npc_act_307(npc_t *npc) {
  static const rect_t rc_left[2] = {
    {0, 32, 16, 48},
    {16, 32, 32, 48},
  };

  static const rect_t rc_right[2] = {
    {0, 48, 16, 64},
    {16, 48, 32, 64},
  };

  switch (npc->act) {
    case 0:
      npc->x += 1 * 0x200;
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

// Stumpy
void npc_act_308(npc_t *npc) {
  u8 deg;

  static const rect_t rc_left[2] = {
    {128, 112, 144, 128},
    {144, 112, 160, 128},
  };

  static const rect_t rc_right[2] = {
    {128, 128, 144, 144},
    {144, 128, 160, 144},
  };

  switch (npc->act) {
    case 0:
      npc->act = 1;
      // Fallthrough
    case 1:
      if (player.x < npc->x + (240 * 0x200) && player.x > npc->x - (240 * 0x200) && player.y < npc->y + (192 * 0x200) &&
          player.y > npc->y - (192 * 0x200))
        npc->act = 10;

      break;

    case 10:
      npc->act = 11;
      npc->act_wait = 0;
      npc->xvel2 = 0;
      npc->yvel2 = 0;

      if (npc->x > player.x)
        npc->dir = 0;
      else
        npc->dir = 2;
      // Fallthrough
    case 11:
      if (++npc->act_wait > 50) npc->act = 20;

      ++npc->anim_wait;

      if (npc->act_wait > 1) {
        npc->anim_wait = 0;

        if (++npc->anim > 1) npc->anim = 0;
      }

      if (player.x > npc->x + (320 * 0x200) || player.x < npc->x - (320 * 0x200) || player.y > npc->y + (240 * 0x200) ||
          player.y < npc->y - (240 * 0x200))
        npc->act = 0;

      break;

    case 20:
      npc->act = 21;
      npc->act_wait = 0;

      deg = (u8)m_atan2(npc->x - player.x, npc->y - player.y);
      deg += (u8)m_rand(-3, 3);
      npc->yvel2 = m_sin(deg) * 2;
      npc->xvel2 = m_cos(deg) * 2;

      if (npc->xvel2 < 0)
        npc->dir = 0;
      else
        npc->dir = 2;
      // Fallthrough
    case 21:
      if (npc->xvel2 < 0 && npc->flags & 1) {
        npc->dir = 2;
        npc->xvel2 *= -1;
      }

      if (npc->xvel2 > 0 && npc->flags & 4) {
        npc->dir = 0;
        npc->xvel2 *= -1;
      }

      if (npc->yvel2 < 0 && npc->flags & 2) npc->yvel2 *= -1;
      if (npc->yvel2 > 0 && npc->flags & 8) npc->yvel2 *= -1;

      if (npc->flags & 0x100) npc->yvel2 = -0x200;

      npc->x += npc->xvel2;
      npc->y += npc->yvel2;

      if (++npc->act_wait > 50) npc->act = 10;

      if (++npc->anim > 1) npc->anim = 0;

      break;
  }

  if (npc->dir == 0)
    npc->rect = &rc_left[npc->anim];
  else
    npc->rect = &rc_right[npc->anim];
}

// Bute
void npc_act_309(npc_t *npc) {
  static const rect_t rc_left[2] = {
    {0, 0, 16, 16},
    {16, 0, 32, 16},
  };

  static const rect_t rc_right[2] = {
    {0, 16, 16, 32},
    {16, 16, 32, 32},
  };

  switch (npc->act) {
    case 0:
      npc->act = 1;
      // Fallthrough
    case 1:
      if (npc->dir == 0) {
        if (player.x > npc->x - (288 * 0x200) && player.x < npc->x - (272 * 0x200)) {
          npc->act = 10;
          break;
        }
      } else {
        if (player.x < npc->x + (288 * 0x200) && player.x > npc->x + (272 * 0x200)) {
          npc->act = 10;
          break;
        }
      }

      return;

    case 10:
      npc->act = 11;
      npc->bits |= NPC_SHOOTABLE;
      npc->damage = 5;
      // Fallthrough
    case 11:
      if (npc->x > player.x)
        npc->dir = 0;
      else
        npc->dir = 2;

      if (npc->dir == 0)
        npc->xvel2 -= 0x10;
      else
        npc->xvel2 += 0x10;

      if (npc->y > player.y)
        npc->yvel2 -= 0x10;
      else
        npc->yvel2 += 0x10;

      if (npc->xvel2 < 0 && npc->flags & 1) npc->xvel2 *= -1;
      if (npc->xvel2 > 0 && npc->flags & 4) npc->xvel2 *= -1;

      if (npc->yvel2 < 0 && npc->flags & 2) npc->yvel2 *= -1;
      if (npc->yvel2 > 0 && npc->flags & 8) npc->yvel2 *= -1;

      if (npc->xvel2 < -0x5FF) npc->xvel2 = -0x5FF;
      if (npc->xvel2 > 0x5FF) npc->xvel2 = 0x5FF;

      if (npc->yvel2 < -0x5FF) npc->yvel2 = -0x5FF;
      if (npc->yvel2 > 0x5FF) npc->yvel2 = 0x5FF;

      npc->x += npc->xvel2;
      npc->y += npc->yvel2;

      if (++npc->anim_wait > 1) {
        npc->anim_wait = 0;
        ++npc->anim;
      }

      if (npc->anim > 1) npc->anim = 0;

      break;
  }

  if (npc->dir == 0)
    npc->rect = &rc_left[npc->anim];
  else
    npc->rect = &rc_right[npc->anim];

  if (npc->life <= 996) {
    npc->class_num = 316;
    npc->act = 0;
  }
}

// Bute (with sword)
void npc_act_310(npc_t *npc) {
  static const rect_t rc_left[5] = {
    {32, 0, 56, 16}, {56, 0, 80, 16}, {80, 0, 104, 16},
    {104, 0, 128, 16}, {128, 0, 152, 16},
  };

  static const rect_t rc_right[5] = {
    {32, 16, 56, 32}, {56, 16, 80, 32}, {80, 16, 104, 32},
    {104, 16, 128, 32}, {128, 16, 152, 32},
  };

  switch (npc->act) {
    case 0:
      npc->act = 1;
      npc->bits &= ~NPC_SHOOTABLE;
      npc->bits |= NPC_INVULNERABLE;
      npc->damage = 0;
      // Fallthrough
    case 1:
      if (npc->x > player.x)
        npc->dir = 0;
      else
        npc->dir = 2;

      npc->anim = 0;

      if (player.x > npc->x - (128 * 0x200) && player.x < npc->x + (128 * 0x200) && player.y > npc->y - (128 * 0x200) &&
          player.y < npc->y + (16 * 0x200))
        npc->act = 10;

      break;

    case 10:
      npc->xvel = 0;
      npc->act = 11;
      npc->act_wait = 0;
      npc->bits &= ~NPC_SHOOTABLE;
      npc->bits |= NPC_INVULNERABLE;
      npc->damage = 0;
      npc->anim = 0;
      // Fallthrough
    case 11:
      if (++npc->act_wait > 30) npc->act = 20;

      break;

    case 20:
      npc->act = 21;
      npc->act_wait = 0;
      npc->bits &= ~NPC_INVULNERABLE;
      npc->bits |= NPC_SHOOTABLE;
      npc->damage = 0;

      if (npc->x > player.x)
        npc->dir = 0;
      else
        npc->dir = 2;
      // Fallthrough
    case 21:
      if (npc->dir == 0)
        npc->xvel = -0x400;
      else
        npc->xvel = 0x400;

      if (++npc->anim_wait > 3) {
        npc->anim_wait = 0;
        ++npc->anim;
      }

      if (npc->anim > 1) npc->anim = 0;

      if (++npc->act_wait > 50) npc->act = 10;

      if (npc->x < player.x + (40 * 0x200) && npc->x > player.x - (40 * 0x200)) {
        npc->yvel = -0x300;
        npc->xvel /= 2;
        npc->anim = 2;
        npc->act = 30;
        snd_play_sound(PRIO_NORMAL, 30, FALSE);
      }

      break;

    case 30:
      if (npc->yvel > -0x80) {
        npc->act = 31;
        npc->anim_wait = 0;
        npc->anim = 3;
        npc->damage = 9;
      }

      break;

    case 31:
      if (++npc->anim_wait > 2) {
        npc->anim_wait = 0;
        npc->anim = 4;
      }

      if (npc->flags & 8) {
        npc->act = 32;
        npc->act_wait = 0;
        npc->xvel = 0;
        npc->damage = 3;
      }

      break;

    case 32:
      if (++npc->act_wait > 30) {
        npc->act = 10;
        npc->damage = 0;
      }

      break;
  }

  npc->yvel += 0x20;

  npc->x += npc->xvel;
  npc->y += npc->yvel;

  if (npc->dir == 0)
    npc->rect = &rc_left[npc->anim];
  else
    npc->rect = &rc_right[npc->anim];

  if (npc->life <= 996) {
    npc->class_num = 316;
    npc->act = 0;
  }
}

// Bute archer
void npc_act_311(npc_t *npc) {
  static const rect_t rc_left[7] = {
    {0, 32, 24, 56},   {24, 32, 48, 56},   {48, 32, 72, 56},   {72, 32, 96, 56},
    {96, 32, 120, 56}, {120, 32, 144, 56}, {144, 32, 168, 56},
  };

  static const rect_t rc_right[7] = {
    {0, 56, 24, 80},   {24, 56, 48, 80},   {48, 56, 72, 80},   {72, 56, 96, 80},
    {96, 56, 120, 80}, {120, 56, 144, 80}, {144, 56, 168, 80},
  };

  switch (npc->act) {
    case 0:
      npc->act = 1;
      // Fallthrough
    case 1:
      if (npc->dir == 0) {
        if (player.x > npc->x - (320 * 0x200) && player.x < npc->x && player.y > npc->y - (160 * 0x200) &&
            player.y < npc->y + (160 * 0x200))
          npc->act = 10;
      } else {
        if (player.x > npc->x && player.x < npc->x + (320 * 0x200) && player.y > npc->y - (160 * 0x200) &&
            player.y < npc->y + (160 * 0x200))
          npc->act = 10;
      }

      break;

    case 10:
      npc->act = 11;
      // Fallthrough
    case 11:
      if (npc->x > player.x)
        npc->dir = 0;
      else
        npc->dir = 2;

      if (player.x > npc->x - (224 * 0x200) && player.x < npc->x + (224 * 0x200) && player.y > npc->y - (8 * 0x200)) {
        npc->anim = 1;
        npc->count1 = 0;
      } else {
        npc->anim = 4;
        npc->count1 = 1;
      }

      if (++npc->act_wait > 10) npc->act = 20;

      break;

    case 20:
      npc->act = 21;
      npc->act_wait = 0;
      // Fallthrough
    case 21:
      if (npc->count1 == 0) {
        if (++npc->anim > 2) npc->anim = 1;
      } else {
        if (++npc->anim > 5) npc->anim = 4;
      }

      if (++npc->act_wait > 30) npc->act = 30;

      break;

    case 30:
      npc->act = 31;
      npc->act_wait = 0;

      if (npc->count1 == 0) {
        if (npc->dir == 0)
          npc_spawn(312, npc->x, npc->y, -0x600, 0, 0, NULL, 0x100);
        else
          npc_spawn(312, npc->x, npc->y, 0x600, 0, 2, NULL, 0x100);

        npc->anim = 3;
      } else {
        if (npc->dir == 0)
          npc_spawn(312, npc->x, npc->y, -0x600, -0x600, 0, NULL, 0x100);
        else
          npc_spawn(312, npc->x, npc->y, 0x600, -0x600, 2, NULL, 0x100);

        npc->anim = 6;
      }
      // Fallthrough
    case 31:
      if (++npc->act_wait > 30) {
        npc->act = 40;
        npc->act_wait = m_rand(0, 100);
      }

      break;

    case 40:
      npc->anim = 0;

      if (++npc->act_wait > 150) npc->act = 10;

      if (player.x < npc->x - (352 * 0x200) || player.x > npc->x + (352 * 0x200) || player.y < npc->y - (240 * 0x200) ||
          player.y > npc->y + (240 * 0x200)) {
        npc->act = 40;
        npc->act_wait = 0;
      }

      break;
  }

  if (npc->dir == 0)
    npc->rect = &rc_left[npc->anim];
  else
    npc->rect = &rc_right[npc->anim];

  if (npc->life <= 992) {
    npc->class_num = 316;
    npc->act = 0;
  }
}

// Bute arrow projectile
void npc_act_312(npc_t *npc) {
  static const rect_t rc_left[5] = {
    {0, 160, 16, 176}, {16, 160, 32, 176}, {32, 160, 48, 176},
    {48, 160, 64, 176}, {64, 160, 80, 176},
  };

  static const rect_t rc_right[5] = {
    {0, 176, 16, 192}, {16, 176, 32, 192}, {32, 176, 48, 192},
    {48, 176, 64, 192}, {64, 176, 80, 192},
  };

  if (npc->act > 0 && npc->act < 20 && npc->flags & 0xFF) npc->act = 20;

  switch (npc->act) {
    case 0:
      npc->act = 1;
      npc->act_wait = 0;

      if (npc->xvel < 0)
        npc->dir = 0;
      else
        npc->dir = 2;

      if (npc->yvel < 0)
        npc->anim = 0;
      else
        npc->anim = 2;
      // Fallthrough
    case 1:
      ++npc->act_wait;

      if (npc->act_wait == 4) npc->bits &= ~NPC_IGNORE_SOLIDITY;

      if (npc->act_wait > 10) npc->act = 10;

      break;

    case 10:
      npc->act = 11;
      npc->anim_wait = 0;
      npc->xvel = 3 * npc->xvel / 4;
      npc->yvel = 3 * npc->yvel / 4;
      // Fallthrough
    case 11:
      npc->yvel += 0x20;

      if (++npc->anim_wait > 10) {
        npc->anim_wait = 0;
        ++npc->anim;
      }

      if (npc->anim > 4) npc->anim = 4;

      break;

    case 20:
      npc->act = 21;
      npc->act_wait = 0;
      npc->xvel = 0;
      npc->yvel = 0;
      npc->damage = 0;
      // Fallthrough
    case 21:
      if (++npc->act_wait > 30) npc->act = 30;

      break;

    case 30:
      npc->act = 31;
      npc->act_wait = 0;
      // Fallthrough
    case 31:
      if (++npc->act_wait > 30) {
        npc->cond = 0;
        return;
      }

      break;
  }

  if (npc->yvel > 0x5FF) npc->yvel = 0x5FF;

  npc->x += npc->xvel;
  npc->y += npc->yvel;

  if (npc->dir == 0)
    npc->rect = &rc_left[npc->anim];
  else
    npc->rect = &rc_right[npc->anim];

  if (npc->act == 31) {
    if (npc->act_wait / 2 % 2) npc->rect = NULL;
  }
}

// Ma Pignon
void npc_act_313(npc_t *npc) {
  static const rect_t rc_left[14] = {
    {128, 0, 144, 16}, {144, 0, 160, 16}, {160, 0, 176, 16}, {176, 0, 192, 16}, {192, 0, 208, 16},
    {208, 0, 224, 16}, {224, 0, 240, 16}, {240, 0, 256, 16}, {256, 0, 272, 16}, {272, 0, 288, 16},
    {288, 0, 304, 16}, {128, 0, 144, 16}, {176, 0, 192, 16}, {304, 0, 320, 16},
  };

  static const rect_t rc_right[14] = {
    {128, 16, 144, 32}, {144, 16, 160, 32}, {160, 16, 176, 32}, {176, 16, 192, 32}, {192, 16, 208, 32},
    {208, 16, 224, 32}, {224, 16, 240, 32}, {240, 16, 256, 32}, {256, 16, 272, 32}, {272, 16, 288, 32},
    {288, 16, 304, 32}, {128, 16, 144, 32}, {176, 16, 192, 32}, {304, 16, 320, 32},
  };

  switch (npc->act) {
    case 0:
      npc->act = 1;
      npc->anim = 0;
      npc->anim_wait = 0;
      npc->y += 4 * 0x200;
      // Fallthrough
    case 1:
      npc->yvel += 0x40;

      if (m_rand(0, 120) == 10) {
        npc->act = 2;
        npc->act_wait = 0;
        npc->anim = 1;
      }

      if (npc->x - (32 * 0x200) < player.x && npc->x + (32 * 0x200) > player.x) {
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

    case 100:
      npc->act = 110;
      npc->act_wait = 0;
      npc->count1 = 0;
      npc->bits |= NPC_SHOOTABLE;
      // Fallthrough
    case 110:
      npc->damage = 1;

      if (npc->x > player.x)
        npc->dir = 0;
      else
        npc->dir = 2;

      npc->anim = 0;

      if (++npc->act_wait > 4) {
        npc->act_wait = 0;
        npc->act = 120;

        if (++npc->count2 > 12) {
          npc->count2 = 0;
          npc->act = 300;
        }
      }

      break;

    case 120:
      npc->anim = 2;

      if (++npc->act_wait > 4) {
        npc->act = 130;
        npc->anim = 3;
        npc->xvel = 2 * m_rand(-0x200, 0x200);
        npc->yvel = -0x800;
        snd_play_sound(PRIO_NORMAL, 30, FALSE);
        ++npc->count1;
      }

      break;

    case 130:
      npc->yvel += 0x80;

      if (npc->y > 128 * 0x200) npc->bits &= ~NPC_IGNORE_SOLIDITY;

      if (npc->xvel < 0 && npc->flags & 1) npc->xvel *= -1;
      if (npc->xvel > 0 && npc->flags & 4) npc->xvel *= -1;

      if (npc->x > player.x)
        npc->dir = 0;
      else
        npc->dir = 2;

      if (npc->yvel < -0x200)
        npc->anim = 3;
      else if (npc->yvel > 0x200)
        npc->anim = 4;
      else
        npc->anim = 0;

      if (npc->flags & 8) {
        npc->act = 140;
        npc->act_wait = 0;
        npc->anim = 2;
        npc->xvel = 0;
      }

      if (npc->count1 > 4 && player.y < npc->y + (4 * 0x200)) {
        npc->act = 200;
        npc->act_wait = 0;
        npc->xvel = 0;
        npc->yvel = 0;
      }

      break;

    case 140:
      npc->anim = 2;

      if (++npc->act_wait > 4) npc->act = 110;

      break;

    case 200:
      npc->anim = 5;

      if (++npc->act_wait > 10) {
        npc->act = 210;
        npc->anim = 6;

        if (npc->dir == 0)
          npc->xvel = -0x5FF;
        else
          npc->xvel = 0x5FF;

        snd_play_sound(PRIO_NORMAL, 25, FALSE);
        npc->bits &= ~NPC_SHOOTABLE;
        npc->bits |= NPC_INVULNERABLE;
        npc->damage = 10;
      }

      break;

    case 210:
      if (++npc->anim > 7) npc->anim = 6;

      if (npc->xvel < 0 && npc->flags & 1) npc->act = 220;
      if (npc->xvel > 0 && npc->flags & 4) npc->act = 220;

      break;

    case 220:
      npc->act = 221;
      npc->act_wait = 0;
      cam_start_quake_small(16);
      snd_play_sound(PRIO_NORMAL, 26, FALSE);
      npc->damage = 4;
      // Fallthrough
    case 221:
      if (++npc->anim > 7) npc->anim = 6;

      if (++npc->act_wait % 6 == 0)
        npc_spawn(314, m_rand(4, 16) * 0x200 * 0x10, 1 * 0x200 * 0x10, 0, 0, 0, NULL, 0x100);

      if (npc->act_wait > 30) {
        npc->count1 = 0;
        npc->act = 130;
        npc->bits |= NPC_SHOOTABLE;
        npc->bits &= ~NPC_INVULNERABLE;
        npc->damage = 3;
      }

      break;

    case 300:
      npc->act = 301;
      npc->anim = 9;

      if (npc->x > player.x)
        npc->dir = 0;
      else
        npc->dir = 2;
      // Fallthrough
    case 301:
      if (++npc->anim > 11) npc->anim = 9;

      if (npc->dir == 0)
        npc->xvel = -0x400;
      else
        npc->xvel = 0x400;

      if (player.x > npc->x - (4 * 0x200) && player.x < npc->x + (4 * 0x200)) {
        npc->act = 310;
        npc->act_wait = 0;
        npc->anim = 2;
        npc->xvel = 0;
      }

      break;

    case 310:
      npc->anim = 2;

      if (++npc->act_wait > 4) {
        npc->act = 320;
        npc->anim = 12;
        npc->yvel = -0x800;
        snd_play_sound(PRIO_NORMAL, 25, FALSE);
        npc->bits |= NPC_IGNORE_SOLIDITY;
        npc->bits &= ~NPC_SHOOTABLE;
        npc->bits |= NPC_INVULNERABLE;
        npc->damage = 10;
      }

      break;

    case 320:
      if (++npc->anim > 13) npc->anim = 12;

      if (npc->y < (16 * 0x200)) npc->act = 330;

      break;

    case 330:
      npc->yvel = 0;
      npc->act = 331;
      npc->act_wait = 0;
      cam_start_quake_small(16);
      snd_play_sound(PRIO_NORMAL, 26, FALSE);
      // Fallthrough
    case 331:
      if (++npc->anim > 13) npc->anim = 12;

      if (++npc->act_wait % 6 == 0) npc_spawn(315, m_rand(4, 16) * 0x200 * 0x10, 0, 0, 0, 0, NULL, 0x100);

      if (npc->act_wait > 30) {
        npc->count1 = 0;
        npc->act = 130;
        npc->bits |= NPC_SHOOTABLE;
        npc->bits &= ~NPC_INVULNERABLE;
        npc->damage = 3;
      }

      break;

    case 500:
      npc->bits &= ~NPC_SHOOTABLE;
      npc->act = 501;
      npc->act_wait = 0;
      npc->anim = 8;
      npc->tgt_x = npc->x;
      npc->damage = 0;
      npc_delete_by_class(315, TRUE);
      // Fallthrough
    case 501:
      npc->yvel += 0x20;

      if (++npc->act_wait % 2)
        npc->x = npc->tgt_x;
      else
        npc->x = npc->tgt_x + (1 * 0x200);

      break;
  }

  if (npc->act > 100 && npc->act < 500 && npc->act != 210 && npc->act != 320) {
    if (bullet_any_exist()) {
      npc->bits &= ~NPC_SHOOTABLE;
      npc->bits |= NPC_INVULNERABLE;
    } else {
      npc->bits |= NPC_SHOOTABLE;
      npc->bits &= ~NPC_INVULNERABLE;
    }
  }

  if (npc->yvel > 0x5FF) npc->yvel = 0x5FF;

  npc->x += npc->xvel;
  npc->y += npc->yvel;

  if (npc->dir == 0)
    npc->rect = &rc_left[npc->anim];
  else
    npc->rect = &rc_right[npc->anim];
}

// Ma Pignon rock
void npc_act_314(npc_t *npc) {
  static const rect_t rc[3] = {
    {64, 64, 80, 80},
    {80, 64, 96, 80},
    {96, 64, 112, 80},
  };

  switch (npc->act) {
    case 0:
      npc->count2 = 0;
      npc->act = 100;
      npc->bits |= NPC_INVULNERABLE;
      npc->anim = m_rand(0, 2);
      // Fallthrough
    case 100:
      npc->yvel += 0x40;

      if (npc->yvel > 0x700) npc->yvel = 0x700;

      if (npc->y > 128 * 0x200) npc->bits &= ~NPC_IGNORE_SOLIDITY;

      if (npc->flags & 8) {
        int i;

        npc->yvel = -0x200;
        npc->act = 110;
        npc->bits |= NPC_IGNORE_SOLIDITY;
        snd_play_sound(PRIO_NORMAL, 12, FALSE);
        cam_start_quake_small(10);

        for (i = 0; i < 2; ++i)
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

  if (++npc->anim_wait > 6) {
    ++npc->anim_wait;
    ++npc->anim;
  }

  if (npc->anim > 2) npc->anim = 0;

  if (player.y > npc->y)
    npc->damage = 10;
  else
    npc->damage = 0;

  npc->y += npc->yvel;

  npc->rect = &rc[npc->anim];
}

// Ma Pignon clone
void npc_act_315(npc_t *npc) {
  static const rect_t rc_left[4] = {
    {128, 0, 144, 16},
    {160, 0, 176, 16},
    {176, 0, 192, 16},
    {192, 0, 208, 16},
  };

  static const rect_t rc_right[4] = {
    {128, 16, 144, 32},
    {160, 16, 176, 32},
    {176, 16, 192, 32},
    {192, 16, 208, 32},
  };

  switch (npc->act) {
    case 0:
      npc->anim = 3;
      npc->yvel += 0x80;

      if (npc->y > 128 * 0x200) {
        npc->act = 130;
        npc->bits &= ~NPC_IGNORE_SOLIDITY;
      }

      break;

    case 100:
      npc->act = 110;
      npc->act_wait = 0;
      npc->count1 = 0;
      npc->bits |= NPC_SHOOTABLE;
      // Fallthrough
    case 110:
      if (npc->x > player.x)
        npc->dir = 0;
      else
        npc->dir = 2;

      npc->anim = 0;

      if (++npc->act_wait > 4) {
        npc->act_wait = 0;
        npc->act = 120;
      }

      break;

    case 120:
      npc->anim = 1;

      if (++npc->act_wait > 4) {
        npc->act = 130;
        npc->anim = 3;
        npc->xvel = 2 * m_rand(-0x200, 0x200);
        npc->yvel = -0x800;
        snd_play_sound(PRIO_NORMAL, 30, FALSE);
      }

      break;

    case 130:
      npc->yvel += 0x80;

      if (npc->xvel < 0 && npc->flags & 1) npc->xvel *= -1;
      if (npc->xvel > 0 && npc->flags & 4) npc->xvel *= -1;

      if (npc->x > player.x)
        npc->dir = 0;
      else
        npc->dir = 2;

      if (npc->yvel < -0x200)
        npc->anim = 2;
      else if (npc->yvel > 0x200)
        npc->anim = 0;
      else
        npc->anim = 3;

      if (npc->flags & 8) {
        npc->act = 140;
        npc->act_wait = 0;
        npc->anim = 1;
        npc->xvel = 0;
      }

      break;

    case 140:
      npc->anim = 1;

      if (++npc->act_wait > 4) {
        npc->act = 110;
        npc->bits |= NPC_SHOOTABLE;
      }

      break;
  }

  if (npc->act > 100) {
    if (bullet_any_exist()) {
      npc->bits &= ~NPC_SHOOTABLE;
      npc->bits |= NPC_INVULNERABLE;
    } else {
      npc->bits |= NPC_SHOOTABLE;
      npc->bits &= ~NPC_INVULNERABLE;
    }
  }

  if (++npc->count2 > 300) {
    npc_show_death_damage(npc);
  } else {
    if (npc->yvel > 0x5FF) npc->yvel = 0x5FF;

    npc->x += npc->xvel;
    npc->y += npc->yvel;

    if (npc->dir == 0)
      npc->rect = &rc_left[npc->anim];
    else
      npc->rect = &rc_right[npc->anim];
  }
}

// Bute (dead)
void npc_act_316(npc_t *npc) {
  static const rect_t rc_left[3] = {
    {248, 32, 272, 56},
    {272, 32, 296, 56},
    {296, 32, 320, 56},
  };

  static const rect_t rc_right[3] = {
    {248, 56, 272, 80},
    {272, 56, 296, 80},
    {296, 56, 320, 80},
  };

  switch (npc->act) {
    case 0:
      npc->bits &= ~NPC_SHOOTABLE;
      npc->bits &= ~NPC_IGNORE_SOLIDITY;
      npc->damage = 0;
      npc->act = 1;
      npc->anim = 0;
      npc->view.front = 12 * 0x200;
      npc->view.back = 12 * 0x200;
      npc->view.top = 12 * 0x200;
      npc->yvel = -0x200;

      if (npc->dir == 0)
        npc->xvel = 0x100;
      else
        npc->xvel = -0x100;

      snd_play_sound(PRIO_NORMAL, 50, FALSE);

      break;

    case 1:
      if (npc->flags & 8) {
        npc->anim = 1;
        npc->anim_wait = 0;
        npc->act = 2;
        npc->act_wait = 0;
      }

      break;

    case 2:
      npc->xvel = 8 * npc->xvel / 9;

      if (++npc->anim_wait > 3) {
        npc->anim_wait = 0;
        ++npc->anim;
      }

      if (npc->anim > 2) npc->anim = 1;

      if (++npc->act_wait > 50) npc->cond |= 8;

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

// Mesa
void npc_act_317(npc_t *npc) {
  static const rect_t rc_left[4] = {
    {0, 80, 32, 120},
    {32, 80, 64, 120},
    {64, 80, 96, 120},
    {96, 80, 128, 120},
  };

  static const rect_t rc_right[4] = {
    {0, 120, 32, 160},
    {32, 120, 64, 160},
    {64, 120, 96, 160},
    {96, 120, 128, 160},
  };

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
      npc->count1 = 0;
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

      if (player.x > npc->x - (320 * 0x200) && player.x < npc->x + (320 * 0x200) && player.y > npc->y - (160 * 0x200) &&
          player.y < npc->y + (160 * 0x200) && ++npc->count1 > 50)
        npc->act = 10;

      break;

    case 10:
      npc->act = 11;
      npc->act_wait = 0;
      npc->anim = 2;
      npc_spawn(319, npc->x, npc->y, 0, 0, 0, npc, 0x100);
      // Fallthrough
    case 11:
      if (++npc->act_wait > 50) {
        npc->act_wait = 0;
        npc->act = 12;
        npc->anim = 3;
        snd_play_sound(PRIO_NORMAL, 39, FALSE);
      }

      break;

    case 12:
      if (++npc->act_wait > 20) npc->act = 1;

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

  if (npc->life <= 936) {
    npc->class_num = 318;
    npc->act = 0;
  }
}

// Mesa (dead)
void npc_act_318(npc_t *npc) {
  static const rect_t rc_left[3] = {
    {224, 80, 256, 120},
    {256, 80, 288, 120},
    {288, 80, 320, 120},
  };

  static const rect_t rc_right[3] = {
    {224, 120, 256, 160},
    {256, 120, 288, 160},
    {288, 120, 320, 160},
  };

  switch (npc->act) {
    case 0:
      npc->bits &= ~NPC_SHOOTABLE;
      npc->bits &= ~NPC_IGNORE_SOLIDITY;
      npc->bits &= ~NPC_SOLID_SOFT;
      npc->damage = 0;
      npc->act = 1;
      npc->anim = 0;
      npc->yvel = -0x200;

      if (npc->dir == 0)
        npc->xvel = 0x40;
      else
        npc->xvel = -0x40;

      snd_play_sound(PRIO_NORMAL, 54, FALSE);
      break;

    case 1:
      if (npc->flags & 8) {
        npc->anim = 1;
        npc->anim_wait = 0;
        npc->act = 2;
        npc->act_wait = 0;
      }

      break;

    case 2:
      npc->xvel = 8 * npc->xvel / 9;

      if (++npc->anim_wait > 3) {
        npc->anim_wait = 0;
        ++npc->anim;
      }

      if (npc->anim > 2) npc->anim = 1;

      if (++npc->act_wait > 50) npc->cond |= 8;

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

// Mesa block
void npc_act_319(npc_t *npc) {
  static const rect_t rc[3] = {
    {16, 0, 32, 16},
    {16, 0, 32, 16},
    {96, 80, 112, 96},
  };

  switch (npc->act) {
    case 0:
      npc->y = npc->parent->y + (10 * 0x200);

      if (npc->parent->dir == 0)
        npc->x = npc->parent->x + (7 * 0x200);
      else
        npc->x = npc->parent->x - (7 * 0x200);

      if (npc->parent->class_num == 318) {
        npc_spawn_death_fx(npc->x, npc->y, 0, 3, 0);
        npc->cond = 0;
        return;
      }

      if (npc->parent->anim != 2) {
        npc->act = 2;
        npc->act_wait = 0;
        npc->yvel = -0x400;
        npc->y = npc->parent->y - (4 * 0x200);

        if (npc->parent->dir == 0)
          npc->xvel = -0x400;
        else
          npc->xvel = 0x400;
      }

      break;

    case 2:
      if (++npc->act_wait == 4) npc->bits &= ~NPC_IGNORE_SOLIDITY;

      npc->yvel += 0x2A;
      if (npc->yvel > 0x5FF) npc->yvel = 0x5FF;

      npc->x += npc->xvel;
      npc->y += npc->yvel;

      if (npc->flags & 8) {
        snd_play_sound(PRIO_NORMAL, 12, FALSE);
        npc_spawn_death_fx(npc->x, npc->y, 0, 3, 0);
        npc->cond = 0;
      }

      break;
  }

  if (++npc->anim > 2) npc->anim = 0;

  npc->rect = &rc[npc->anim];
}
