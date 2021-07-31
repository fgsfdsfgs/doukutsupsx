#include "game/npc_act/npc_act.h"

// Computer
void npc_act_020(npc_t *npc) {
  static const rect_t rc_left = {288, 16, 320, 40};

  static const rect_t rc_right[3] = {
    {288, 40, 320, 64},
    {288, 40, 320, 64},
    {288, 64, 320, 88},
  };

  if (++npc->anim_wait > 3) {
    npc->anim_wait = 0;
    ++npc->anim;
  }

  if (npc->anim > 2) npc->anim = 0;

  if (npc->dir == 0)
    npc->rect = &rc_left;
  else
    npc->rect = &rc_right[npc->anim];
}

// Chest (open)
void npc_act_021(npc_t *npc) {
  if (npc->act == 0) {
    npc->act = 1;

    if (npc->dir == 2) npc->y += 16 * 0x200;
  }

  static const rect_t rect = {224, 40, 240, 48};

  npc->rect = &rect;
}

// Teleporter
void npc_act_022(npc_t *npc) {
  static const rect_t rect[2] = {
    {240, 16, 264, 48},
    {248, 152, 272, 184},
  };

  switch (npc->act) {
    case 0:
      npc->anim = 0;
      break;

    case 1:
      if (++npc->anim > 1) npc->anim = 0;

      break;
  }

  npc->rect = &rect[npc->anim];
}

// Teleporter lights
void npc_act_023(npc_t *npc) {
  static const rect_t rect[8] = {
    {264, 16, 288, 20}, {264, 20, 288, 24}, {264, 24, 288, 28},
    {264, 28, 288, 32}, {264, 32, 288, 36}, {264, 36, 288, 40},
    {264, 40, 288, 44}, {264, 44, 288, 48},
  };

  if (++npc->anim_wait > 1) {
    npc->anim_wait = 0;
    ++npc->anim;
  }

  if (npc->anim > 7) npc->anim = 0;

  npc->rect = &rect[npc->anim];
}

// Power Critter
void npc_act_024(npc_t *npc) {
  static const rect_t rc_left[6] = {
    {0, 0, 24, 24},  {24, 0, 48, 24},  {48, 0, 72, 24},
    {72, 0, 96, 24}, {96, 0, 120, 24}, {120, 0, 144, 24},
  };

  static const rect_t rc_right[6] = {
    {0, 24, 24, 48},  {24, 24, 48, 48},  {48, 24, 72, 48},
    {72, 24, 96, 48}, {96, 24, 120, 48}, {120, 24, 144, 48},
  };

  switch (npc->act) {
    case 0:
      npc->y += 3 * 0x200;
      npc->act = 1;
      // Fallthrough
    case 1:
      if (npc->act_wait >= 8 && npc->x - (128 * 0x200) < player.x &&
          npc->x + (128 * 0x200) > player.x && npc->y - (128 * 0x200) < player.y &&
          npc->y + (48 * 0x200) > player.y) {
        if (npc->x > player.x)
          npc->dir = 0;
        else
          npc->dir = 2;

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

      if (npc->act_wait >= 8 && npc->x - (96 * 0x200) < player.x &&
          npc->x + (96 * 0x200) > player.x && npc->y - (96 * 0x200) < player.y &&
          npc->y + (48 * 0x200) > player.y) {
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
        snd_play_sound(-1, 108, SOUND_MODE_PLAY);

        if (npc->x > player.x)
          npc->dir = 0;
        else
          npc->dir = 2;

        if (npc->dir == 0)
          npc->xvel = -0x100;
        else
          npc->xvel = 0x100;
      }

      break;

    case 3:
      if (npc->yvel > 0x200) {
        npc->tgt_y = npc->y;
        npc->act = 4;
        npc->anim = 3;
        npc->act_wait = 0;
        npc->act_wait = 0;  // Pixel duplicated this line
      }

      break;

    case 4:
      if (npc->x < player.x)
        npc->dir = 2;
      else
        npc->dir = 0;

      ++npc->act_wait;

      if (npc->flags & 7 || npc->act_wait > 100) {
        npc->damage = 12;
        npc->act = 5;
        npc->anim = 2;
        npc->xvel /= 2;
        break;
      }

      if (npc->act_wait % 4 == 1) snd_play_sound(-1, 110, SOUND_MODE_PLAY);

      if (++npc->anim_wait > 0) {
        npc->anim_wait = 0;
        ++npc->anim;
      }

      if (npc->anim > 5) npc->anim = 3;

      break;

    case 5:
      if (npc->flags & 8) {
        npc->damage = 2;
        npc->xvel = 0;
        npc->act_wait = 0;
        npc->anim = 0;
        npc->act = 1;
        snd_play_sound(-1, 26, SOUND_MODE_PLAY);
        // SetQuake(30);
      }

      break;
  }

  if (npc->act != 4) {
    npc->yvel += 0x20;
    if (npc->yvel > 0x5FF) npc->yvel = 0x5FF;
  } else {
    if (npc->x < player.x)
      npc->xvel += 0x20;
    else
      npc->xvel -= 0x20;

    if (npc->y > npc->tgt_y)
      npc->yvel -= 0x10;
    else
      npc->yvel += 0x10;

    if (npc->yvel > 0x200) npc->yvel = 0x200;
    if (npc->yvel < -0x200) npc->yvel = -0x200;

    if (npc->xvel > 0x200) npc->xvel = 0x200;
    if (npc->xvel < -0x200) npc->xvel = -0x200;
  }

  npc->x += npc->xvel;
  npc->y += npc->yvel;

  if (npc->dir == 0)
    npc->rect = &rc_left[npc->anim];
  else
    npc->rect = &rc_right[npc->anim];
}

// Egg Corridor lift
void npc_act_025(npc_t *npc) {
  static const rect_t rc_left[2] = {
    {256, 64, 288, 80},
    {256, 80, 288, 96},
  };

  switch (npc->act) {
    case 0:
      npc->act = 1;
      npc->anim = 0;
      npc->anim_wait = 0;
      npc->x += 8 * 0x200;
      // Fallthrough
    case 1:
      if (++npc->act_wait > 150) {
        npc->act_wait = 0;
        ++npc->act;
      }

      break;

    case 2:  // Identical to case 4
      if (++npc->act_wait <= 0x40) {
        npc->y -= 1 * 0x200;
      } else {
        npc->act_wait = 0;
        ++npc->act;
      }

      break;

    case 3:
      if (++npc->act_wait > 150) {
        npc->act_wait = 0;
        ++npc->act;
      }

      break;

    case 4:  // Identical to case 2
      if (++npc->act_wait <= 0x40) {
        npc->y -= 1 * 0x200;
      } else {
        npc->act_wait = 0;
        ++npc->act;
      }

      break;

    case 5:
      if (++npc->act_wait > 150) {
        npc->act_wait = 0;
        ++npc->act;
      }

      break;

    case 6:
      if (++npc->act_wait <= 0x40) {
        npc->y += 1 * 0x200;
      } else {
        npc->act_wait = 0;
        ++npc->act;
      }

      break;

    case 7:
      if (++npc->act_wait > 150) {
        npc->act_wait = 0;
        ++npc->act;
      }

      break;

    case 8:
      if (++npc->act_wait <= 0x40) {
        npc->y += 1 * 0x200;
      } else {
        npc->act_wait = 0;
        npc->act = 1;
      }

      break;
  }

  switch (npc->act) {
    case 2:
    case 4:
    case 6:
    case 8:
      if (++npc->anim_wait > 1) {
        npc->anim_wait = 0;
        ++npc->anim;
      }

      if (npc->anim > 1) npc->anim = 0;

      break;
  }

  npc->rect = &rc_left[npc->anim];
}

// Bat (Grasstown, flying)
void npc_act_026(npc_t *npc) {
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

      if (npc->count1 < 120) {
        ++npc->count1;
        break;
      }

      if (npc->x - (8 * 0x200) < player.x && npc->x + (8 * 0x200) > player.x &&
          npc->y < player.y && npc->y + (96 * 0x200) > player.y) {
        npc->xvel /= 2;
        npc->yvel = 0;
        npc->act = 3;
        npc->bits &= ~NPC_IGNORE_SOLIDITY;
      }

      break;

    case 3:
      npc->yvel += 0x40;
      if (npc->yvel > 0x5FF) npc->yvel = 0x5FF;

      if (npc->flags & 8) {
        npc->yvel = 0;
        npc->xvel *= 2;
        npc->count1 = 0;
        npc->act = 1;
        npc->bits |= NPC_IGNORE_SOLIDITY;
      }

      break;
  }

  npc->x += npc->xvel;
  npc->y += npc->yvel;

  static const rect_t rect_left[4] = {
    {32, 80, 48, 96},
    {48, 80, 64, 96},
    {64, 80, 80, 96},
    {80, 80, 96, 96},
  };

  static const rect_t rect_right[4] = {
    {32, 96, 48, 112},
    {48, 96, 64, 112},
    {64, 96, 80, 112},
    {80, 96, 96, 112},
  };

  if (npc->act == 3) {
    npc->anim = 3;
  } else {
    if (++npc->anim_wait > 1) {
      npc->anim_wait = 0;
      ++npc->anim;
    }

    if (npc->anim > 2) npc->anim = 0;
  }

  if (npc->dir == 0)
    npc->rect = &rect_left[npc->anim];
  else
    npc->rect = &rect_right[npc->anim];
}

// Death trap
void npc_act_027(npc_t *npc) {
  static const rect_t rc_left[1] = {{96, 64, 128, 88}};

  npc->rect = &rc_left[npc->anim];
}

// Flying Critter (Grasstown)
void npc_act_028(npc_t *npc) {
  static const rect_t rc_left[6] = {
    {0, 48, 16, 64},  {16, 48, 32, 64}, {32, 48, 48, 64},
    {48, 48, 64, 64}, {64, 48, 80, 64}, {80, 48, 96, 64},
  };

  static const rect_t rc_right[6] = {
    {0, 64, 16, 80},  {16, 64, 32, 80}, {32, 64, 48, 80},
    {48, 64, 64, 80}, {64, 64, 80, 80}, {80, 64, 96, 80},
  };

  switch (npc->act) {
    case 0:
      npc->y += 3 * 0x200;
      npc->act = 1;
      // Fallthrough
    case 1:
      if (npc->act_wait >= 8 && npc->x - (128 * 0x200) < player.x &&
          npc->x + (128 * 0x200) > player.x && npc->y - (128 * 0x200) < player.y &&
          npc->y + (48 * 0x200) > player.y) {
        if (npc->x > player.x)
          npc->dir = 0;
        else
          npc->dir = 2;

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

      if (npc->act_wait >= 8 && npc->x - (96 * 0x200) < player.x &&
          npc->x + (96 * 0x200) > player.x && npc->y - (96 * 0x200) < player.y &&
          npc->y + (48 * 0x200) > player.y) {
        npc->act = 2;
        npc->anim = 0;
        npc->act_wait = 0;
      }

      break;

    case 2:
      if (++npc->act_wait > 8) {
        npc->act = 3;
        npc->anim = 2;
        npc->yvel = -0x4CC;
        snd_play_sound(-1, 30, SOUND_MODE_PLAY);

        if (npc->x > player.x)
          npc->dir = 0;
        else
          npc->dir = 2;

        if (npc->dir == 0)
          npc->xvel = -0x100;
        else
          npc->xvel = 0x100;
      }

      break;

    case 3:
      if (npc->yvel > 0x100) {
        npc->tgt_y = npc->y;
        npc->act = 4;
        npc->anim = 3;
        npc->act_wait = 0;
        npc->act_wait = 0;  // Pixel duplicated this line
      }

      break;

    case 4:
      if (npc->x < player.x)
        npc->dir = 2;
      else
        npc->dir = 0;

      ++npc->act_wait;

      if (npc->flags & 7 || npc->act_wait > 100) {
        npc->damage = 3;
        npc->act = 5;
        npc->anim = 2;
        npc->xvel /= 2;
        break;
      }

      if (npc->act_wait % 4 == 1) snd_play_sound(-1, 109, SOUND_MODE_PLAY);

      if (npc->flags & 8) npc->yvel = -0x200;

      if (++npc->anim_wait > 0) {
        npc->anim_wait = 0;
        ++npc->anim;
      }

      if (npc->anim > 5) npc->anim = 3;

      break;

    case 5:
      if (npc->flags & 8) {
        npc->damage = 2;
        npc->xvel = 0;
        npc->act_wait = 0;
        npc->anim = 0;
        npc->act = 1;
        snd_play_sound(-1, 23, SOUND_MODE_PLAY);
      }

      break;
  }

  if (npc->act != 4) {
    npc->yvel += 0x40;
    if (npc->yvel > 0x5FF) npc->yvel = 0x5FF;
  } else {
    if (npc->x < player.x)
      npc->xvel += 0x20;
    else
      npc->xvel -= 0x20;

    if (npc->y > npc->tgt_y)
      npc->yvel -= 0x10;
    else
      npc->yvel += 0x10;

    if (npc->yvel > 0x200) npc->yvel = 0x200;
    if (npc->yvel < -0x200) npc->yvel = -0x200;

    if (npc->xvel > 0x200) npc->xvel = 0x200;
    if (npc->xvel < -0x200) npc->xvel = -0x200;
  }

  npc->x += npc->xvel;
  npc->y += npc->yvel;

  if (npc->dir == 0)
    npc->rect = &rc_left[npc->anim];
  else
    npc->rect = &rc_right[npc->anim];
}

// Cthulhu
void npc_act_029(npc_t *npc) {
  static const rect_t rc_left[2] = {
    {0, 192, 16, 216},
    {16, 192, 32, 216},
  };

  static const rect_t rc_right[2] = {
    {0, 216, 16, 240},
    {16, 216, 32, 240},
  };

  switch (npc->act) {
    case 0:
      npc->act = 1;
      npc->anim = 0;
      npc->anim_wait = 0;
      // Fallthrough
    case 1:
      if (npc->x - (48 * 0x200) < player.x && npc->x + (48 * 0x200) > player.x &&
          npc->y - (48 * 0x200) < player.y && npc->y + (16 * 0x200) > player.y)
        npc->anim = 1;
      else
        npc->anim = 0;

      break;
  }

  if (npc->dir == 0)
    npc->rect = &rc_left[npc->anim];
  else
    npc->rect = &rc_right[npc->anim];
}

// Gunsmith
void npc_act_030(npc_t *npc) {
  static const rect_t rc[3] = {
    {48, 0, 64, 16},
    {48, 16, 64, 32},
    {0, 32, 16, 48},
  };

  if (npc->dir == 0) {
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
  } else {
    if (npc->act == 0) {
      npc->act = 1;
      npc->y += 16 * 0x200;
      npc->anim = 2;
    }

    if (++npc->act_wait > 100) {
      npc->act_wait = 0;
      // SetCaret(npc->x, npc->y - (2 * 0x200), CARET_ZZZ, DIR_LEFT);
    }
  }

  npc->rect = &rc[npc->anim];
}

// Bat (Grasstown, hanging)
void npc_act_031(npc_t *npc) {
  static const rect_t rc_left[5] = {
    {0, 80, 16, 96},  {16, 80, 32, 96}, {32, 80, 48, 96},
    {48, 80, 64, 96}, {64, 80, 80, 96},
  };

  static const rect_t rc_right[5] = {
    {0, 96, 16, 112},  {16, 96, 32, 112}, {32, 96, 48, 112},
    {48, 96, 64, 112}, {64, 96, 80, 112},
  };

  switch (npc->act) {
    case 0:
      npc->act = 1;
      // Fallthrough
    case 1:
      if (m_rand(0, 120) == 10) {
        npc->act = 2;
        npc->act_wait = 0;
        npc->anim = 1;
      }

      if (npc->x - (8 * 0x200) < player.x && npc->x + (8 * 0x200) > player.x &&
          npc->y - (8 * 0x200) < player.y && npc->y + (96 * 0x200) > player.y) {
        npc->anim = 0;
        npc->act = 3;
      }

      break;

    case 2:
      if (++npc->act_wait > 8) {
        npc->act = 1;
        npc->anim = 0;
      }

      break;

    case 3:
      npc->anim = 0;

      if (npc->shock || npc->x - (20 * 0x200) > player.x ||
          npc->x + (20 * 0x200) < player.x) {
        npc->anim = 1;
        npc->anim_wait = 0;
        npc->act = 4;
        npc->act_wait = 0;
      }

      break;

    case 4:
      npc->yvel += 0x20;
      if (npc->yvel > 0x5FF) npc->yvel = 0x5FF;

      if (++npc->act_wait < 20 && !(npc->flags & 8)) break;

      if (npc->flags & 8 || npc->y > player.y - (16 * 0x200)) {
        npc->anim_wait = 0;
        npc->anim = 2;
        npc->act = 5;
        npc->tgt_y = npc->y;

        if (npc->flags & 8) npc->yvel = -0x200;
      }

      break;

    case 5:
      if (++npc->anim_wait > 1) {
        npc->anim_wait = 0;
        ++npc->anim;
      }

      if (npc->anim > 4) npc->anim = 2;

      if (player.x < npc->x)
        npc->dir = 0;
      else
        npc->dir = 2;

      if (player.x < npc->x) npc->xvel -= 0x10;
      if (player.x > npc->x) npc->xvel += 0x10;

      if (npc->tgt_y < npc->y) npc->yvel -= 0x10;
      if (npc->tgt_y > npc->y) npc->yvel += 0x10;

      if (npc->xvel > 0x200) npc->xvel = 0x200;
      if (npc->xvel < -0x200) npc->xvel = -0x200;

      if (npc->yvel > 0x200) npc->yvel = 0x200;
      if (npc->yvel < -0x200) npc->yvel = -0x200;

      if (npc->flags & 8) npc->yvel = -0x200;
      if (npc->flags & 2) npc->yvel = 0x200;

      break;
  }

  npc->x += npc->xvel;
  npc->y += npc->yvel;

  if (npc->dir == 0)
    npc->rect = &rc_left[npc->anim];
  else
    npc->rect = &rc_right[npc->anim];
}

// Life capsule
void npc_act_032(npc_t *npc) {
  static const rect_t rect[2] = {
    {32, 96, 48, 112},
    {48, 96, 64, 112},
  };

  if (++npc->anim_wait > 2) {
    npc->anim_wait = 0;
    ++npc->anim;
  }

  if (npc->anim > 1) npc->anim = 0;

  npc->rect = &rect[npc->anim];
}

// Balrog bouncing projectile
void npc_act_033(npc_t *npc) {
  if (npc->flags & 5) {
    // SetCaret(npc->x, npc->y, CARET_PROJECTILE_DISSIPATION, DIR_LEFT);
    npc->cond = 0;
  } else if (npc->flags & 8) {
    npc->yvel = -0x400;
  }

  npc->yvel += 0x2A;

  npc->y += npc->yvel;
  npc->x += npc->xvel;

  static const rect_t rect_left[2] = {
    {240, 64, 256, 80},
    {240, 80, 256, 96},
  };

  if (++npc->anim_wait > 2) {
    npc->anim_wait = 0;

    if (++npc->anim > 1) npc->anim = 0;
  }

  npc->rect = &rect_left[npc->anim];

  if (++npc->act_wait > 250) {
    // SetCaret(npc->x, npc->y, CARET_PROJECTILE_DISSIPATION, DIR_LEFT);
    npc->cond = 0;
  }
}

// Bed
void npc_act_034(npc_t *npc) {
  static const rect_t rc_left = {192, 48, 224, 64};
  static const rect_t rc_right = {192, 184, 224, 200};

  if (npc->dir == 0)
    npc->rect = &rc_left;
  else
    npc->rect = &rc_right;
}

// Mannan
void npc_act_035(npc_t *npc) {
  if (npc->act < 3 && npc->life < 90) {
    snd_play_sound(-1, 71, SOUND_MODE_PLAY);
    npc_spawn_death_fx(npc->x, npc->y, npc->view.back, 8, 0);
    npc_spawn_exp(npc->x, npc->y, npc->info->exp);
    npc->act = 3;
    npc->act_wait = 0;
    npc->anim = 2;
    npc->bits &= ~NPC_SHOOTABLE;
    npc->damage = 0;
  }

  switch (npc->act) {
    case 0:
    case 1:
      if (npc->shock) {
        if (npc->dir == 0)
          npc_spawn(103, npc->x - (8 * 0x200), npc->y + (8 * 0x200), 0, 0, npc->dir, NULL, 0x100);
        else
          npc_spawn(103, npc->x + (8 * 0x200), npc->y + (8 * 0x200), 0, 0, npc->dir, NULL, 0x100);

        npc->anim = 1;
        npc->act = 2;
        npc->act_wait = 0;
      }

      break;

    case 2:
      if (++npc->act_wait > 20) {
        npc->act_wait = 0;
        npc->act = 1;
        npc->anim = 0;
      }

      break;

    case 3:
      if (++npc->act_wait == 50 || npc->act_wait == 60) npc->anim = 3;

      if (npc->act_wait == 53 || npc->act_wait == 63) npc->anim = 2;

      if (npc->act_wait > 100) npc->act = 4;

      break;
  }

  static const rect_t rc_left[4] = {
    {96, 64, 120, 96},
    {120, 64, 144, 96},
    {144, 64, 168, 96},
    {168, 64, 192, 96},
  };

  static const rect_t rc_right[4] = {
    {96, 96, 120, 128},
    {120, 96, 144, 128},
    {144, 96, 168, 128},
    {168, 96, 192, 128},
  };

  if (npc->dir == 0)
    npc->rect = &rc_left[npc->anim];
  else
    npc->rect = &rc_right[npc->anim];
}

// Balrog (hover)
void npc_act_036(npc_t *npc) {
  int i;
  u8 deg;
  int xvel, yvel;

  switch (npc->act) {
    case 0:
      npc->act = 1;
      // Fallthrough
    case 1:
      if (++npc->act_wait > 12) {
        npc->act = 2;
        npc->act_wait = 0;
        npc->count1 = 3;
        npc->anim = 1;
      }

      break;

    case 2:
      if (++npc->act_wait > 16) {
        --npc->count1;
        npc->act_wait = 0;

        deg = m_atan2(npc->x - player.x, npc->y + (4 * 0x200) - player.y);
        deg += (u8)m_rand(-0x10, 0x10);
        yvel = m_sin(deg);
        xvel = m_cos(deg);

        npc_spawn(11, npc->x, npc->y + 0x800, xvel, yvel, 0, NULL, 0x100);
        snd_play_sound(-1, 39, SOUND_MODE_PLAY);

        if (npc->count1 == 0) {
          npc->act = 3;
          npc->act_wait = 0;
        }
      }

      break;

    case 3:
      if (++npc->act_wait > 3) {
        npc->act = 4;
        npc->act_wait = 0;
        npc->xvel = (player.x - npc->x) / 100;
        npc->yvel = -0x600;
        npc->anim = 3;
      }

      break;

    case 4:
      if (npc->yvel > -0x200) {
        if (npc->life > 60) {
          npc->act = 5;
          npc->anim = 4;
          npc->anim_wait = 0;
          npc->act_wait = 0;
          npc->tgt_y = npc->y;
        } else {
          npc->act = 6;
        }
      }

      break;

    case 5:
      if (++npc->anim_wait > 1) {
        npc->anim_wait = 0;
        ++npc->anim;
      }

      if (npc->anim > 5) {
        npc->anim = 4;
        snd_play_sound(-1, 47, SOUND_MODE_PLAY);
      }

      if (++npc->act_wait > 100) {
        npc->act = 6;
        npc->anim = 3;
      }

      if (npc->y < npc->tgt_y)
        npc->yvel += 0x40;
      else
        npc->yvel -= 0x40;

      if (npc->yvel < -0x200) npc->yvel = -0x200;
      if (npc->yvel > 0x200) npc->yvel = 0x200;

      break;

    case 6:
      if (npc->y + (16 * 0x200) < player.y)
        npc->damage = 10;
      else
        npc->damage = 0;

      if (npc->flags & 8) {
        npc->act = 7;
        npc->act_wait = 0;
        npc->anim = 2;
        snd_play_sound(-1, 26, SOUND_MODE_PLAY);
        snd_play_sound(-1, 25, SOUND_MODE_PLAY);
        // SetQuake(30);
        npc->damage = 0;

        for (i = 0; i < 8; ++i)
          npc_spawn(4, npc->x + (m_rand(-12, 12) * 0x200),
                    npc->y + (m_rand(-12, 12) * 0x200), m_rand(-341, 341),
                    m_rand(-0x600, 0), 0, NULL, 0x100);

        for (i = 0; i < 8; ++i)
          npc_spawn(33, npc->x + (m_rand(-12, 12) * 0x200),
                    npc->y + (m_rand(-12, 12) * 0x200), m_rand(-0x400, 0x400),
                    m_rand(-0x400, 0), 0, NULL, 0x100);
      }

      break;

    case 7:
      npc->xvel = 0;

      if (++npc->act_wait > 3) {
        npc->act = 1;
        npc->act_wait = 0;
      }

      break;
  }

  if (npc->act != 5) {
    npc->yvel += 0x200 / 10;

    if (npc->x < player.x)
      npc->dir = 2;
    else
      npc->dir = 0;
  }

  if (npc->yvel > 0x5FF) npc->yvel = 0x5FF;

  npc->x += npc->xvel;
  npc->y += npc->yvel;

  static const rect_t rect_left[6] = {
    {0, 0, 40, 24},    {40, 0, 80, 24},    {80, 0, 120, 24},
    {120, 0, 160, 24}, {160, 48, 200, 72}, {200, 48, 240, 72},
  };

  static const rect_t rect_right[6] = {
    {0, 24, 40, 48},    {40, 24, 80, 48},   {80, 24, 120, 48},
    {120, 24, 160, 48}, {160, 72, 200, 96}, {200, 72, 240, 96},
  };

  if (npc->dir == 0)
    npc->rect = &rect_left[npc->anim];
  else
    npc->rect = &rect_right[npc->anim];
}

// Signpost
void npc_act_037(npc_t *npc) {
  static const rect_t rect[2] = {
    {192, 64, 208, 80},
    {208, 64, 224, 80},
  };

  if (++npc->anim_wait > 1) {
    npc->anim_wait = 0;
    ++npc->anim;
  }

  if (npc->anim > 1) npc->anim = 0;

  npc->rect = &rect[npc->anim];
}

// Fireplace
void npc_act_038(npc_t *npc) {
  static const rect_t rect[4] = {
    {128, 64, 144, 80},
    {144, 64, 160, 80},
    {160, 64, 176, 80},
    {176, 64, 192, 80},
  };

  switch (npc->act) {
    case 0:
      if (++npc->anim_wait > 3) {
        npc->anim_wait = 0;
        ++npc->anim;
      }

      if (npc->anim > 3) npc->anim = 0;

      npc->rect = &rect[npc->anim];
      break;

    case 10:
      npc->act = 11;
      npc_spawn_death_fx(npc->x, npc->y, npc->view.back, 8, 0);
      // Fallthrough
    case 11:
      npc->rect = NULL;
      break;
  }
}

// Save sign
void npc_act_039(npc_t *npc) {
  static const rect_t rect[2] = {
    {224, 64, 240, 80},
    {240, 64, 256, 80},
  };

  if (npc->dir == 0)
    npc->anim = 0;
  else
    npc->anim = 1;

  npc->rect = &rect[npc->anim];
}
