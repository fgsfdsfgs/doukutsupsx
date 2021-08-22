#include "game/npc_act/npc_act.h"

// Grate
void npc_act_100(npc_t *npc) {
  static const rect_t rc[2] = {
    {272, 48, 288, 64},
    {272, 48, 288, 64},
  };

  switch (npc->act) {
    case 0:
      npc->y += 16 * 0x200;
      npc->act = 1;
      break;
  }

  if (npc->dir == 0)
    npc->rect = &rc[0];
  else
    npc->rect = &rc[1];
}

// Malco computer screen
void npc_act_101(npc_t *npc) {
  static const rect_t rect[3] = {
    {240, 136, 256, 152},
    {240, 136, 256, 152},
    {256, 136, 272, 152},
  };

  if (++npc->anim_wait > 3) {
    npc->anim_wait = 0;
    ++npc->anim;
  }

  if (npc->anim > 2) npc->anim = 0;

  npc->rect = &rect[npc->anim];
}

// Malco computer wave
void npc_act_102(npc_t *npc) {
  static const rect_t rect[4] = {
    {208, 120, 224, 136},
    {224, 120, 240, 136},
    {240, 120, 256, 136},
    {256, 120, 272, 136},
  };

  if (npc->act == 0) {
    npc->act = 1;
    npc->y += 8 * 0x200;
  }

  if (++npc->anim_wait > 0) {
    npc->anim_wait = 0;
    ++npc->anim;
  }

  if (npc->anim > 3) npc->anim = 0;

  npc->rect = &rect[npc->anim];
}

// Mannan projectile
void npc_act_103(npc_t *npc) {
  static const rect_t rc_left[3] = {
    {192, 96, 208, 120},
    {208, 96, 224, 120},
    {224, 96, 240, 120},
  };

  static const rect_t rc_right[3] = {
    {192, 120, 208, 144},
    {208, 120, 224, 144},
    {224, 120, 240, 144},
  };

  switch (npc->act) {
    case 0:
      npc->act = 1;
      // Fallthrough
    case 1:
      if (npc->dir == 0)
        npc->xvel -= 0x20;
      else
        npc->xvel += 0x20;

      if (++npc->anim_wait > 0) {
        npc->anim_wait = 0;
        ++npc->anim;
      }

      if (npc->anim > 2) npc->anim = 0;

      break;
  }

  npc->x += npc->xvel;

  if (npc->dir == 0)
    npc->rect = &rc_left[npc->anim];
  else
    npc->rect = &rc_right[npc->anim];

  if (++npc->count1 > 100) npc->cond = 0;

  if (npc->count1 % 4 == 1) snd_play_sound(PRIO_NORMAL, 46, FALSE);
}

// Frog
void npc_act_104(npc_t *npc) {
  bool do_jump;

  static const rect_t rc_left[3] = {
    {0, 112, 32, 144},
    {32, 112, 64, 144},
    {64, 112, 96, 144},
  };

  static const rect_t rc_right[3] = {
    {0, 144, 32, 176},
    {32, 144, 64, 176},
    {64, 144, 96, 176},
  };

  switch (npc->act) {
    case 0:
      npc->act = 1;
      npc->act_wait = 0;
      npc->xvel = 0;
      npc->yvel = 0;

      if (npc->dir == 4) {
        if (m_rand(0, 1))
          npc->dir = 0;
        else
          npc->dir = 2;

        npc->bits |= NPC_IGNORE_SOLIDITY;
        npc->anim = 2;
        npc->act = 3;
        break;
      }

      npc->bits &= ~NPC_IGNORE_SOLIDITY;
      // Fallthrough
    case 1:
      ++npc->act_wait;

      if (m_rand(0, 50) == 1) {
        npc->act = 2;
        npc->act_wait = 0;
        npc->anim = 0;
        npc->anim_wait = 0;
        break;
      }

      break;

    case 2:
      ++npc->act_wait;

      if (++npc->anim_wait > 2) {
        npc->anim_wait = 0;
        ++npc->anim;
      }

      if (npc->anim > 1) npc->anim = 0;

      if (npc->act_wait > 18) {
        npc->act = 1;
        npc->act = 1;  // Duplicate line
      }

      break;

    case 3:
      if (++npc->act_wait > 40) npc->bits &= ~NPC_IGNORE_SOLIDITY;

      if (npc->flags & 8) {
        npc->act = 0;
        npc->anim = 0;
        npc->act_wait = 0;
      }

      break;

    case 10:
      npc->act = 11;
      // Fallthrough
    case 11:
      if (npc->flags & 1 && npc->xvel < 0) {
        npc->xvel *= -1;
        npc->dir = 2;
      }

      if (npc->flags & 4 && npc->xvel > 0) {
        npc->xvel *= -1;
        npc->dir = 0;
      }

      if (npc->flags & 8) {
        npc->act = 0;
        npc->anim = 0;
        npc->act_wait = 0;
      }

      break;
  }

  do_jump = FALSE;

  if (npc->act < 10 && npc->act != 3 && npc->act_wait > 10) {
    if (npc->shock) do_jump = TRUE;

    if (npc->x < player.x - (160 * 0x200) || npc->x > player.x + (160 * 0x200) || npc->y < player.y - (64 * 0x200) ||
        npc->y > player.y + (64 * 0x200)) {
      // This blank space is needed for the function to produce the same assembly.
      // Chances are there used to be some commented-out code here.
    } else {
      if (m_rand(0, 50) == 2) do_jump = TRUE;
    }
  }

  if (do_jump) {
    if (npc->x < player.x)
      npc->dir = 2;
    else
      npc->dir = 0;

    npc->act = 10;
    npc->anim = 2;
    npc->yvel = -0x5FF;

    if (!(player.cond & PLRCOND_INVISIBLE)) snd_play_sound(PRIO_NORMAL, 30, FALSE);

    if (npc->dir == 0)
      npc->xvel = -0x200;
    else
      npc->xvel = 0x200;
  }

  npc->yvel += 0x80;
  if (npc->yvel > 0x5FF) npc->yvel = 0x5FF;

  npc->x += npc->xvel;
  npc->y += npc->yvel;

  if (npc->dir == 0)
    npc->rect = &rc_left[npc->anim];
  else
    npc->rect = &rc_right[npc->anim];
}

// "HEY!" speech bubble (low)
void npc_act_105(npc_t *npc) {
  static const rect_t rect[2] = {
    {128, 32, 144, 48},
    {128, 32, 128, 32},
  };

  if (++npc->act_wait > 30) npc->cond = 0;

  if (npc->act_wait < 5) npc->y -= 1 * 0x200;

  npc->rect = &rect[npc->anim];
}

// "HEY!" speech bubble (high)
void npc_act_106(npc_t *npc) {
  switch (npc->act) {
    case 0:
      npc_spawn(105, npc->x, npc->y - (8 * 0x200), 0, 0, 0, NULL, 0x180);
      npc->act = 1;
      break;
  }
}

// Malco
void npc_act_107(npc_t *npc) {
  int i;

  switch (npc->act) {
    case 0:
      npc->act = 1;

      if (npc->dir == 2) npc->anim = 5;

      break;

    case 10:
      npc->act = 11;
      npc->act_wait = 0;
      npc->anim_wait = 0;

      for (i = 0; i < 4; ++i) npc_spawn(4, npc->x, npc->y, m_rand(-341, 341), m_rand(-0x600, 0), 0, NULL, 0x100);

      // Fallthrough
    case 11:
      if (++npc->anim_wait > 1) {
        snd_play_sound(PRIO_NORMAL, 43, FALSE);
        npc->anim_wait = 0;
        ++npc->anim;
      }

      if (npc->anim > 1) npc->anim = 0;

      if (++npc->act_wait > 100) npc->act = 12;

      break;

    case 12:
      npc->act = 13;
      npc->act_wait = 0;
      npc->anim = 1;
      // Fallthrough
    case 13:
      if (++npc->act_wait > 50) npc->act = 14;

      break;

    case 14:
      npc->act = 15;
      npc->act_wait = 0;
      // Fallthrough
    case 15:
      if (npc->act_wait / 2 % 2) {
        npc->x += 1 * 0x200;
        snd_play_sound(PRIO_NORMAL, 11, FALSE);
      } else {
        npc->x -= 1 * 0x200;
      }

      if (++npc->act_wait > 50) npc->act = 16;

      break;

    case 16:
      npc->act = 17;
      npc->act_wait = 0;
      npc->anim = 2;
      snd_play_sound(PRIO_NORMAL, 12, FALSE);

      for (i = 0; i < 8; ++i) npc_spawn(4, npc->x, npc->y, m_rand(-341, 341), m_rand(-0x600, 0), 0, NULL, 0x100);

      // Fallthrough
    case 17:
      if (++npc->act_wait > 150) npc->act = 18;

      break;

    case 18:
      npc->act = 19;
      npc->act_wait = 0;
      npc->anim = 3;
      npc->anim_wait = 0;
      // Fallthrough
    case 19:
      if (++npc->anim_wait > 3) {
        npc->anim_wait = 0;
        ++npc->anim;
      }

      if (npc->anim > 4) {
        snd_play_sound(PRIO_NORMAL, 11, FALSE);
        npc->anim = 3;
      }

      if (++npc->act_wait > 100) {
        npc->act = 20;
        snd_play_sound(PRIO_NORMAL, 12, FALSE);

        for (i = 0; i < 4; ++i) npc_spawn(4, npc->x, npc->y, m_rand(-341, 341), m_rand(-0x600, 0), 0, NULL, 0x100);
      }

      break;

    case 20:
      npc->anim = 4;
      break;

    case 21:
      npc->act = 22;
      npc->anim = 5;
      snd_play_sound(PRIO_NORMAL, 51, FALSE);
      break;

    case 100:
      npc->act = 101;
      npc->anim = 6;
      npc->anim_wait = 0;
      // Fallthrough
    case 101:
      if (++npc->anim_wait > 4) {
        npc->anim_wait = 0;
        ++npc->anim;
      }

      if (npc->anim > 9) npc->anim = 6;

      break;

    case 110:
      npc_spawn_death_fx(npc->x, npc->y, 0x2000, 16, 0);
      npc->cond = 0;
      break;
  }

  static const rect_t rcPoweron[10] = {
    {144, 0, 160, 24}, {160, 0, 176, 24}, {176, 0, 192, 24}, {192, 0, 208, 24}, {208, 0, 224, 24},
    {224, 0, 240, 24}, {176, 0, 192, 24}, {192, 0, 208, 24}, {208, 0, 224, 24}, {192, 0, 208, 24},
  };

  npc->rect = &rcPoweron[npc->anim];
}

// Balfrog projectile
void npc_act_108(npc_t *npc) {
  if (npc->flags & 0xFF) {
    caret_spawn(npc->x, npc->y, CARET_PROJECTILE_DISSIPATION, DIR_LEFT);
    npc->cond = 0;
  }

  npc->y += npc->yvel;
  npc->x += npc->xvel;

  static const rect_t rect_left[3] = {
    {96, 48, 112, 64},
    {112, 48, 128, 64},
    {128, 48, 144, 64},
  };

  if (++npc->anim_wait > 1) {
    npc->anim_wait = 0;
    ++npc->anim;
  }

  if (npc->anim > 2) npc->anim = 0;

  npc->rect = &rect_left[npc->anim];

  if (++npc->count1 > 300) {
    caret_spawn(npc->x, npc->y, CARET_PROJECTILE_DISSIPATION, DIR_LEFT);
    npc->cond = 0;
  }
}

// Malco (broken)
void npc_act_109(npc_t *npc) {
  int i;

  static const rect_t rc_left[2] = {
    {240, 0, 256, 24},
    {256, 0, 272, 24},
  };

  static const rect_t rc_right[2] = {
    {240, 24, 256, 48},
    {256, 24, 272, 48},
  };

  switch (npc->act) {
    case 0:
      if (--npc->act_wait != 0) npc->act = 1;

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

    case 10:
      npc->act = 0;
      snd_play_sound(PRIO_NORMAL, 12, FALSE);

      for (i = 0; i < 8; ++i)
        npc_spawn(4, npc->x, npc->y, m_rand(-341, 341), m_rand(-0x600, 0), 0, NULL, 0x100);

      break;
  }

  npc->yvel += 0x40;
  if (npc->yvel > 0x5FF) npc->yvel = 0x5FF;

  npc->y += npc->yvel;

  if (npc->dir == 0)
    npc->rect = &rc_left[npc->anim];
  else
    npc->rect = &rc_right[npc->anim];
}

// Puchi
void npc_act_110(npc_t *npc) {
  bool do_jump;

  static const rect_t rc_left[3] = {
      {96, 128, 112, 144},
      {112, 128, 128, 144},
      {128, 128, 144, 144},
  };

  static const rect_t rc_right[3] = {
      {96, 144, 112, 160},
      {112, 144, 128, 160},
      {128, 144, 144, 160},
  };

  switch (npc->act) {
    case 0:
      npc->act = 1;
      npc->act_wait = 0;
      npc->xvel = 0;
      npc->yvel = 0;

      if (npc->dir == 4) {
        if (m_rand(0, 1))
          npc->dir = 0;
        else
          npc->dir = 2;

        npc->bits |= NPC_IGNORE_SOLIDITY;
        npc->anim = 2;
        npc->act = 3;

        break;
      }

      npc->bits &= ~NPC_IGNORE_SOLIDITY;
      // Fallthrough
    case 1:
      ++npc->act_wait;

      if (m_rand(0, 50) == 1) {
        npc->act = 2;
        npc->act_wait = 0;
        npc->anim = 0;
        npc->anim_wait = 0;
        break;
      }

      break;

    case 2:
      ++npc->act_wait;

      if (++npc->anim_wait > 2) {
        npc->anim_wait = 0;
        ++npc->anim;
      }

      if (npc->anim > 1) npc->anim = 0;

      if (npc->act_wait > 18) {
        npc->act = 1;
        npc->act = 1;  // Duplicate line
      }

      break;

    case 3:
      if (++npc->act_wait > 40) npc->bits &= ~NPC_IGNORE_SOLIDITY;

      if (npc->flags & 8) {
        npc->act = 0;
        npc->anim = 0;
        npc->act_wait = 0;
      }

      break;

    case 10:
      npc->act = 11;
      // Fallthrough
    case 11:
      if (npc->flags & 1 && npc->xvel < 0) {
        npc->xvel *= -1;
        npc->dir = 2;
      }

      if (npc->flags & 4 && npc->xvel > 0) {
        npc->xvel *= -1;
        npc->dir = 0;
      }

      if (npc->flags & 8) {
        npc->act = 0;
        npc->anim = 0;
        npc->act_wait = 0;
      }

      break;
  }

  do_jump = FALSE;

  if (npc->act < 10 && npc->act != 3 && npc->act_wait > 10) {
    if (npc->shock) do_jump = TRUE;

    if (npc->x < player.x - (160 * 0x200) || npc->x > player.x + (160 * 0x200) || npc->y < player.y - (64 * 0x200) ||
        npc->y > player.y + (64 * 0x200)) {
      // This blank space is needed for the function to produce the same assembly.
      // Chances are there used to be some commented-out code here.
    } else {
      if (m_rand(0, 50) == 2) do_jump = TRUE;
    }
  }

  if (do_jump) {
    if (npc->x < player.x)
      npc->dir = 2;
    else
      npc->dir = 0;

    npc->act = 10;
    npc->anim = 2;
    npc->yvel = -0x2FF;
    snd_play_sound(PRIO_NORMAL, 6, FALSE);

    if (npc->dir == 0)
      npc->xvel = -0x100;
    else
      npc->xvel = 0x100;
  }

  npc->yvel += 0x80;
  if (npc->yvel > 0x5FF) npc->yvel = 0x5FF;

  npc->x += npc->xvel;
  npc->y += npc->yvel;

  if (npc->dir == 0)
    npc->rect = &rc_left[npc->anim];
  else
    npc->rect = &rc_right[npc->anim];
}

// Quote (teleport out)
void npc_act_111(npc_t *npc) {
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
      npc->anim = 0;
      npc->y -= 16 * 0x200;
      break;

    case 1:
      if (++npc->act_wait > 20) {
        npc->act_wait = 0;
        npc->act = 2;
        npc->anim = 1;
        npc->yvel = -0x2FF;
      }

      break;

    case 2:
      if (npc->yvel > 0) npc->hit.bottom = 16 * 0x200;

      if (npc->flags & 8) {
        npc->act = 3;
        npc->act_wait = 0;
        npc->anim = 0;
      }

      break;

    case 3:
      if (++npc->act_wait > 40) {
        npc->act = 4;
        npc->act_wait = 64;
        snd_play_sound(PRIO_NORMAL, 29, FALSE);
      }

      break;

    case 4:
      --npc->act_wait;
      npc->anim = 0;

      if (npc->act_wait == 0) npc->cond = 0;

      break;
  }

  npc->yvel += 0x40;
  npc->y += npc->yvel;

  if (npc->dir == 0)
    npc->rect = &rc_left[npc->anim];
  else
    npc->rect = &rc_right[npc->anim];

  // Use a different sprite if the player is wearing the Mimiga Mask
  if (player.equip & EQUIP_MIMIGA_MASK) {
    npc->rect_delta.top = 32;
    npc->rect_delta.bottom = 32;
  }

  if (npc->act == 4) {
    npc->rect_delta.bottom += npc->rect->top + (npc->act_wait / 4) - npc->rect->bottom;
    if (npc->act_wait / 2 % 2) npc->rect_delta.left = 1;
  }

  // force rect update
  npc->rect_prev = NULL;
}

// Quote (teleport in)
void npc_act_112(npc_t *npc) {
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
      npc->anim = 0;
      npc->anim_wait = 0;
      npc->x += 16 * 0x200;
      npc->y += 8 * 0x200;
      snd_play_sound(PRIO_NORMAL, 29, FALSE);
      // Fallthrough
    case 1:
      if (++npc->act_wait == 64) {
        npc->act = 2;
        npc->act_wait = 0;
      }

      break;

    case 2:
      if (++npc->act_wait > 20) {
        npc->act = 3;
        npc->anim = 1;
        npc->hit.bottom = 8 * 0x200;
      }

      break;

    case 3:
      if (npc->flags & 8) {
        npc->act = 4;
        npc->act_wait = 0;
        npc->anim = 0;
      }

      break;
  }

  npc->yvel += 0x40;
  npc->y += npc->yvel;

  if (npc->dir == 0)
    npc->rect = &rc_left[npc->anim];
  else
    npc->rect = &rc_right[npc->anim];

  // Use a different sprite if the player is wearing the Mimiga Mask
  if (player.equip & EQUIP_MIMIGA_MASK) {
    npc->rect_delta.top = 32;
    npc->rect_delta.bottom = 32;
  }

  if (npc->act == 1) {
    npc->rect_delta.bottom += npc->rect->top + (npc->act_wait / 4) - npc->rect->bottom;
    if (npc->act_wait / 2 % 2) npc->rect_delta.left = 1;
  }

  // force rect update
  npc->rect_prev = NULL;
}

// Professor Booster
void npc_act_113(npc_t *npc) {
  static const rect_t rc_left[7] = {
    {224, 0, 240, 16}, {240, 0, 256, 16}, {256, 0, 272, 16}, {224, 0, 240, 16},
    {272, 0, 288, 16}, {224, 0, 240, 16}, {288, 0, 304, 16},
  };

  static const rect_t rc_right[7] = {
    {224, 16, 240, 32}, {240, 16, 256, 32}, {256, 16, 272, 32}, {224, 16, 240, 32},
    {272, 16, 288, 32}, {224, 16, 240, 32}, {288, 16, 304, 32},
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
        npc->x -= 1 * 0x200;
      else
        npc->x += 1 * 0x200;

      break;

    case 5:
      npc->anim = 6;
      break;

    case 30:
      npc->act = 31;
      npc->anim = 0;
      npc->anim_wait = 0;
      npc->hit.bottom = 16 * 0x200;
      npc->x -= 16 * 0x200;
      npc->y += 8 * 0x200;
      snd_play_sound(PRIO_NORMAL, 29, FALSE);
      // Fallthrough
    case 31:
      if (++npc->act_wait == 64) {
        npc->act = 32;
        npc->act_wait = 0;
      }

      break;

    case 32:
      if (++npc->act_wait > 20) {
        npc->act = 33;
        npc->anim = 1;
        npc->hit.bottom = 8 * 0x200;
      }

      break;

    case 33:
      if (npc->flags & 8) {
        npc->act = 34;
        npc->act_wait = 0;
        npc->anim = 0;
      }

      break;
  }

  npc->yvel += 0x40;
  npc->y += npc->yvel;

  if (npc->dir == 0)
    npc->rect = &rc_left[npc->anim];
  else
    npc->rect = &rc_right[npc->anim];

  if (npc->act == 31) {
    npc->rect_delta.bottom = npc->rect->top + (npc->act_wait / 4) - npc->rect->bottom;
    if (npc->act_wait / 2 % 2) npc->rect_delta.left = 1;
    // force rect update
    npc->rect_prev = NULL;
  }
}

// Press
void npc_act_114(npc_t *npc) {
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
      if (npc->flags & 8) {
        // Chances are there used to be commented-out code here
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
          for (i = 0; i < 4; ++i)
            npc_spawn(4, npc->x, npc->y, m_rand(-341, 341), m_rand(-0x600, 0), 0, NULL, 0x100);
          snd_play_sound(PRIO_NORMAL, 26, FALSE);
          cam_start_quake_small(10);
        }

        npc->act = 1;
        npc->anim = 0;
        npc->damage = 0;
        npc->bits |= NPC_SOLID_HARD;
      }

      break;
  }

  npc->yvel += 0x20;
  if (npc->yvel > 0x5FF) npc->yvel = 0x5FF;

  npc->y += npc->yvel;

  npc->rect = &rc_left[npc->anim];
}

// Ravil
void npc_act_115(npc_t *npc) {
  int i;

  static const rect_t rc_left[6] = {
    {0, 120, 24, 144},  {24, 120, 48, 144},  {48, 120, 72, 144},
    {72, 120, 96, 144}, {96, 120, 120, 144}, {120, 120, 144, 144},
  };

  static const rect_t rc_right[6] = {
    {0, 144, 24, 168},  {24, 144, 48, 168},  {48, 144, 72, 168},
    {72, 144, 96, 168}, {96, 144, 120, 168}, {120, 144, 144, 168},
  };

  switch (npc->act) {
    case 0:
      npc->xvel = 0;
      npc->act = 1;
      npc->act_wait = 0;
      npc->count1 = 0;
      // Fallthrough
    case 1:
      if (player.x < npc->x + (96 * 0x200) && player.x > npc->x - (96 * 0x200) && player.y < npc->y + (32 * 0x200) &&
          player.y > npc->y - (96 * 0x200))
        npc->act = 10;

      if (npc->shock) npc->act = 10;

      break;

    case 10:
      if (player.x < npc->x)
        npc->dir = 0;
      else
        npc->dir = 2;

      npc->anim = 1;

      if (++npc->act_wait > 20) {
        npc->act_wait = 0;
        npc->act = 20;
      }

      break;

    case 20:
      npc->damage = 0;
      npc->xvel = 0;

      if (++npc->anim_wait > 2) {
        npc->anim_wait = 0;
        ++npc->anim;
      }

      if (npc->anim > 2) {
        if (npc->x > player.x)
          npc->dir = 0;
        else
          npc->dir = 2;

        if (npc->dir == 0)
          npc->xvel = -0x200;
        else
          npc->xvel = 0x200;

        if (++npc->count1 > 2) {
          npc->count1 = 0;
          npc->anim = 4;
          npc->act = 21;
          npc->yvel = -0x400;
          npc->xvel *= 2;
          npc->damage = 5;
          snd_play_sound(PRIO_NORMAL, 102, FALSE);
        } else {
          npc->act = 21;
          npc->yvel = -0x400;
          snd_play_sound(PRIO_NORMAL, 30, FALSE);
        }
      }

      break;

    case 21:
      if (npc->flags & 8) {
        snd_play_sound(PRIO_NORMAL, 23, FALSE);
        npc->act = 20;
        npc->anim = 1;
        npc->anim_wait = 0;
        npc->damage = 0;

        if (player.x > npc->x + (144 * 0x200) || player.x < npc->x - (144 * 0x200) ||
            player.y > npc->y + (48 * 0x200) || player.y < npc->y - (144 * 0x200))
          npc->act = 0;
      }

      break;

    case 30:
      for (i = 0; i < 8; ++i)
        npc_spawn(4, npc->x + (m_rand(-12, 12) * 0x200), npc->y + (m_rand(-12, 12) * 0x200), m_rand(-341, 341),
                  m_rand(-0x600, 0), 0, NULL, 0x100);

      npc->anim = 0;
      npc->act = 0;

      break;

    case 50:
      npc->act = 51;
      npc->anim = 4;
      npc->damage = 0;
      npc->yvel = -0x200;
      npc->bits &= ~(NPC_SOLID_SOFT | NPC_SHOOTABLE);
      snd_play_sound(PRIO_NORMAL, 51, FALSE);
      // Fallthrough
    case 51:
      if (npc->flags & 8) {
        snd_play_sound(PRIO_NORMAL, 23, FALSE);
        npc->act = 52;
        npc->anim = 5;
        npc->xvel = 0;
      }

      break;
  }

  if (npc->act > 50)
    npc->yvel += 0x20;
  else
    npc->yvel += 0x40;

  if (npc->yvel > 0x5FF) npc->yvel = 0x5FF;

  npc->x += npc->xvel;
  npc->y += npc->yvel;

  if (npc->dir == 0)
    npc->rect = &rc_left[npc->anim];
  else
    npc->rect = &rc_right[npc->anim];
}

// Red petals
void npc_act_116(npc_t *npc) {
  static const rect_t rc = {272, 184, 320, 200};
  npc->rect = &rc;
}

// Curly
void npc_act_117(npc_t *npc) {
  static const rect_t rc_left[10] = {
    {0, 96, 16, 112},    {16, 96, 32, 112},   {0, 96, 16, 112},    {32, 96, 48, 112},   {0, 96, 16, 112},
    {176, 96, 192, 112}, {112, 96, 128, 112}, {160, 96, 176, 112}, {144, 96, 160, 112}, {48, 96, 64, 112},
  };

  static const rect_t rc_right[10] = {
    {0, 112, 16, 128},    {16, 112, 32, 128},   {0, 112, 16, 128},    {32, 112, 48, 128},   {0, 112, 16, 128},
    {176, 112, 192, 128}, {112, 112, 128, 128}, {160, 112, 176, 128}, {144, 112, 160, 128}, {48, 112, 64, 128},
  };

  switch (npc->act) {
    case 0:
      if (npc->dir == 4) {
        if (npc->x > player.x)
          npc->dir = 0;
        else
          npc->dir = 2;
      }

      npc->act = 1;
      npc->anim = 0;
      npc->anim_wait = 0;
      // Fallthrough
    case 1:
      npc->xvel = 0;
      npc->yvel += 0x40;
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

      npc->yvel += 0x40;

      if (npc->dir == 0)
        npc->xvel = -0x200;
      else
        npc->xvel = 0x200;

      break;

    case 5:
      npc->act = 6;
      npc->anim = 5;
      npc_spawn_death_fx(npc->x, npc->y, npc->view.back, 8, 0);
      break;

    case 6:
      npc->anim = 5;
      break;

    case 10:
      npc->act = 11;
      npc->anim = 1;
      npc->anim_wait = 0;

      if (npc->x > player.x)
        npc->dir = 0;
      else
        npc->dir = 2;

      // Fallthrough
    case 11:
      if (++npc->anim_wait > 4) {
        npc->anim_wait = 0;
        ++npc->anim;
      }

      if (npc->anim > 4) npc->anim = 1;

      if (npc->dir == 0)
        npc->x -= 1 * 0x200;
      else
        npc->x += 1 * 0x200;

      if (player.x < npc->x + (20 * 0x200) && player.x > npc->x - (20 * 0x200)) npc->act = 0;

      break;

    case 20:
      npc->xvel = 0;
      npc->anim = 6;
      break;

    case 21:
      npc->xvel = 0;
      npc->anim = 9;
      break;

    case 30:
      npc->act = 31;
      npc->act_wait = 0;
      npc->yvel = -0x400;
      // Fallthrough
    case 31:
      npc->anim = 7;

      if (npc->dir == 0)
        npc->xvel = 0x200;
      else
        npc->xvel = -0x200;

      npc->yvel += 0x40;

      if (npc->act_wait++ != 0 && npc->flags & 8) npc->act = 32;

      break;

    case 32:
      npc->yvel += 0x40;
      npc->anim = 8;
      npc->xvel = 0;
      break;

    case 70:
      npc->act = 71;
      npc->act_wait = 0;
      npc->anim = 1;
      npc->anim_wait = 0;
      // Fallthrough
    case 71:
      if (npc->dir == 0)
        npc->x += 0x100;
      else
        npc->x -= 0x100;

      if (++npc->anim_wait > 8) {
        npc->anim_wait = 0;
        ++npc->anim;
      }

      if (npc->anim > 4) npc->anim = 1;

      break;
  }

  if (npc->yvel > 0x5FF) npc->yvel = 0x5FF;

  npc->x += npc->xvel;
  npc->y += npc->yvel;

  if (npc->dir == 0)
    npc->rect = &rc_left[npc->anim];
  else
    npc->rect = &rc_right[npc->anim];
}

// Curly (boss)
void npc_act_118(npc_t *npc) {
  bool bUpper;

  static const rect_t rc_left[9] = {
    {0, 32, 32, 56}, {32, 32, 64, 56},   {64, 32, 96, 56}, {96, 32, 128, 56},
    {0, 32, 32, 56}, {128, 32, 160, 56}, {0, 32, 32, 56},  {0, 32, 32, 56},
    {160, 32, 192, 56},
  };

  static const rect_t rc_right[9] = {
    {0, 56, 32, 80}, {32, 56, 64, 80},   {64, 56, 96, 80}, {96, 56, 128, 80},
    {0, 56, 32, 80}, {128, 56, 160, 80}, {0, 56, 32, 80},  {0, 56, 32, 80},
    {160, 56, 192, 80},
  };

  bUpper = FALSE;

  if (npc->dir == 0 && npc->x < player.x) bUpper = TRUE;
  if (npc->dir == 2 && npc->x > player.x) bUpper = TRUE;

  switch (npc->act) {
    case 0:
      npc->act = 1;
      npc->anim = 0;
      npc->anim_wait = 0;
      break;

    case 10:
      npc->act = 11;
      npc->act_wait = m_rand(50, 100);
      npc->anim = 0;

      if (npc->x > player.x)
        npc->dir = 0;
      else
        npc->dir = 2;

      npc->bits |= NPC_SHOOTABLE;
      npc->bits &= ~NPC_INVULNERABLE;
      // Fallthrough
    case 11:
      if (npc->act_wait != 0)
        --npc->act_wait;
      else
        npc->act = 13;

      break;

    case 13:
      npc->act = 14;
      npc->anim = 3;
      npc->act_wait = m_rand(50, 100);

      if (npc->x > player.x)
        npc->dir = 0;
      else
        npc->dir = 2;
      // Fallthrough
    case 14:
      if (++npc->anim_wait > 2) {
        npc->anim_wait = 0;
        ++npc->anim;
      }

      if (npc->anim > 6) npc->anim = 3;

      if (npc->dir == 0)
        npc->xvel -= 0x40;
      else
        npc->xvel += 0x40;

      if (npc->act_wait != 0) {
        --npc->act_wait;
      } else {
        npc->bits |= NPC_SHOOTABLE;
        npc->act = 20;
        npc->act_wait = 0;
        snd_play_sound(PRIO_NORMAL, 103, FALSE);
      }

      break;

    case 20:
      if (npc->x > player.x)
        npc->dir = 0;
      else
        npc->dir = 2;

      npc->xvel = (npc->xvel * 8) / 9;

      if (++npc->anim > 1) npc->anim = 0;

      if (++npc->act_wait > 50) {
        npc->act = 21;
        npc->act_wait = 0;
      }

      break;

    case 21:
      if (++npc->act_wait % 4 == 1) {
        if (npc->dir == 0) {
          if (bUpper) {
            npc->anim = 2;
            npc_spawn(123, npc->x, npc->y - (8 * 0x200), 0, 0, 1, NULL, 0x100);
          } else {
            npc->anim = 0;
            npc_spawn(123, npc->x - (8 * 0x200), npc->y + (4 * 0x200), 0, 0, 0, NULL, 0x100);
            npc->x += 1 * 0x200;
          }
        } else {
          if (bUpper) {
            npc->anim = 2;
            npc_spawn(123, npc->x, npc->y - (8 * 0x200), 0, 0, 1, NULL, 0x100);
          } else {
            npc->anim = 0;
            npc_spawn(123, npc->x + (8 * 0x200), npc->y + (4 * 0x200), 0, 0, 2, NULL, 0x100);
            npc->x -= 1 * 0x200;
          }
        }
      }

      if (npc->act_wait > 30) npc->act = 10;

      break;

    case 30:
      if (++npc->anim > 8) npc->anim = 7;

      if (++npc->act_wait > 30) {
        npc->act = 10;
        npc->anim = 0;
      }

      break;
  }

  if (npc->act > 10 && npc->act < 30 && bullet_count_by_arm(6)) {
    npc->act_wait = 0;
    npc->act = 30;
    npc->anim = 7;
    npc->bits &= ~NPC_SHOOTABLE;
    npc->bits |= NPC_INVULNERABLE;
    npc->xvel = 0;
  }

  npc->yvel += 0x20;

  if (npc->xvel > 0x1FF) npc->xvel = 0x1FF;
  if (npc->xvel < -0x1FF) npc->xvel = -0x1FF;

  if (npc->yvel > 0x5FF) npc->yvel = 0x5FF;

  npc->y += npc->yvel;
  npc->x += npc->xvel;

  if (npc->dir == 0)
    npc->rect = &rc_left[npc->anim];
  else
    npc->rect = &rc_right[npc->anim];
}

// Table and chair
void npc_act_119(npc_t *npc) {
  static const rect_t rc = {248, 184, 272, 200};
  npc->rect = &rc;
}
