#include "game/npc_act/npc_act.h"

// Shovel Brigade
void npc_act_220(npc_t *npc) {
  static const rect_t rc_left[2] = {
    {0, 64, 16, 80},
    {16, 64, 32, 80},
  };

  static const rect_t rc_right[2] = {
    {0, 80, 16, 96},
    {16, 80, 32, 96},
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

// Shovel Brigade (walking)
void npc_act_221(npc_t *npc) {
  static const rect_t rc_left[6] = {
    {0, 64, 16, 80}, {16, 64, 32, 80}, {32, 64, 48, 80},
    {0, 64, 16, 80}, {48, 64, 64, 80}, {0, 64, 16, 80},
  };

  static const rect_t rc_right[6] = {
    {0, 80, 16, 96}, {16, 80, 32, 96}, {32, 80, 48, 96},
    {0, 80, 16, 96}, {48, 80, 64, 96}, {0, 80, 16, 96},
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

// Prison bars
void npc_act_222(npc_t *npc) {
  static const rect_t rc = {96, 168, 112, 200};

  if (npc->act == 0) {
    ++npc->act;
    npc->y -= 8 * 0x200;
  }

  npc->rect = &rc;
}

// Momorin
void npc_act_223(npc_t *npc) {
  static const rect_t rc_left[3] = {
    {80, 192, 96, 216},
    {96, 192, 112, 216},
    {112, 192, 128, 216},
  };

  static const rect_t rc_right[3] = {
    {80, 216, 96, 240},
    {96, 216, 112, 240},
    {112, 216, 128, 240},
  };

  switch (npc->act) {
    case 0:
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

    case 3:
      npc->anim = 2;
      break;
  }

  if (npc->act < 2 && player.y < npc->y + (16 * 0x200) && player.y > npc->y - (16 * 0x200)) {
    if (player.x < npc->x)
      npc->dir = 0;
    else
      npc->dir = 2;
  }

  if (npc->dir == 0)
    npc->rect = &rc_left[npc->anim];
  else
    npc->rect = &rc_right[npc->anim];
}

// Chie
void npc_act_224(npc_t *npc) {
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

  if (npc->act < 2 && player.y < npc->y + (16 * 0x200) && player.y > npc->y - (16 * 0x200)) {
    if (player.x < npc->x)
      npc->dir = 0;
    else
      npc->dir = 2;
  }

  if (npc->dir == 0)
    npc->rect = &rc_left[npc->anim];
  else
    npc->rect = &rc_right[npc->anim];
}

// Megane
void npc_act_225(npc_t *npc) {
  static const rect_t rc_left[2] = {
    {64, 64, 80, 80},
    {80, 64, 96, 80},
  };

  static const rect_t rc_right[2] = {
    {64, 80, 80, 96},
    {80, 80, 96, 96},
  };

  switch (npc->act) {
    case 0:
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

  if (npc->dir == 0)
    npc->rect = &rc_left[npc->anim];
  else
    npc->rect = &rc_right[npc->anim];
}

// Kanpachi
void npc_act_226(npc_t *npc) {
  static const rect_t rc_right[7] = {
    {256, 56, 272, 80}, {272, 56, 288, 80}, {288, 56, 304, 80}, {256, 56, 272, 80},
    {304, 56, 320, 80}, {256, 56, 272, 80}, {240, 56, 256, 80},
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

      break;

    case 2:
      if (++npc->act_wait > 8) {
        npc->act = 1;
        npc->anim = 0;
      }

      break;

    case 10:
      npc->act = 11;
      npc->anim = 2;
      npc->anim_wait = 0;
      // Fallthrough
    case 11:
      npc->xvel = 0x200;

      if (++npc->anim_wait > 4) {
        npc->anim_wait = 0;
        ++npc->anim;
      }

      if (npc->anim > 5) npc->anim = 2;

      ++npc->act_wait;

      break;

    case 20:
      npc->xvel = 0;
      npc->anim = 6;
      break;
  }

  npc->yvel += 0x20;
  if (npc->yvel > 0x5FF) npc->yvel = 0x5FF;

  npc->x += npc->xvel;
  npc->y += npc->yvel;

  npc->rect = &rc_right[npc->anim];
}

// Bucket
void npc_act_227(npc_t *npc) {
  static const rect_t rc = {208, 32, 224, 48};
  npc->rect = &rc;
}

// Droll (guard)
void npc_act_228(npc_t *npc) {
  static const rect_t rc_left[4] = {
    {0, 0, 32, 40},
    {32, 0, 64, 40},
    {64, 0, 96, 40},
    {96, 0, 128, 40},
  };

  static const rect_t rc_right[4] = {
    {0, 40, 32, 80},
    {32, 40, 64, 80},
    {64, 40, 96, 80},
    {96, 40, 128, 80},
  };

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

      if (++npc->anim_wait > 50) {
        npc->anim_wait = 0;
        ++npc->anim;
      }

      if (npc->anim > 1) npc->anim = 0;

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

        if (npc->dir == 0)
          npc->xvel = -0x200;
        else
          npc->xvel = 0x200;
      }

      break;

    case 12:
      if (npc->flags & 8) {
        npc->anim = 2;
        npc->act = 13;
        npc->act_wait = 0;
      }

      break;

    case 13:
      npc->xvel /= 2;

      if (++npc->act_wait > 10) npc->act = 1;

      break;
  }

  npc->yvel += 0x40;
  if (npc->yvel > 0x5FF) npc->yvel = 0x5FF;

  npc->x += npc->xvel;
  npc->y += npc->yvel;

  if (npc->dir == 0)
    npc->rect = &rc_left[npc->anim];
  else
    npc->rect = &rc_right[npc->anim];
}

// Red Flowers (sprouts)
void npc_act_229(npc_t *npc) {
  static const rect_t rc[2] = {
    {0, 96, 48, 112},
    {0, 112, 48, 128},
  };

  switch (npc->act) {
    case 0:
      npc->act = 1;
      npc->y -= 16 * 0x200;
      break;
  }

  if (npc->dir == 0)
    npc->rect = &rc[0];
  else
    npc->rect = &rc[1];
}

// Red Flowers (blooming)
void npc_act_230(npc_t *npc) {
  static const rect_t rc[2] = {
    {48, 96, 96, 128},
    {96, 96, 144, 128},
  };

  switch (npc->act) {
    case 0:
      npc->act = 1;
      npc->x -= 16 * 0x200;
      npc->y -= 16 * 0x200;
      break;
  }

  if (npc->dir == 0)
    npc->rect = &rc[0];
  else
    npc->rect = &rc[1];
}

// Rocket
void npc_act_231(npc_t *npc) {
  int i;

  static const rect_t rc[2] = {
    {176, 32, 208, 48},
    {176, 48, 208, 64},
  };

  switch (npc->act) {
    case 0:
      npc->act = 1;
      // Fallthrough
    case 1:
      npc->anim = 0;
      break;

    case 10:
      npc->act = 11;
      npc->act_wait = 0;
      // Fallthrough
    case 11:
      ++npc->act_wait;
      npc->yvel += 8;

      if (npc->flags & 8) {
        if (npc->act_wait < 10)
          npc->act = 12;
        else
          npc->act = 1;
      }

      break;

    case 12:
      npc->bits &= ~NPC_INTERACTABLE;
      npc->act = 13;
      npc->act_wait = 0;
      npc->anim = 1;

      for (i = 0; i < 10; ++i) {
        npc_spawn(4, npc->x + (m_rand(-16, 16) * 0x200), npc->y + (m_rand(-8, 8) * 0x200), 0, 0, 0, NULL, 0x100);
        snd_play_sound(PRIO_NORMAL, 12, FALSE);  // Wait, it does this in a loop?
      }
      // Fallthrough
    case 13:
      npc->yvel -= 8;

      ++npc->act_wait;

      if (npc->act_wait % 2 == 0) caret_spawn(npc->x - (10 * 0x200), npc->y + (8 * 0x200), CARET_EXHAUST, DIR_DOWN);
      if (npc->act_wait % 2 == 1) caret_spawn(npc->x + (10 * 0x200), npc->y + (8 * 0x200), CARET_EXHAUST, DIR_DOWN);

      if (npc->act_wait % 4 == 1) snd_play_sound(PRIO_NORMAL, 34, FALSE);

      if (npc->flags & 2 || player.flags & 2 || npc->act_wait > 450) {
        if (npc->flags & 2 || player.flags & 2) npc->yvel = 0;

        npc->act = 15;

        for (i = 0; i < 6; ++i) {
          npc_spawn(4, npc->x + (m_rand(-16, 16) * 0x200), npc->y + (m_rand(-8, 8) * 0x200), 0, 0, 0, NULL, 0x100);
          snd_play_sound(PRIO_NORMAL, 12, FALSE);  // Here it does it again...
        }
      }

      break;

    case 15:
      npc->yvel += 8;
      ++npc->act_wait;

      if (npc->yvel < 0) {
        if (npc->act_wait % 8 == 0) caret_spawn(npc->x - (10 * 0x200), npc->y + (8 * 0x200), CARET_EXHAUST, DIR_DOWN);
        if (npc->act_wait % 8 == 4) caret_spawn(npc->x + (10 * 0x200), npc->y + (8 * 0x200), CARET_EXHAUST, DIR_DOWN);
        if (npc->act_wait % 16 == 1) snd_play_sound(PRIO_NORMAL, 34, FALSE);
      }

      if (npc->flags & 8) {
        npc->bits |= NPC_INTERACTABLE;
        npc->act = 1;
        npc->anim = 0;
      }

      break;
  }

  if (npc->yvel < -0x5FF) npc->yvel = -0x5FF;
  if (npc->yvel > 0x5FF) npc->yvel = 0x5FF;

  npc->y += npc->yvel;

  npc->rect = &rc[npc->anim];
}

// Orangebell
void npc_act_232(npc_t *npc) {
  int i;

  switch (npc->act) {
    case 0:
      npc->act = 1;
      npc->tgt_x = npc->x;
      npc->tgt_y = npc->y;
      npc->yvel = 0x200;

      for (i = 0; i < 8; ++i) npc_spawn(233, npc->x, npc->y, 0, 0, npc->dir, npc, 0x100);
      // Fallthrough
    case 1:
      if (npc->xvel < 0 && npc->flags & 1) npc->dir = 2;
      if (npc->xvel > 0 && npc->flags & 4) npc->dir = 0;

      if (npc->dir == 0)
        npc->xvel = -0x100;
      else
        npc->xvel = 0x100;

      if (npc->y < npc->tgt_y)
        npc->yvel += 8;
      else
        npc->yvel -= 8;

      if (npc->yvel > 0x200) npc->yvel = 0x200;
      if (npc->yvel < -0x200) npc->yvel = -0x200;

      if (++npc->anim_wait > 5) {
        npc->anim_wait = 0;
        ++npc->anim;
      }

      if (npc->anim > 2) npc->anim = 0;

      break;
  }

  npc->y += npc->yvel;
  npc->x += npc->xvel;

  static const rect_t rc_left[3] = {
    {128, 0, 160, 32},
    {160, 0, 192, 32},
    {192, 0, 224, 32},
  };

  static const rect_t rc_right[3] = {
    {128, 32, 160, 64},
    {160, 32, 192, 64},
    {192, 32, 224, 64},
  };

  if (npc->dir == 0)
    npc->rect = &rc_left[npc->anim];
  else
    npc->rect = &rc_right[npc->anim];
}

// Orangebell bat
void npc_act_233(npc_t *npc) {
  u8 deg;

  switch (npc->act) {
    case 0:
      npc->act = 1;

      deg = m_rand(0, 0xFF);
      npc->xvel = m_cos(deg);
      deg = m_rand(0, 0xFF);
      npc->yvel = m_sin(deg);

      npc->count1 = 120;
      npc->count2 = m_rand(-0x20, 0x20) * 0x200;
      // Fallthrough
    case 1:
      if (npc->parent->class_num == 232) {
        npc->tgt_x = npc->parent->x;
        npc->tgt_y = npc->parent->y;
        npc->dir = npc->parent->dir;
      }

      if (npc->tgt_x < npc->x) npc->xvel -= 8;
      if (npc->tgt_x > npc->x) npc->xvel += 8;

      if (npc->tgt_y + npc->count2 < npc->y) npc->yvel -= 0x20;
      if (npc->tgt_y + npc->count2 > npc->y) npc->yvel += 0x20;

      if (npc->xvel > 0x400) npc->xvel = 0x400;
      if (npc->xvel < -0x400) npc->xvel = -0x400;

      if (npc->yvel > 0x400) npc->yvel = 0x400;
      if (npc->yvel < -0x400) npc->yvel = -0x400;

      if (npc->count1 < 120) {
        ++npc->count1;
        break;
      }

      if (npc->x - (8 * 0x200) < player.x && npc->x + (8 * 0x200) > player.x && npc->y < player.y &&
          npc->y + (176 * 0x200) > player.y) {
        npc->xvel /= 4;
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

  static const rect_t rc_left[4] = {
    {256, 0, 272, 16},
    {272, 0, 288, 16},
    {288, 0, 304, 16},
    {304, 0, 320, 16},
  };

  static const rect_t rc_right[4] = {
    {256, 16, 272, 32},
    {272, 16, 288, 32},
    {288, 16, 304, 32},
    {304, 16, 320, 32},
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
    npc->rect = &rc_left[npc->anim];
  else
    npc->rect = &rc_right[npc->anim];
}

// Red Flowers (picked)
void npc_act_234(npc_t *npc) {
  static const rect_t rc[2] = {
      {144, 96, 192, 112},
      {144, 112, 192, 128},
  };

  switch (npc->act) {
    case 0:
      npc->act = 1;
      npc->y += 16 * 0x200;
      break;
  }

  if (npc->dir == 0)
    npc->rect = &rc[0];
  else
    npc->rect = &rc[1];
}

// Midorin
void npc_act_235(npc_t *npc) {
  static const rect_t rc_left[4] = {
    {192, 96, 208, 112},
    {208, 96, 224, 112},
    {224, 96, 240, 112},
    {192, 96, 208, 112},
  };

  static const rect_t rc_right[4] = {
    {192, 112, 208, 128},
    {208, 112, 224, 128},
    {224, 112, 240, 128},
    {192, 112, 208, 128},
  };

  switch (npc->act) {
    case 0:
      npc->act = 1;
      npc->anim = 0;
      npc->anim_wait = 0;
      npc->xvel = 0;
      // Fallthrough
    case 1:
      if (m_rand(0, 30) == 1) {
        npc->act = 2;
        npc->act_wait = 0;
        npc->anim = 1;
      }

      if (m_rand(0, 30) == 1) {
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
        npc->xvel = -0x400;
      else
        npc->xvel = 0x400;

      if (++npc->anim_wait > 1) {
        npc->anim_wait = 0;
        ++npc->anim;
      }

      if (npc->anim > 3) npc->anim = 2;

      if (++npc->act_wait > 64) npc->act = 0;

      break;
  }

  npc->yvel += 0x20;
  if (npc->yvel > 0x5FF) npc->yvel = 0x5FF;

  npc->x += npc->xvel;
  npc->y += npc->yvel;

  if (npc->anim == 2)
    npc->hit.top = 5 * 0x200;
  else
    npc->hit.top = 4 * 0x200;

  if (npc->dir == 0)
    npc->rect = &rc_left[npc->anim];
  else
    npc->rect = &rc_right[npc->anim];
}

// Gunfish
void npc_act_236(npc_t *npc) {
  static const rect_t rc_left[6] = {
    {128, 64, 152, 88}, {152, 64, 176, 88}, {176, 64, 200, 88},
    {200, 64, 224, 88}, {224, 64, 248, 88}, {248, 64, 272, 88},
  };

  static const rect_t rc_right[6] = {
    {128, 88, 152, 112}, {152, 88, 176, 112}, {176, 88, 200, 112},
    {200, 88, 224, 112}, {224, 88, 248, 112}, {248, 88, 272, 112},
  };

  switch (npc->act) {
    case 0:
      npc->act = 1;
      npc->act_wait = m_rand(0, 50);
      npc->tgt_x = npc->x;
      npc->tgt_y = npc->y;
      npc->yvel = 0;
      // Fallthrough
    case 1:
      if (npc->act_wait != 0) {
        --npc->act_wait;
      } else {
        npc->yvel = 0x200;
        npc->act = 2;
      }

      break;

    case 2:
      if (npc->x < player.x)
        npc->dir = 2;
      else
        npc->dir = 0;

      if (player.x < npc->x + (128 * 0x200) && player.x > npc->x - (128 * 0x200) && player.y < npc->y + (32 * 0x200) &&
          player.y > npc->y - (160 * 0x200))
        ++npc->act_wait;

      if (npc->act_wait > 80) {
        npc->act = 10;
        npc->act_wait = 0;
      }

      if (++npc->anim_wait > 1) {
        npc->anim_wait = 0;
        ++npc->anim;
      }

      if (npc->anim > 1) npc->anim = 0;

      break;

    case 10:
      if (++npc->act_wait > 20) {
        npc->act_wait = 0;
        npc->act = 20;
      }

      if (++npc->anim_wait > 1) {
        npc->anim_wait = 0;
        ++npc->anim;
      }

      if (npc->anim > 3) npc->anim = 2;

      break;

    case 20:
      if (++npc->act_wait > 60) {
        npc->act_wait = 0;
        npc->act = 2;
      }

      if (npc->act_wait % 10 == 3) {
        snd_play_sound(PRIO_NORMAL, 39, FALSE);

        if (npc->dir == 0)
          npc_spawn(237, npc->x - (8 * 0x200), npc->y - (8 * 0x200), -0x400, -0x400, 0, NULL, 0x100);
        else
          npc_spawn(237, npc->x + (8 * 0x200), npc->y - (8 * 0x200), 0x400, -0x400, 0, NULL, 0x100);
      }

      if (++npc->anim_wait > 1) {
        npc->anim_wait = 0;
        ++npc->anim;
      }

      if (npc->anim > 5) npc->anim = 4;

      break;
  }

  if (npc->y < npc->tgt_y)
    npc->yvel += 0x10;
  else
    npc->yvel -= 0x10;

  if (npc->yvel > 0x100) npc->yvel = 0x100;
  if (npc->yvel < -0x100) npc->yvel = -0x100;

  npc->y += npc->yvel;

  if (npc->dir == 0)
    npc->rect = &rc_left[npc->anim];
  else
    npc->rect = &rc_right[npc->anim];
}

// Gunfish projectile
void npc_act_237(npc_t *npc) {
  static const rect_t rc = {312, 32, 320, 40};

  int i;
  bool hit;

  switch (npc->act) {
    case 0:
      npc->act = 1;
      // Fallthrough
    case 1:
      hit = FALSE;

      ++npc->act_wait;

      if (npc->flags & 0xFF) hit = TRUE;

      if (npc->act_wait > 10 && npc->flags & 0x100) hit = TRUE;

      if (hit) {
        for (i = 0; i < 5; ++i) caret_spawn(npc->x, npc->y, CARET_BUBBLE, DIR_LEFT);

        snd_play_sound(PRIO_NORMAL, 21, FALSE);
        npc->cond = 0;
        return;
      }

      break;
  }

  npc->yvel += 0x20;
  if (npc->yvel > 0x5FF) npc->yvel = 0x5FF;

  npc->x += npc->xvel;
  npc->y += npc->yvel;

  npc->rect = &rc;
}

// Press (sideways)
void npc_act_238(npc_t *npc) {
  int i;

  static const rect_t rc[3] = {
    {184, 200, 208, 216},
    {208, 200, 232, 216},
    {232, 200, 256, 216},
  };

  switch (npc->act) {
    case 0:
      npc->act = 1;
      npc->tgt_x = npc->x;
      npc->tgt_y = npc->y;
      npc->view.front = 16 * 0x200;
      npc->view.back = 8 * 0x200;
      // Fallthrough
    case 1:
      if (npc->dir == 0 && player.x < npc->x && player.x > npc->x - (192 * 0x200) && player.y > npc->y - (4 * 0x200) &&
          player.y < npc->y + (8 * 0x200)) {
        npc->act = 10;
        npc->act_wait = 0;
        npc->anim = 2;
      }

      if (npc->dir == 2 && player.x > npc->x && player.x < npc->x + (192 * 0x200) && player.y > npc->y - (4 * 0x200) &&
          player.y < npc->y + (8 * 0x200)) {
        npc->act = 10;
        npc->act_wait = 0;
        npc->anim = 2;
      }

      break;

    case 10:
      npc->damage = 0x7F;

      if (npc->dir == 0)
        npc->x -= 6 * 0x200;
      else
        npc->x += 6 * 0x200;

      if (++npc->act_wait == 8) {
        npc->act = 20;
        npc->act_wait = 0;

        for (i = 0; i < 4; ++i) {
          npc_spawn(4, npc->x + (m_rand(-16, 16) * 0x200), npc->y + (m_rand(-8, 8) * 0x200), 0, 0, 0, NULL, 0x100);
          snd_play_sound(PRIO_NORMAL, 12, FALSE);
        }
      }

      break;

    case 20:
      npc->damage = 0;

      if (++npc->act_wait > 50) {
        npc->act_wait = 0;
        npc->act = 30;
      }

      break;

    case 30:
      npc->damage = 0;
      npc->anim = 1;

      if (++npc->act_wait == 12) {
        npc->act = 1;
        npc->act_wait = 0;
        npc->anim = 0;
      }

      if (npc->dir == 0)
        npc->x += 4 * 0x200;
      else
        npc->x -= 4 * 0x200;

      break;
  }

  if (npc->dir == 0 && player.x < npc->x)
    npc->hit.back = 16 * 0x200;
  else if (npc->dir == 2 && player.x > npc->x)
    npc->hit.back = 16 * 0x200;
  else
    npc->hit.back = 8 * 0x200;

  npc->rect = &rc[npc->anim];
}

// Cage bars
void npc_act_239(npc_t *npc) {
  static const rect_t rc_left = {192, 48, 256, 80};
  static const rect_t rc_right = {96, 112, 144, 144};

  switch (npc->act) {
    case 0:
      npc->act = 1;

      if (npc->dir == 0) {
        npc->x += 8 * 0x200;
        npc->y += 16 * 0x200;
      } else {
        npc->view.front = 24 * 0x200;
        npc->view.back = 24 * 0x200;
        npc->view.top = 8 * 0x200;
        npc->view.back = 24 * 0x200;
      }

      break;
  }

  if (npc->dir == 0)
    npc->rect = &rc_left;
  else
    npc->rect = &rc_right;
}
