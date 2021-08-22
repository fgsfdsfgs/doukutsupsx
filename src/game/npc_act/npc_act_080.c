#include "game/npc_act/npc_act.h"

// Gravekeeper
void npc_act_080(npc_t *npc) {
  static const rect_t rc_left[7] = {
    {0, 64, 24, 88},  {24, 64, 48, 88},  {0, 64, 24, 88},    {48, 64, 72, 88},
    {72, 64, 96, 88}, {96, 64, 120, 88}, {120, 64, 144, 88},
  };

  static const rect_t rc_right[7] = {
    {0, 88, 24, 112},  {24, 88, 48, 112},  {0, 88, 24, 112},    {48, 88, 72, 112},
    {72, 88, 96, 112}, {96, 88, 120, 112}, {120, 88, 144, 112},
  };

  switch (npc->act) {
    case 0:
      npc->bits &= ~NPC_SHOOTABLE;
      npc->act = 1;
      npc->damage = 0;
      npc->hit.front = 4 * 0x200;
      // Fallthrough
    case 1:
      npc->anim = 0;

      if (npc->x - (128 * 0x200) < player.x && npc->x + (128 * 0x200) > player.x && npc->y - (48 * 0x200) < player.y &&
          npc->y + (32 * 0x200) > player.y) {
        npc->anim_wait = 0;
        npc->act = 2;
      }

      if (npc->shock) {
        npc->anim = 1;
        npc->anim_wait = 0;
        npc->act = 2;
        npc->bits &= ~NPC_SHOOTABLE;
      }

      if (player.x < npc->x)
        npc->dir = 0;
      else
        npc->dir = 2;

      break;

    case 2:
      if (++npc->anim_wait > 6) {
        npc->anim_wait = 0;
        ++npc->anim;
      }

      if (npc->anim > 3) npc->anim = 0;

      if (npc->x - (16 * 0x200) < player.x && npc->x + (16 * 0x200) > player.x) {
        npc->hit.front = (18 * 0x200);
        npc->act_wait = 0;
        npc->act = 3;
        npc->bits |= NPC_SHOOTABLE;
        snd_play_sound(PRIO_NORMAL, 34, FALSE);

        if (npc->dir == 0)
          npc->xvel = -0x400;
        else
          npc->xvel = 0x400;
      }

      if (player.x < npc->x) {
        npc->dir = 0;
        npc->xvel = -0x100;
      } else {
        npc->dir = 2;
        npc->xvel = 0x100;
      }

      break;

    case 3:
      npc->xvel = 0;

      if (++npc->act_wait > 40) {
        npc->act_wait = 0;
        npc->act = 4;
        snd_play_sound(PRIO_NORMAL, 106, FALSE);
      }

      npc->anim = 4;
      break;

    case 4:
      npc->damage = 10;

      if (++npc->act_wait > 2) {
        npc->act_wait = 0;
        npc->act = 5;
      }

      npc->anim = 5;
      break;

    case 5:
      npc->anim = 6;

      if (++npc->act_wait > 60) npc->act = 0;

      break;
  }

  if (npc->xvel < 0 && npc->flags & 1) npc->xvel = 0;
  if (npc->xvel > 0 && npc->flags & 4) npc->xvel = 0;

  npc->yvel += 0x20;

  if (npc->xvel > 0x400) npc->xvel = 0x400;
  if (npc->xvel < -0x400) npc->xvel = -0x400;
  if (npc->yvel > 0x5FF) npc->yvel = 0x5FF;
  if (npc->yvel < -0x5FF) npc->yvel = -0x5FF;

  npc->x += npc->xvel;
  npc->y += npc->yvel;

  if (npc->dir == 0)
    npc->rect = &rc_left[npc->anim];
  else
    npc->rect = &rc_right[npc->anim];
}

// Giant pignon
void npc_act_081(npc_t *npc) {
  static const rect_t rc_left[6] = {
    {144, 64, 168, 88}, {168, 64, 192, 88}, {192, 64, 216, 88},
    {216, 64, 240, 88}, {144, 64, 168, 88}, {240, 64, 264, 88},
  };

  static const rect_t rc_right[6] = {
    {144, 88, 168, 112}, {168, 88, 192, 112}, {192, 88, 216, 112},
    {216, 88, 240, 112}, {144, 88, 168, 112}, {240, 88, 264, 112},
  };

  switch (npc->act) {
    case 0:
      npc->act = 1;
      npc->anim = 0;
      npc->anim_wait = 0;
      npc->xvel = 0;
      // Fallthrough
    case 1:
      if (m_rand(0, 100) == 1) {
        npc->act = 2;
        npc->act_wait = 0;
        npc->anim = 1;
        break;
      }

      if (m_rand(0, 150) == 1) {
        if (npc->dir == 0)
          npc->dir = 2;
        else
          npc->dir = 0;
      }

      if (m_rand(0, 150) == 1) {
        npc->act = 3;
        npc->act_wait = 50;
        npc->anim = 0;
        break;
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
      if (--npc->act_wait == 0) npc->act = 0;

      if (++npc->anim_wait > 2) {
        npc->anim_wait = 0;
        ++npc->anim;
      }

      if (npc->anim > 4) npc->anim = 2;

      if (npc->flags & 1) {
        npc->dir = 2;
        npc->xvel = 0x200;
      }

      if (npc->flags & 4) {
        npc->dir = 0;
        npc->xvel = -0x200;
      }

      if (npc->dir == 0)
        npc->xvel = -0x100;
      else
        npc->xvel = 0x100;

      break;

    case 5:
      if (npc->flags & 8) npc->act = 0;

      break;
  }

  switch (npc->act) {
    case 1:
    case 2:
    case 4:
      if (npc->shock) {
        npc->yvel = -0x200;
        npc->anim = 5;
        npc->act = 5;

        if (npc->x < player.x)
          npc->xvel = 0x100;
        else
          npc->xvel = -0x100;
      }

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

// Misery (standing)
void npc_act_082(npc_t *npc) {
  switch (npc->act) {
    case 0:
      npc->act = 1;
      npc->anim = 2;
      // Fallthrough
    case 1:
      if (m_rand(0, 120) == 10) {
        npc->act = 2;
        npc->act_wait = 0;
        npc->anim = 3;
      }

      break;

    case 2:
      if (++npc->act_wait > 8) {
        npc->act = 1;
        npc->anim = 2;
      }

      break;

    case 15:
      npc->act = 16;
      npc->act_wait = 0;
      npc->anim = 4;
      // Fallthrough
    case 16:
      if (++npc->act_wait == 30) {
        snd_play_sound(PRIO_NORMAL, 21, FALSE);
        npc_spawn(66, npc->x, npc->y - (16 * 0x200), 0, 0, 0, npc, 0);
      }

      if (npc->act_wait == 50) npc->act = 14;

      break;

    case 20:
      npc->act = 21;
      npc->anim = 0;
      npc->yvel = 0;
      npc->bits |= NPC_IGNORE_SOLIDITY;
      // Fallthrough
    case 21:
      npc->yvel -= 0x20;

      if (npc->y < -8 * 0x200) npc->cond = 0;

      break;

    case 25:
      npc->act = 26;
      npc->act_wait = 0;
      npc->anim = 5;
      npc->anim_wait = 0;
      // Fallthrough
    case 26:
      if (++npc->anim > 7) npc->anim = 5;

      if (++npc->act_wait == 30) {
        snd_play_sound(PRIO_NORMAL, 101, FALSE);
        cam_start_flash(0, 0, FLASH_MODE_FLASH);
        npc->act = 27;
        npc->anim = 7;
      }

      break;

    case 27:
      if (++npc->act_wait == 50) {
        npc->act = 0;
        npc->anim = 0;
      }

      break;

    case 30:
      npc->act = 31;
      npc->anim = 3;
      npc->anim_wait = 0;
      // Fallthrough
    case 31:
      if (++npc->anim_wait > 10) {
        npc->act = 32;
        npc->anim = 4;
        npc->anim_wait = 0;
      }

      break;

    case 32:
      if (++npc->anim_wait > 100) {
        npc->act = 1;
        npc->anim = 2;
      }

      break;

    case 40:
      npc->act = 41;
      npc->act_wait = 0;
      // Fallthrough
    case 41:
      npc->anim = 4;

      switch (++npc->act_wait) {
        case 30:
        case 40:
        case 50:
          npc_spawn(11, npc->x + (8 * 0x200), npc->y - (8 * 0x200), 0x600, m_rand(-0x200, 0), 0, NULL, 0x100);
          snd_play_sound(PRIO_NORMAL, 33, FALSE);
          break;
      }

      if (npc->act_wait > 50) npc->act = 0;

      break;

    case 50:
      npc->anim = 8;
      break;
  }

  npc->x += npc->xvel;
  npc->y += npc->yvel;

  static const rect_t rc_left[9] = {
    {80, 0, 96, 16},   {96, 0, 112, 16},  {112, 0, 128, 16}, {128, 0, 144, 16},  {144, 0, 160, 16},
    {160, 0, 176, 16}, {176, 0, 192, 16}, {144, 0, 160, 16}, {208, 64, 224, 80},
  };

  static const rect_t rc_right[9] = {
    {80, 16, 96, 32},   {96, 16, 112, 32},  {112, 16, 128, 32}, {128, 16, 144, 32}, {144, 16, 160, 32},
    {160, 16, 176, 32}, {176, 16, 192, 32}, {144, 16, 160, 32}, {208, 80, 224, 96},
  };

  if (npc->act == 11) {
    if (npc->anim_wait != 0) {
      --npc->anim_wait;
      npc->anim = 1;
    } else {
      if (m_rand(0, 100) == 1) npc->anim_wait = 30;

      npc->anim = 0;
    }
  }

  if (npc->act == 14) {
    if (npc->anim_wait != 0) {
      --npc->anim_wait;
      npc->anim = 3;
    } else {
      if (m_rand(0, 100) == 1) npc->anim_wait = 30;

      npc->anim = 2;
    }
  }

  if (npc->dir == 0)
    npc->rect = &rc_left[npc->anim];
  else
    npc->rect = &rc_right[npc->anim];
}

// Igor (cutscene)
void npc_act_083(npc_t *npc) {
  static const rect_t rc_left[8] = {
    {0, 0, 40, 40},    {40, 0, 80, 40}, {80, 0, 120, 40},  {0, 0, 40, 40},
    {120, 0, 160, 40}, {0, 0, 40, 40},  {160, 0, 200, 40}, {200, 0, 240, 40},
  };

  static const rect_t rc_right[8] = {
    {0, 40, 40, 80},    {40, 40, 80, 80}, {80, 40, 120, 80},  {0, 40, 40, 80},
    {120, 40, 160, 80}, {0, 40, 40, 80},  {160, 40, 200, 80}, {200, 40, 240, 80},
  };

  switch (npc->act) {
    case 0:
      npc->xvel = 0;
      npc->act = 1;
      npc->anim = 0;
      npc->anim_wait = 0;
      // Fallthrough
    case 1:
      if (++npc->anim_wait > 5) {
        npc->anim_wait = 0;
        ++npc->anim;
      }

      if (npc->anim > 1) npc->anim = 0;

      break;

    case 2:
      npc->act = 3;
      npc->anim = 2;
      npc->anim_wait = 0;
      // Fallthrough
    case 3:
      if (++npc->anim_wait > 3) {
        npc->anim_wait = 0;
        ++npc->anim;
      }

      if (npc->anim > 5) npc->anim = 2;

      if (npc->dir == 0)
        npc->xvel = -0x200;
      else
        npc->xvel = 0x200;

      break;

    case 4:
      npc->xvel = 0;
      npc->act = 5;
      npc->act_wait = 0;
      npc->anim = 6;
      // Fallthrough
    case 5:
      if (++npc->act_wait > 10) {
        npc->act_wait = 0;
        npc->act = 6;
        npc->anim = 7;
        snd_play_sound(PRIO_NORMAL, 70, FALSE);
      }

      break;

    case 6:
      if (++npc->act_wait > 8) {
        npc->act = 0;
        npc->anim = 0;
      }

      break;

    case 7:
      npc->act = 1;
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

// Basu projectile (Egg Corridor)
void npc_act_084(npc_t *npc) {
  if (npc->flags & 0xFF) {
    caret_spawn(npc->x, npc->y, CARET_PROJECTILE_DISSIPATION, DIR_LEFT);
    npc->cond = 0;
  }

  npc->y += npc->yvel;
  npc->x += npc->xvel;

  static const rect_t rect_left[4] = {
    {48, 48, 64, 64},
    {64, 48, 80, 64},
    {48, 64, 64, 80},
    {64, 64, 80, 80},
  };

  if (++npc->anim_wait > 2) {
    npc->anim_wait = 0;
    ++npc->anim;
  }

  if (npc->anim > 3) npc->anim = 0;

  npc->rect = &rect_left[npc->anim];

  if (++npc->count1 > 300) {
    caret_spawn(npc->x, npc->y, CARET_PROJECTILE_DISSIPATION, DIR_LEFT);
    npc->cond = 0;
  }
}

// Terminal
void npc_act_085(npc_t *npc) {
  static const rect_t rc_left[3] = {
    {256, 96, 272, 120},
    {256, 96, 272, 120},
    {272, 96, 288, 120},
  };

  static const rect_t rc_right[3] = {
    {256, 96, 272, 120},
    {288, 96, 304, 120},
    {304, 96, 320, 120},
  };

  switch (npc->act) {
    case 0:
      npc->anim = 0;

      if (npc->x - (8 * 0x200) < player.x && npc->x + (8 * 0x200) > player.x && npc->y - (16 * 0x200) < player.y &&
          npc->y + (8 * 0x200) > player.y) {
        snd_play_sound(PRIO_NORMAL, 43, FALSE);
        npc->act = 1;
      }

      break;

    case 1:
      if (++npc->anim > 2) npc->anim = 1;

      break;
  }

  if (npc->dir == 0)
    npc->rect = &rc_left[npc->anim];
  else
    npc->rect = &rc_right[npc->anim];
}

// Missile
void npc_act_086(npc_t *npc) {
  static const rect_t rect1[2] = {
    {0, 80, 16, 96},
    {16, 80, 32, 96},
  };

  static const rect_t rect3[2] = {
    {0, 112, 16, 128},
    {16, 112, 32, 128},
  };

  static const rect_t rc_last = {16, 0, 32, 16};

  if (npc->dir == 0) {
    if (++npc->anim_wait > 2) {
      npc->anim_wait = 0;
      ++npc->anim;
    }

    if (npc->anim > 1) npc->anim = 0;
  }

  if (stage_data->bk_type == BACKGROUND_TYPE_AUTOSCROLL || stage_data->bk_type == BACKGROUND_TYPE_CLOUDS_WINDY) {
    if (npc->act == 0) {
      npc->act = 1;
      npc->yvel = m_rand(-0x20, 0x20);
      npc->xvel = m_rand(0x7F, 0x100);
    }

    npc->xvel -= 8;

    if (npc->x < 80 * 0x200) npc->cond = 0;

    if (npc->x < -3 * 0x200) npc->x = -3 * 0x200;

    if (npc->flags & 1) npc->xvel = 0x100;

    if (npc->flags & 2) npc->yvel = 0x40;

    if (npc->flags & 8) npc->yvel = -0x40;

    npc->x += npc->xvel;
    npc->y += npc->yvel;
  }

  switch (npc->exp) {
    case 1:
      npc->rect = &rect1[npc->anim];
      break;

    case 3:
      npc->rect = &rect3[npc->anim];
      break;
  }

  if (npc->dir == 0) ++npc->count1;

  if (npc->count1 > 550) npc->cond = 0;

  if (npc->count1 > 500 && npc->count1 / 2 % 2) npc->rect = NULL;

  if (npc->count1 > 547) npc->rect = &rc_last;
}

// Heart
void npc_act_087(npc_t *npc) {
  static const rect_t rect2[2] = {
    {32, 80, 48, 96},
    {48, 80, 64, 96},
  };

  static const rect_t rect6[2] = {
    {64, 80, 80, 96},
    {80, 80, 96, 96},
  };

  static const rect_t rc_last = {16, 0, 32, 16};

  if (npc->dir == 0) {
    if (++npc->anim_wait > 2) {
      npc->anim_wait = 0;
      ++npc->anim;
    }

    if (npc->anim > 1) npc->anim = 0;
  }

  if (stage_data->bk_type == BACKGROUND_TYPE_AUTOSCROLL || stage_data->bk_type == BACKGROUND_TYPE_CLOUDS_WINDY) {
    if (npc->act == 0) {
      npc->act = 1;
      npc->yvel = m_rand(-0x20, 0x20);
      npc->xvel = m_rand(0x7F, 0x100);
    }

    npc->xvel -= 8;

    if (npc->x < 80 * 0x200) npc->cond = 0;

    if (npc->x < -3 * 0x200) npc->x = -3 * 0x200;

    if (npc->flags & 1) npc->xvel = 0x100;

    if (npc->flags & 2) npc->yvel = 0x40;

    if (npc->flags & 8) npc->yvel = -0x40;

    npc->x += npc->xvel;
    npc->y += npc->yvel;
  }

  switch (npc->exp) {
    case 2:
      npc->rect = &rect2[npc->anim];
      break;

    case 6:
      npc->rect = &rect6[npc->anim];
      break;
  }

  if (npc->dir == 0) ++npc->count1;

  if (npc->count1 > 550) npc->cond = 0;

  if (npc->count1 > 500 && npc->count1 / 2 % 2) npc->rect = NULL;

  if (npc->count1 > 547) npc->rect = &rc_last;
}

// Igor (boss)
void npc_act_088(npc_t *npc) {
  int i;
  unsigned char deg;
  int xvel, yvel;

  static const rect_t rc_left[12] = {
    {0, 0, 40, 40},    {40, 0, 80, 40},   {80, 0, 120, 40},  {0, 0, 40, 40},
    {120, 0, 160, 40}, {0, 0, 40, 40},    {160, 0, 200, 40}, {200, 0, 240, 40},
    {0, 80, 40, 120},  {40, 80, 80, 120}, {240, 0, 280, 40}, {280, 0, 320, 40},
  };

  static const rect_t rc_right[12] = {
    {0, 40, 40, 80},     {40, 40, 80, 80},    {80, 40, 120, 80},  {0, 40, 40, 80},
    {120, 40, 160, 80},  {0, 40, 40, 80},     {160, 40, 200, 80}, {200, 40, 240, 80},
    {120, 80, 160, 120}, {160, 80, 200, 120}, {240, 40, 280, 80}, {280, 40, 320, 80},
  };

  switch (npc->act) {
    case 0:
      npc->xvel = 0;
      npc->act = 1;
      npc->anim = 0;
      npc->anim_wait = 0;
      // Fallthrough
    case 1:
      if (++npc->anim_wait > 5) {
        npc->anim_wait = 0;
        ++npc->anim;
      }

      if (npc->anim > 1) npc->anim = 0;

      if (++npc->act_wait > 50) npc->act = 2;

      break;

    case 2:
      npc->act = 3;
      npc->act_wait = 0;
      npc->anim = 2;
      npc->anim_wait = 0;

      if (++npc->count1 < 3 || npc->life > 150) {
        npc->count2 = 0;

        if (player.x < npc->x)
          npc->dir = 0;
        else
          npc->dir = 2;
      } else {
        npc->count2 = 1;

        if (player.x < npc->x)
          npc->dir = 2;
        else
          npc->dir = 0;
      }
      // Fallthrough
    case 3:
      ++npc->act_wait;

      if (++npc->anim_wait > 3) {
        npc->anim_wait = 0;
        ++npc->anim;
      }

      if (npc->anim > 5) npc->anim = 2;

      if (npc->dir == 0)
        npc->xvel = -0x200;
      else
        npc->xvel = 0x200;

      if (npc->count2) {
        if (npc->act_wait > 16) {
          npc->act = 9;
          npc->xvel = 0;
          npc->anim = 10;
          break;
        }
      } else if (npc->act_wait > 50) {
        npc->anim = 8;
        npc->yvel = -0x400;
        npc->act = 7;
        npc->act_wait = 0;
        npc->xvel = (npc->xvel * 3) / 2;
        npc->damage = 2;
        break;
      } else {
        if (npc->dir == 0) {
          if (npc->x - (24 * 0x200) < player.x) npc->act = 4;
        } else {
          if (npc->x + (24 * 0x200) > player.x) npc->act = 4;
        }
      }

      break;

    case 4:
      npc->xvel = 0;
      npc->act = 5;
      npc->act_wait = 0;
      npc->anim = 6;
      // Fallthrough
    case 5:
      if (++npc->act_wait > 12) {
        npc->act_wait = 0;
        npc->act = 6;
        npc->anim = 7;
        snd_play_sound(PRIO_NORMAL, 70, FALSE);
        npc->damage = 5;
        npc->hit.front = 24 * 0x200;
        npc->hit.top = 1;
      }

      break;

    case 6:
      if (++npc->act_wait > 10) {
        npc->act = 0;
        npc->anim = 0;
        npc->damage = 0;
        npc->hit.front = 8 * 0x200;
        npc->hit.top = 16 * 0x200;
      }

      break;

    case 7:
      if (npc->flags & 8) {
        npc->act = 8;
        npc->anim = 9;
        snd_play_sound(PRIO_NORMAL, 26, FALSE);
        cam_start_quake_small(30);
        npc->damage = 0;

        for (i = 0; i < 4; ++i)
          npc_spawn(4, npc->x + (m_rand(-12, 12) * 0x200), npc->y + (m_rand(-12, 12) * 0x200), m_rand(-341, 341),
                    m_rand(-0x600, 0), 0, NULL, 0x100);
      }

      break;

    case 8:
      npc->xvel = 0;

      if (++npc->act_wait > 10) {
        npc->act = 0;
        npc->anim = 0;
        npc->damage = 0;
      }

      break;

    case 9:
      npc->act = 10;
      npc->act_wait = 0;

      if (player.x < npc->x)
        npc->dir = 0;
      else
        npc->dir = 2;

      // Fallthrough
    case 10:
      if (++npc->act_wait > 100 && npc->act_wait % 6 == 1) {
        if (npc->dir == 0)
          deg = 0x88;
        else
          deg = 0xF8;

        deg += (unsigned char)m_rand(-0x10, 0x10);
        yvel = m_sin(deg) * 3;
        xvel = m_cos(deg) * 3;
        npc_spawn(11, npc->x, npc->y + (4 * 0x200), xvel, yvel, 0, NULL, 0x100);

        snd_play_sound(PRIO_NORMAL, 12, FALSE);
      }

      if (npc->act_wait > 50 && npc->act_wait / 2 % 2)
        npc->anim = 11;
      else
        npc->anim = 10;

      if (npc->act_wait > 132) {
        npc->act = 0;
        npc->anim = 0;
        npc->count1 = 0;
      }

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

// Igor (defeated)
void npc_act_089(npc_t *npc) {
  int i;

  static const rect_t rc_left[4] = {
    {80, 80, 120, 120},
    {240, 80, 264, 104},
    {264, 80, 288, 104},
    {288, 80, 312, 104},
  };

  static const rect_t rc_right[4] = {
    {200, 80, 240, 120},
    {240, 104, 264, 128},
    {264, 104, 288, 128},
    {288, 104, 312, 128},
  };

  switch (npc->act) {
    case 0:
      snd_play_sound(PRIO_NORMAL, 72, FALSE);

      if (npc->x > player.x)
        npc->dir = 0;
      else
        npc->dir = 2;

      for (i = 0; i < 8; ++i)
        npc_spawn(4, npc->x + (m_rand(-12, 12) * 0x200), npc->y + (m_rand(-12, 12) * 0x200), m_rand(-341, 341),
                  m_rand(-0x600, 0), 0, NULL, 0x100);

      npc->act = 1;
      // Fallthrough
    case 1:
      if (++npc->act_wait > 100) {
        npc->act_wait = 0;
        npc->act = 2;
      }

      if (npc->act_wait % 5 == 0)
        npc_spawn(4, npc->x + (m_rand(-12, 12) * 0x200), npc->y + (m_rand(-12, 12) * 0x200), m_rand(-341, 341),
                  m_rand(-0x600, 0), 0, NULL, 0x100);

      if (npc->dir == 0)
        npc->rect = &rc_left[0];
      else
        npc->rect = &rc_right[0];

      if (npc->act_wait / 2 % 2) {
        npc->rect_delta.left = -1;
        npc->rect_prev = NULL;  // force texrect update
      }

      break;

    case 2:
      if (++npc->act_wait / 2 % 2 && npc->act_wait < 100) {
        npc->anim = 0;
        npc->view.back = 20 * 0x200;
        npc->view.front = 20 * 0x200;
        npc->view.top = 20 * 0x200;
      } else {
        npc->anim = 1;
        npc->view.back = 12 * 0x200;
        npc->view.front = 12 * 0x200;
        npc->view.top = 8 * 0x200;
      }

      if (npc->act_wait > 150) {
        npc->act_wait = 0;
        npc->act = 3;
        npc->anim = 1;
      }

      if (npc->act_wait % 9 == 0)
        npc_spawn(4, npc->x + (m_rand(-12, 12) * 0x200), npc->y + (m_rand(-12, 12) * 0x200), m_rand(-341, 341),
                  m_rand(-0x600, 0), 0, NULL, 0x100);

      if (npc->dir == 0)
        npc->rect = &rc_left[npc->anim];
      else
        npc->rect = &rc_right[npc->anim];

      break;

    case 3:
      if (++npc->anim_wait > 50) {
        npc->anim_wait = 0;
        ++npc->anim;
      }

      if (npc->anim == 3) npc->act = 4;

      if (npc->dir == 0)
        npc->rect = &rc_left[npc->anim];
      else
        npc->rect = &rc_right[npc->anim];

      break;
  }

  npc->yvel += 0x40;
  if (npc->yvel > 0x5FF) npc->yvel = 0x5FF;

  npc->x += npc->xvel;
  npc->y += npc->yvel;
}

// Background
void npc_act_090(npc_t *npc) {
  static const rect_t rect = {280, 80, 296, 104};

  npc->rect = &rect;
}

// Cage
void npc_act_091(npc_t *npc) {
  static const rect_t rect = {96, 88, 128, 112};

  if (npc->act == 0) {
    ++npc->act;
    npc->y += 16 * 0x200;
  }

  npc->rect = &rect;
}

// Sue at PC
void npc_act_092(npc_t *npc) {
  static const rect_t rc_left[3] = {
    {272, 216, 288, 240},
    {288, 216, 304, 240},
    {304, 216, 320, 240},
  };

  switch (npc->act) {
    case 0:
      npc->x -= 4 * 0x200;
      npc->y += 16 * 0x200;
      npc->act = 1;
      npc->anim = 0;
      npc->anim_wait = 0;
      // Fallthrough
    case 1:
      if (++npc->anim_wait > 2) {
        npc->anim_wait = 0;
        ++npc->anim;
      }

      if (npc->anim > 1) npc->anim = 0;

      if (m_rand(0, 80) == 1) {
        npc->act = 2;
        npc->act_wait = 0;
        npc->anim = 1;
      }

      if (m_rand(0, 120) == 10) {
        npc->act = 3;
        npc->act_wait = 0;
        npc->anim = 2;
      }

      break;

    case 2:
      if (++npc->act_wait > 40) {
        npc->act = 3;
        npc->act_wait = 0;
        npc->anim = 2;
      }

      break;

    case 3:
      if (++npc->act_wait > 80) {
        npc->act = 1;
        npc->anim = 0;
      }

      break;
  }

  npc->rect = &rc_left[npc->anim];
}

// Chaco
void npc_act_093(npc_t *npc) {
  static const rect_t rc_left[7] = {
    {128, 0, 144, 16}, {144, 0, 160, 16}, {160, 0, 176, 16}, {128, 0, 144, 16},
    {176, 0, 192, 16}, {128, 0, 144, 16}, {32, 32, 48, 48},
  };

  static const rect_t rc_right[7] = {
    {128, 16, 144, 32}, {144, 16, 160, 32}, {160, 16, 176, 32}, {128, 16, 144, 32},
    {176, 16, 192, 32}, {128, 16, 144, 32}, {32, 32, 48, 48},
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
        ++npc->anim;
      }

      if (npc->anim > 5) npc->anim = 2;

      if (npc->dir == 0)
        npc->x -= 1 * 0x200;
      else
        npc->x += 1 * 0x200;

      break;

    case 10:
      npc->anim = 6;

      if (++npc->act_wait > 200) {
        npc->act_wait = 0;
        caret_spawn(npc->x, npc->y, CARET_ZZZ, DIR_LEFT);
      }

      break;
  }

  if (npc->dir == 0)
    npc->rect = &rc_left[npc->anim];
  else
    npc->rect = &rc_right[npc->anim];
}

// Kulala
void npc_act_094(npc_t *npc) {
  static const rect_t rect[5] = {
    {272, 0, 320, 24}, {272, 24, 320, 48}, {272, 48, 320, 72},
    {272, 72, 320, 96}, {272, 96, 320, 120},
  };

  switch (npc->act) {
    case 0:
      npc->anim = 4;

      if (npc->shock) {
        npc->anim = 0;
        npc->act = 10;
        npc->act_wait = 0;
      }

      break;

    case 10:
      npc->bits |= NPC_SHOOTABLE;
      npc->bits &= ~NPC_INVULNERABLE;

      if (++npc->act_wait > 40) {
        npc->act_wait = 0;
        npc->anim_wait = 0;
        npc->act = 11;
      }

      break;

    case 11:
      if (++npc->anim_wait > 5) {
        npc->anim_wait = 0;
        ++npc->anim;
      }

      if (npc->anim > 2) {
        npc->act = 12;
        npc->anim = 3;
      }

      break;

    case 12:
      npc->yvel = -0x155;

      if (++npc->act_wait > 20) {
        npc->act_wait = 0;
        npc->act = 10;
        npc->anim = 0;
      }

      break;

    case 20:
      npc->xvel /= 2;
      npc->yvel += 0x20;

      if (npc->shock == 0) {
        npc->act_wait = 30;
        npc->act = 10;
        npc->anim = 0;
      }

      break;
  }

  if (npc->shock) {
    if (++npc->count2 > 12) {
      npc->act = 20;
      npc->anim = 4;
      npc->bits &= ~NPC_SHOOTABLE;
      npc->bits |= NPC_INVULNERABLE;
    }
  } else {
    npc->count2 = 0;
  }

  if (npc->act >= 10) {
    if (npc->flags & 1) {
      npc->count1 = 50;
      npc->dir = 2;
    }

    if (npc->flags & 4) {
      npc->count1 = 50;
      npc->dir = 0;
    }

    if (npc->count1 != 0) {
      --npc->count1;

      if (npc->dir == 0)
        npc->xvel -= 0x80;
      else
        npc->xvel += 0x80;
    } else {
      npc->count1 = 50;

      if (npc->x > player.x)
        npc->dir = 0;
      else
        npc->dir = 2;
    }

    npc->yvel += 0x10;

    if (npc->flags & 8) npc->yvel = -0x400;
  }

  if (npc->xvel > 0x100) npc->xvel = 0x100;
  if (npc->xvel < -0x100) npc->xvel = -0x100;

  if (npc->yvel > 0x300) npc->yvel = 0x300;
  if (npc->yvel < -0x300) npc->yvel = -0x300;

  npc->x += npc->xvel;
  npc->y += npc->yvel;

  npc->rect = &rect[npc->anim];
}

// Jelly
void npc_act_095(npc_t *npc) {
  static const rect_t rc_left[4] = {
    {208, 64, 224, 80},
    {224, 64, 240, 80},
    {240, 64, 256, 80},
    {256, 64, 272, 80},
  };

  static const rect_t rc_right[4] = {
    {208, 80, 224, 96},
    {224, 80, 240, 96},
    {240, 80, 256, 96},
    {256, 80, 272, 96},
  };

  switch (npc->act) {
    case 0:
      npc->act = 1;
      npc->act_wait = m_rand(0, 50);
      npc->tgt_y = npc->y;
      npc->tgt_x = npc->x;

      if (npc->dir == 0)
        npc->xvel = 0x200;
      else
        npc->xvel = -0x200;
      // Fallthrough
    case 1:
      if (--npc->act_wait > 0) break;

      npc->act = 10;
      // Fallthrough
    case 10:
      if (++npc->act_wait > 10) {
        npc->act_wait = 0;
        npc->anim_wait = 0;
        npc->act = 11;
      }

      break;

    case 11:
      if (++npc->anim_wait > 5) {
        npc->anim_wait = 0;
        ++npc->anim;
      }

      if (npc->anim == 2) {
        if (npc->dir == 0)
          npc->xvel -= 0x100;
        else
          npc->xvel += 0x100;

        npc->yvel -= 0x200;
      }

      if (npc->anim > 2) {
        npc->act = 12;
        npc->anim = 3;
      }

      break;

    case 12:
      ++npc->act_wait;

      if (npc->y > npc->tgt_y && npc->act_wait > 10) {
        npc->act_wait = 0;
        npc->act = 10;
        npc->anim = 0;
      }

      break;
  }

  if (npc->x > npc->tgt_x)
    npc->dir = 0;
  else
    npc->dir = 2;

  if (npc->flags & 1) {
    npc->count1 = 50;
    npc->dir = 2;
  }

  if (npc->flags & 4) {
    npc->count1 = 50;
    npc->dir = 0;
  }

  npc->yvel += 0x20;

  if (npc->flags & 8) npc->yvel = -0x400;

  if (npc->xvel > 0x100) npc->xvel = 0x100;
  if (npc->xvel < -0x100) npc->xvel = -0x100;

  if (npc->yvel > 0x200) npc->yvel = 0x200;
  if (npc->yvel < -0x200) npc->yvel = -0x200;

  if (npc->shock) {
    npc->x += npc->xvel / 2;
    npc->y += npc->yvel / 2;
  } else {
    npc->x += npc->xvel;
    npc->y += npc->yvel;
  }

  if (npc->dir == 0)
    npc->rect = &rc_left[npc->anim];
  else
    npc->rect = &rc_right[npc->anim];
}

// Fan (left)
void npc_act_096(npc_t *npc) {
  static const rect_t rc[3] = {
    {272, 120, 288, 136},
    {288, 120, 304, 136},
    {304, 120, 320, 136},
  };

  switch (npc->act) {
    case 0:
      if (npc->dir == 2)
        npc->act = 2;
      else
        npc->anim = 1;

      // Fallthrough
    case 1:
      npc->anim = 0;
      break;

    case 2:
      if (++npc->anim_wait > 0) {
        npc->anim_wait = 0;
        ++npc->anim;
      }

      if (npc->anim > 2) npc->anim = 0;

      if (player.x > npc->x - (((VID_WIDTH / 2) + 160) * 0x200) &&
          player.x < npc->x + (((VID_WIDTH / 2) + 160) * 0x200) &&
          player.y > npc->y - (((VID_HEIGHT / 2) + 120) * 0x200) &&
          player.y < npc->y + (((VID_HEIGHT / 2) + 120) * 0x200)) {
        if (m_rand(0, 5) == 1) npc_spawn(199, npc->x, npc->y + (m_rand(-8, 8) * 0x200), 0, 0, 0, NULL, 0x100);
      }

      if (player.y < npc->y + (8 * 0x200) && player.y > npc->y - (8 * 0x200) && player.x < npc->x &&
          player.x > npc->x - (96 * 0x200)) {
        player.xvel -= 0x88;
        player.cond |= PLRCOND_IN_WIND;
      }

      break;
  }

  npc->rect = &rc[npc->anim];
}

// Fan (up)
void npc_act_097(npc_t *npc) {
  static const rect_t rc[3] = {
    {272, 136, 288, 152},
    {288, 136, 304, 152},
    {304, 136, 320, 152},
  };

  switch (npc->act) {
    case 0:
      if (npc->dir == 2)
        npc->act = 2;
      else
        npc->anim = 1;

      // Fallthrough
    case 1:
      npc->anim = 0;
      break;

    case 2:
      if (++npc->anim_wait > 0) {
        npc->anim_wait = 0;
        ++npc->anim;
      }

      if (npc->anim > 2) npc->anim = 0;

      if (player.x > npc->x - (((VID_WIDTH / 2) + 160) * 0x200) &&
          player.x < npc->x + (((VID_WIDTH / 2) + 160) * 0x200) &&
          player.y > npc->y - (((VID_HEIGHT / 2) + 120) * 0x200) &&
          player.y < npc->y + (((VID_HEIGHT / 2) + 120) * 0x200)) {
        if (m_rand(0, 5) == 1) npc_spawn(199, npc->x + (m_rand(-8, 8) * 0x200), npc->y, 0, 0, 1, NULL, 0x100);
      }

      if (player.x < npc->x + (8 * 0x200) && player.x > npc->x - (8 * 0x200) && player.y < npc->y &&
          player.y > npc->y - (96 * 0x200))
        player.yvel -= 0x88;

      break;
  }

  npc->rect = &rc[npc->anim];
}

// Fan (right)
void npc_act_098(npc_t *npc) {
  static const rect_t rc[3] = {
    {272, 152, 288, 168},
    {288, 152, 304, 168},
    {304, 152, 320, 168},
  };

  switch (npc->act) {
    case 0:
      if (npc->dir == 2)
        npc->act = 2;
      else
        npc->anim = 1;

      // Fallthrough
    case 1:
      npc->anim = 0;
      break;

    case 2:
      if (++npc->anim_wait > 0) {
        npc->anim_wait = 0;
        ++npc->anim;
      }

      if (npc->anim > 2) npc->anim = 0;

      if (player.x > npc->x - (((VID_WIDTH / 2) + 160) * 0x200) &&
          player.x < npc->x + (((VID_WIDTH / 2) + 160) * 0x200) &&
          player.y > npc->y - (((VID_HEIGHT / 2) + 120) * 0x200) &&
          player.y < npc->y + (((VID_HEIGHT / 2) + 120) * 0x200)) {
        if (m_rand(0, 5) == 1) npc_spawn(199, npc->x, npc->y + (m_rand(-8, 8) * 0x200), 0, 0, 2, NULL, 0x100);
      }

      if (player.y < npc->y + (8 * 0x200) && player.y > npc->y - (8 * 0x200) && player.x < npc->x + (96 * 0x200) &&
          player.x > npc->x) {
        player.xvel += 0x88;
        player.cond |= PLRCOND_IN_WIND;
      }

      break;
  }

  npc->rect = &rc[npc->anim];
}

// Fan (down)
void npc_act_099(npc_t *npc) {
  static const rect_t rc[3] = {
    {272, 168, 288, 184},
    {288, 168, 304, 184},
    {304, 168, 320, 184},
  };

  switch (npc->act) {
    case 0:
      if (npc->dir == 2)
        npc->act = 2;
      else
        npc->anim = 1;

      // Fallthrough
    case 1:
      npc->anim = 0;
      break;

    case 2:
      if (++npc->anim_wait > 0) {
        npc->anim_wait = 0;
        ++npc->anim;
      }

      if (npc->anim > 2) npc->anim = 0;

      if (player.x > npc->x - (((VID_WIDTH / 2) + 160) * 0x200) &&
          player.x < npc->x + (((VID_WIDTH / 2) + 160) * 0x200) &&
          player.y > npc->y - (((VID_HEIGHT / 2) + 120) * 0x200) &&
          player.y < npc->y + (((VID_HEIGHT / 2) + 120) * 0x200)) {
        if (m_rand(0, 5) == 1) npc_spawn(199, npc->x + (m_rand(-8, 8) * 0x200), npc->y, 0, 0, 3, NULL, 0x100);
      }

      if (player.x < npc->x + (8 * 0x200) && player.x > npc->x - (8 * 0x200) && player.y < npc->y + (96 * 0x200) &&
          player.y > npc->y)
        player.yvel += 0x88;

      break;
  }

  npc->rect = &rc[npc->anim];
}
