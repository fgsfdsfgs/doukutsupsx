#include "game/npc_act/npc_act.h"

// Curly AI
void npc_act_180(npc_t *npc) {
  int xx, yy;

  static const rect_t rc_left[11] = {
    {0, 96, 16, 112},  {16, 96, 32, 112}, {0, 96, 16, 112},    {32, 96, 48, 112},
    {0, 96, 16, 112},  {48, 96, 64, 112}, {64, 96, 80, 112},   {48, 96, 64, 112},
    {80, 96, 96, 112}, {48, 96, 64, 112}, {144, 96, 160, 112},
  };

  static const rect_t rc_right[11] = {
    {0, 112, 16, 128},  {16, 112, 32, 128}, {0, 112, 16, 128},    {32, 112, 48, 128},
    {0, 112, 16, 128},  {48, 112, 64, 128}, {64, 112, 80, 128},   {48, 112, 64, 128},
    {80, 112, 96, 128}, {48, 112, 64, 128}, {144, 112, 160, 128},
  };

  if (npc->y < player.y - (10 * 0x10 * 0x200)) {
    if (npc->y < 16 * 0x10 * 0x200) {
      npc->tgt_x = 320 * 0x10 * 0x200;
      npc->tgt_y = npc->y;
    } else {
      npc->tgt_x = 0;
      npc->tgt_y = npc->y;
    }
  } else {
    if (npc_curly_state.shoot_wait != 0) {
      npc->tgt_x = npc_curly_state.shoot_x;
      npc->tgt_y = npc_curly_state.shoot_y;
    } else {
      npc->tgt_x = player.x;
      npc->tgt_y = player.y;
    }
  }

  if (npc->xvel < 0 && npc->flags & 1) npc->xvel = 0;
  if (npc->xvel > 0 && npc->flags & 4) npc->xvel = 0;

  switch (npc->act) {
    case 20:
      npc->x = player.x;
      npc->y = player.y;
      npc->act = 100;
      npc->anim = 0;
      npc_spawn(183, 0, 0, 0, 0, 0, npc, 0x100);

      if (npc_get_flag(563))
        npc_spawn(182, 0, 0, 0, 0, 0, npc, 0x100);
      else
        npc_spawn(181, 0, 0, 0, 0, 0, npc, 0x100);

      break;

    case 40:
      npc->act = 41;
      npc->act_wait = 0;
      npc->anim = 10;
      // Fallthrough
    case 41:
      if (++npc->act_wait == 750) {
        npc->bits &= ~NPC_INTERACTABLE;
        npc->anim = 0;
      }

      if (npc->act_wait > 1000) {
        npc->act = 100;
        npc->anim = 0;
        npc_spawn(183, 0, 0, 0, 0, 0, npc, 0x100);

        if (npc_get_flag(563))
          npc_spawn(182, 0, 0, 0, 0, 0, npc, 0x100);
        else
          npc_spawn(181, 0, 0, 0, 0, 0, npc, 0x100);
      }

      break;

    case 100:
      npc->anim = 0;
      npc->xvel = (npc->xvel * 7) / 8;
      npc->count1 = 0;

      if (npc->x > npc->tgt_x + (16 * 0x200)) {
        npc->act = 200;
        npc->anim = 1;
        npc->dir = 0;
        npc->act_wait = m_rand(20, 60);
      } else if (npc->x < npc->tgt_x - (16 * 0x200)) {
        npc->act = 300;
        npc->anim = 1;
        npc->dir = 2;
        npc->act_wait = m_rand(20, 60);
      }

      break;

    case 200:
      npc->xvel -= 0x20;
      npc->dir = 0;

      if (npc->flags & 1)
        ++npc->count1;
      else
        npc->count1 = 0;

      break;

    case 210:
      npc->xvel -= 0x20;
      npc->dir = 0;

      if (npc->flags & 8) npc->act = 100;

      break;

    case 300:
      npc->xvel += 0x20;
      npc->dir = 2;

      if (npc->flags & 4)
        ++npc->count1;
      else
        npc->count1 = 0;

      break;

    case 310:
      npc->xvel += 0x20;
      npc->dir = 2;

      if (npc->flags & 8) npc->act = 100;

      break;
  }

  if (npc_curly_state.shoot_wait != 0) --npc_curly_state.shoot_wait;

  if (npc_curly_state.shoot_wait == 70) npc->count2 = 10;

  if (npc_curly_state.shoot_wait == 60 && npc->flags & 8 && m_rand(0, 2)) {
    npc->count1 = 0;
    npc->yvel = -0x600;
    npc->anim = 1;
    snd_play_sound(-1, 15, SOUND_MODE_PLAY);

    if (npc->x > npc->tgt_x)
      npc->act = 210;
    else
      npc->act = 310;
  }

  xx = npc->x - npc->tgt_x;
  yy = npc->y - npc->tgt_y;

  if (xx < 0) xx *= -1;

  if (npc->act == 100) {
    if (xx + (2 * 0x200) < yy)
      npc->anim = 5;
    else
      npc->anim = 0;
  }

  if (npc->act == 210 || npc->act == 310) {
    if (xx + (2 * 0x200) < yy)
      npc->anim = 6;
    else
      npc->anim = 1;
  }

  if (npc->act == 200 || npc->act == 300) {
    ++npc->anim_wait;

    if (xx + (2 * 0x200) < yy)
      npc->anim = 6 + (npc->anim_wait / 4 % 4);
    else
      npc->anim = 1 + (npc->anim_wait / 4 % 4);

    if (npc->act_wait) {
      --npc->act_wait;

      // I assume this is what was intended
      if (npc->flags & 8 && npc->count1 > 10)
      {
        npc->count1 = 0;
        npc->yvel = -0x600;
        npc->act += 10;
        npc->anim = 1;
        snd_play_sound(-1, 15, SOUND_MODE_PLAY);
      }
    } else {
      npc->act = 100;
      npc->anim = 0;
    }
  }

  if (npc->act >= 100 && npc->act < 500) {
    if (npc->x < player.x - (80 * 0x200) || npc->x > player.x + (80 * 0x200)) {
      if (npc->flags & 5)
        npc->yvel += 0x200 / 32;
      else
        npc->yvel += 0x200 / 10;
    } else {
      npc->yvel += 0x200 / 10;
    }
  }

  if (npc->xvel > 0x300) npc->xvel = 0x300;
  if (npc->xvel < -0x300) npc->xvel = -0x300;

  if (npc->yvel > 0x5FF) npc->yvel = 0x5FF;

  npc->x += npc->xvel;
  npc->y += npc->yvel;

  if (npc->act >= 100 && !(npc->flags & 8)) {
    switch (npc->anim) {
      case 1000:
        break;

      default:
        if (xx + (2 * 0x200) < yy)
          npc->anim = 6;
        else
          npc->anim = 1;

        break;
    }
  }

  if (npc->dir == 0)
    npc->rect = &rc_left[npc->anim];
  else
    npc->rect = &rc_right[npc->anim];
}

// Curly AI Machine Gun
void npc_act_181(npc_t *npc) {
  static const rect_t rc_left[2] = {
    {216, 152, 232, 168},
    {232, 152, 248, 168},
  };

  static const rect_t rc_right[2] = {
    {216, 168, 232, 184},
    {232, 168, 248, 184},
  };

  if (npc->parent == NULL) return;

  if (npc->parent->anim < 5) {
    if (npc->parent->dir == 0) {
      npc->dir = 0;
      npc->x = npc->parent->x - (8 * 0x200);
    } else {
      npc->dir = 2;
      npc->x = npc->parent->x + (8 * 0x200);
    }

    npc->y = npc->parent->y;
    npc->anim = 0;
  } else {
    if (npc->parent->dir == 0) {
      npc->dir = 0;
      npc->x = npc->parent->x;
    } else {
      npc->dir = 2;
      npc->x = npc->parent->x;
    }

    npc->y = npc->parent->y - (10 * 0x200);
    npc->anim = 1;
  }

  if (npc->parent->anim == 1 || npc->parent->anim == 3 || npc->parent->anim == 6 || npc->parent->anim == 8)
    npc->y -= 1 * 0x200;

  switch (npc->act) {
    case 0:
      if (npc->parent->count2 == 10) {
        npc->parent->count2 = 0;
        npc->act = 10;
        npc->act_wait = 0;
      }

      break;

    case 10:
      if (++npc->act_wait % 6 == 1) {
        if (npc->anim == 0) {
          if (npc->dir == 0) {
            bullet_spawn(12, npc->x - (4 * 0x200), npc->y + (3 * 0x200), 0);
            caret_spawn(npc->x - (4 * 0x200), npc->y + (3 * 0x200), CARET_SHOOT, DIR_LEFT);
          } else {
            bullet_spawn(12, npc->x + (4 * 0x200), npc->y + (3 * 0x200), 2);
            caret_spawn(npc->x + (4 * 0x200), npc->y + (3 * 0x200), CARET_SHOOT, DIR_LEFT);
          }
        } else {
          if (npc->dir == 0) {
            bullet_spawn(12, npc->x - (2 * 0x200), npc->y - (4 * 0x200), 1);
            caret_spawn(npc->x - (2 * 0x200), npc->y - (4 * 0x200), CARET_SHOOT, DIR_LEFT);
          } else {
            bullet_spawn(12, npc->x + (2 * 0x200), npc->y - (4 * 0x200), 1);
            caret_spawn(npc->x + (2 * 0x200), npc->y - (4 * 0x200), CARET_SHOOT, DIR_LEFT);
          }
        }
      }

      if (npc->act_wait == 60) npc->act = 0;

      break;
  }

  if (npc->dir == 0)
    npc->rect = &rc_left[npc->anim];
  else
    npc->rect = &rc_right[npc->anim];
}

// Curly AI Polar Star
void npc_act_182(npc_t *npc) {
  static const rect_t rc_left[2] = {
    {184, 152, 200, 168},
    {200, 152, 216, 168},
  };

  static const rect_t rc_right[2] = {
    {184, 168, 200, 184},
    {200, 168, 216, 184},
  };

  if (npc->parent == NULL) return;

  if (npc->parent->anim < 5) {
    if (npc->parent->dir == 0) {
      npc->dir = 0;
      npc->x = npc->parent->x - (8 * 0x200);
    } else {
      npc->dir = 2;
      npc->x = npc->parent->x + (8 * 0x200);
    }

    npc->y = npc->parent->y;
    npc->anim = 0;
  } else {
    if (npc->parent->dir == 0) {
      npc->dir = 0;
      npc->x = npc->parent->x;
    } else {
      npc->dir = 2;
      npc->x = npc->parent->x;
    }

    npc->y = npc->parent->y - (10 * 0x200);
    npc->anim = 1;
  }

  if (npc->parent->anim == 1 || npc->parent->anim == 3 || npc->parent->anim == 6 || npc->parent->anim == 8)
    npc->y -= 1 * 0x200;

  switch (npc->act) {
    case 0:
      if (npc->parent->count2 == 10) {
        npc->parent->count2 = 0;
        npc->act = 10;
        npc->act_wait = 0;
      }

      break;

    case 10:
      if (++npc->act_wait % 12 == 1) {
        if (npc->anim == 0) {
          if (npc->dir == 0) {
            bullet_spawn(6, npc->x - (4 * 0x200), npc->y + (3 * 0x200), 0);
            caret_spawn(npc->x - (4 * 0x200), npc->y + (3 * 0x200), CARET_SHOOT, DIR_LEFT);
          } else {
            bullet_spawn(6, npc->x + (4 * 0x200), npc->y + (3 * 0x200), 2);
            caret_spawn(npc->x + (4 * 0x200), npc->y + (3 * 0x200), CARET_SHOOT, DIR_LEFT);
          }
        } else {
          if (npc->dir == 0) {
            bullet_spawn(6, npc->x - (2 * 0x200), npc->y - (4 * 0x200), 1);
            caret_spawn(npc->x - (2 * 0x200), npc->y - (4 * 0x200), CARET_SHOOT, DIR_LEFT);
          } else {
            bullet_spawn(6, npc->x + (2 * 0x200), npc->y - (4 * 0x200), 1);
            caret_spawn(npc->x + (2 * 0x200), npc->y - (4 * 0x200), CARET_SHOOT, DIR_LEFT);
          }
        }
      }

      if (npc->act_wait == 60) npc->act = 0;

      break;
  }

  if (npc->dir == 0)
    npc->rect = &rc_left[npc->anim];
  else
    npc->rect = &rc_right[npc->anim];
}

// Curly Air Tank Bubble
void npc_act_183(npc_t *npc) {
  static const rect_t rect[2] = {
    {56, 96, 80, 120},
    {80, 96, 104, 120},
  };

  if (npc->parent == NULL) return;

  switch (npc->act) {
    case 0:
      npc->x = npc->parent->x;
      npc->y = npc->parent->y;
      npc->act = 1;
      break;
  }

  npc->x += (npc->parent->x - npc->x) / 2;
  npc->y += (npc->parent->y - npc->y) / 2;

  if (++npc->anim_wait > 1) {
    npc->anim_wait = 0;
    ++npc->anim;
  }

  if (npc->anim > 1) npc->anim = 0;

  if (npc->parent->flags & 0x100)
    npc->rect = &rect[npc->anim];
  else
    npc->rect = NULL;
}

// Big Shutter
void npc_act_184(npc_t *npc) {
  int i;

  static const rect_t rc[4] = {
    {0, 64, 32, 96},
    {32, 64, 64, 96},
    {64, 64, 96, 96},
    {32, 64, 64, 96},
  };

  switch (npc->act) {
    case 0:
      npc->act = 1;
      npc->x += 8 * 0x200;
      npc->y += 8 * 0x200;
      break;

    case 10:
      npc->act = 11;
      npc->anim = 1;
      npc->act_wait = 0;
      npc->bits |= NPC_IGNORE_SOLIDITY;
      // Fallthrough
    case 11:
      switch (npc->dir) {
        case 0:
          npc->x -= 0x80;
          break;

        case 1:
          npc->y -= 0x80;
          break;

        case 2:
          npc->x += 0x80;
          break;

        case 3:
          npc->y += 0x80;
          break;
      }

      if ((++npc->act_wait % 8) == 0) snd_play_sound(-1, 26, SOUND_MODE_PLAY);

      cam_start_quake_small(20);
      break;

    case 20:
      for (i = 0; i < 4; ++i)
        npc_spawn(4, npc->x + (m_rand(-12, 12) * 0x200), npc->y + (16 * 0x200), m_rand(-0x155, 0x155),
          m_rand(-0x600, 0), 0, NULL, 0x100);

      npc->act = 1;
      break;
  }

  if (++npc->anim_wait > 10) {
    npc->anim_wait = 0;
    ++npc->anim;
  }

  if (npc->anim > 3) npc->anim = 0;

  npc->rect = &rc[npc->anim];
}

// Small Shutter
void npc_act_185(npc_t *npc) {
  static const rect_t rc = {96, 64, 112, 96};

  switch (npc->act) {
    case 0:
      npc->act = 1;
      npc->y += 8 * 0x200;
      break;

    case 10:
      npc->act = 11;
      npc->anim = 1;
      npc->act_wait = 0;
      npc->bits |= NPC_IGNORE_SOLIDITY;
      // Fallthrough
    case 11:
      switch (npc->dir) {
        case 0:
          npc->x -= 0x80;
          break;

        case 1:
          npc->y -= 0x80;
          break;

        case 2:
          npc->x += 0x80;
          break;

        case 3:
          npc->y += 0x80;
          break;
      }

      ++npc->act_wait;
      break;

    case 20:
      npc->y -= 24 * 0x200;
      npc->act = 1;
      break;
  }

  npc->rect = &rc;
}

// Lift block
void npc_act_186(npc_t *npc) {
  static const rect_t rc[4] = {
    {48, 48, 64, 64},
    {64, 48, 80, 64},
    {80, 48, 96, 64},
    {64, 48, 80, 64},
  };

  switch (npc->act) {
    case 0:
      npc->act = 1;
      // Fallthrough
    case 1:
      break;

    case 10:
      npc->act = 11;
      npc->anim = 1;
      npc->act_wait = 0;
      npc->bits |= NPC_IGNORE_SOLIDITY;
      // Fallthrough
    case 11:
      switch (npc->dir) {
        case 0:
          npc->x -= 0x80;
          break;

        case 1:
          npc->y -= 0x80;
          break;

        case 2:
          npc->x += 0x80;
          break;

        case 3:
          npc->y += 0x80;
          break;
      }

      ++npc->act_wait;
      break;
  }

  if (++npc->anim_wait > 10) {
    npc->anim_wait = 0;
    ++npc->anim;
  }

  if (npc->anim > 3) npc->anim = 0;

  npc->rect = &rc[npc->anim];
}

// Fuzz Core
void npc_act_187(npc_t *npc) {
  int i;

  switch (npc->act) {
    case 0:
      npc->act = 1;
      npc->tgt_x = npc->x;
      npc->tgt_y = npc->y;
      npc->count1 = 120;
      npc->act_wait = m_rand(0, 50);
      for (i = 0; i < 5; ++i)
        npc_spawn(188, 0, 0, 0, 0, 51 * i, npc, 0x100);
      // Fallthrough
    case 1:
      if (++npc->act_wait < 50) break;

      npc->act_wait = 0;
      npc->act = 2;
      npc->yvel = 0x300;
      break;

    case 2:
      npc->count1 += 4;

      if (player.x < npc->x)
        npc->dir = 0;
      else
        npc->dir = 2;

      if (npc->tgt_y < npc->y) npc->yvel -= 0x10;
      if (npc->tgt_y > npc->y) npc->yvel += 0x10;

      if (npc->yvel > 0x355) npc->yvel = 0x355;
      if (npc->yvel < -0x355) npc->yvel = -0x355;

      break;
  }

  npc->x += npc->xvel;
  npc->y += npc->yvel;

  static const rect_t rect_left[2] = {
    {224, 104, 256, 136},
    {256, 104, 288, 136},
  };

  static const rect_t rect_right[2] = {
    {224, 136, 256, 168},
    {256, 136, 288, 168},
  };

  if (++npc->anim_wait > 2) {
    npc->anim_wait = 0;
    ++npc->anim;
  }

  if (npc->anim > 1) npc->anim = 0;

  if (npc->dir == 0)
    npc->rect = &rect_left[npc->anim];
  else
    npc->rect = &rect_right[npc->anim];
}

// Fuzz
void npc_act_188(npc_t *npc) {
  u8 deg;

  switch (npc->act) {
    case 0:
      npc->act = 1;
      npc->count1 = npc->dir;
      // Fallthrough
    case 1:
      if (npc->parent->class_num == 187 && npc->parent->cond & 0x80) {
        deg = (npc->parent->count1 + npc->count1) % 0x100;
        npc->x = npc->parent->x + (m_sin(deg) * 20);
        npc->y = npc->parent->y + (m_cos(deg) * 0x20);
      } else {
        npc->xvel = m_rand(-0x200, 0x200);
        npc->yvel = m_rand(-0x200, 0x200);
        npc->act = 10;
      }

      break;

    case 10:
      if (player.x < npc->x)
        npc->xvel -= 0x20;
      else
        npc->xvel += 0x20;

      if (player.y < npc->y)
        npc->yvel -= 0x20;
      else
        npc->yvel += 0x20;

      if (npc->xvel > 0x800) npc->xvel = 0x800;
      if (npc->xvel < -0x800) npc->xvel = -0x800;

      if (npc->yvel > 0x200) npc->yvel = 0x200;
      if (npc->yvel < -0x200) npc->yvel = -0x200;

      npc->x += npc->xvel;
      npc->y += npc->yvel;

      break;
  }

  if (player.x < npc->x)
    npc->dir = 0;
  else
    npc->dir = 2;

  if (++npc->anim_wait > 2) {
    npc->anim_wait = 0;
    ++npc->anim;
  }

  if (npc->anim > 1) npc->anim = 0;

  static const rect_t rect_left[2] = {
    {288, 104, 304, 120},
    {304, 104, 320, 120},
  };

  static const rect_t rect_right[2] = {
    {288, 120, 304, 136},
    {304, 120, 320, 136},
  };

  if (npc->dir == 0)
    npc->rect = &rect_left[npc->anim];
  else
    npc->rect = &rect_right[npc->anim];
}

// Unused homing flame object (possibly related to the Core?)
void npc_act_189(npc_t *npc) {
  switch (npc->act) {
    case 0:
      npc->act = 1;
      npc->xvel = -0x40;
      // Fallthrough
    case 1:
      npc->y += npc->yvel;

      if (++npc->act_wait > 0x100) npc->act = 10;

      break;

    case 10:
      if (player.x < npc->x)
        npc->xvel -= 8;
      else
        npc->xvel += 8;

      if (player.y < npc->y)
        npc->yvel -= 8;
      else
        npc->yvel += 8;

      if (npc->xvel > 0x400) npc->xvel = 0x400;
      if (npc->xvel < -0x400) npc->xvel = -0x400;

      if (npc->yvel > 0x400) npc->yvel = 0x400;
      if (npc->yvel < -0x400) npc->yvel = -0x400;

      npc->x += npc->xvel;
      npc->y += npc->yvel;

      break;
  }

  if (player.x < npc->x)
    npc->dir = 0;
  else
    npc->dir = 2;

  if (++npc->anim_wait > 2) {
    npc->anim_wait = 0;
    ++npc->anim;
  }

  if (npc->anim > 2) npc->anim = 0;

  static const rect_t rect[3] = {
    {224, 184, 232, 200},
    {232, 184, 240, 200},
    {240, 184, 248, 200},
  };

  npc->rect = &rect[npc->anim];
}

// Broken robot
void npc_act_190(npc_t *npc) {
  static const rect_t rect[2] = {
    {192, 32, 208, 48},
    {208, 32, 224, 48},
  };

  int i;

  switch (npc->act) {
    case 0:
      npc->anim = 0;
      break;

    case 10:
      snd_play_sound(-1, 72, SOUND_MODE_PLAY);

      for (i = 0; i < 8; ++i)
        npc_spawn(4, npc->x, npc->y + (m_rand(-8, 8) * 0x200), m_rand(-8, -2) * 0x200, m_rand(-3, 3) * 0x200, 0, NULL, 0x100);

      npc->cond = 0;
      break;

    case 20:
      if (++npc->anim_wait > 10) {
        npc->anim_wait = 0;
        ++npc->anim;
      }

      if (npc->anim > 1) npc->anim = 0;

      break;
  }

  npc->rect = &rect[npc->anim];
}

// Water level
void npc_act_191(npc_t *npc) {
  switch (npc->act) {
    case 0:
      npc->act = 10;
      npc->tgt_y = npc->y;
      npc->yvel = 0x200;
      // Fallthrough
    case 10:
      if (npc->y < npc->tgt_y)
        npc->yvel += 4;
      else
        npc->yvel -= 4;

      if (npc->yvel < -0x100) npc->yvel = -0x100;
      if (npc->yvel > 0x100) npc->yvel = 0x100;

      npc->y += npc->yvel;
      break;

    case 20:
      npc->act = 21;
      npc->act_wait = 0;
      // Fallthrough
    case 21:
      if (npc->y < npc->tgt_y)
        npc->yvel += 4;
      else
        npc->yvel -= 4;

      if (npc->yvel < -0x200) npc->yvel = -0x200;
      if (npc->yvel > 0x200) npc->yvel = 0x200;

      npc->y += npc->yvel;

      if (++npc->act_wait > 1000) npc->act = 22;

      break;

    case 22:
      if (npc->y < 0)
        npc->yvel += 4;
      else
        npc->yvel -= 4;

      if (npc->yvel < -0x200) npc->yvel = -0x200;
      if (npc->yvel > 0x200) npc->yvel = 0x200;

      npc->y += npc->yvel;

      if (npc->y < 64 * 0x200 || npc_doctor_state.crystal_y != 0) {
        npc->act = 21;
        npc->act_wait = 0;
      }
      break;

    case 30:
      if (npc->y < 0)
        npc->yvel += 4;
      else
        npc->yvel -= 4;

      if (npc->yvel < -0x200) npc->yvel = -0x200;
      if (npc->yvel > 0x100) npc->yvel = 0x100;

      npc->y += npc->yvel;
      break;
  }

  stage_water_y = npc->y;

  npc->rect = NULL;
}

// Scooter
void npc_act_192(npc_t *npc) {
  switch (npc->act) {
    case 0:
      npc->act = 1;
      npc->view.back = 16 * 0x200;
      npc->view.front = 16 * 0x200;
      npc->view.top = 8 * 0x200;
      npc->view.bottom = 8 * 0x200;
      break;

    case 10:
      npc->act = 11;
      npc->anim = 1;
      npc->view.top = 16 * 0x200;
      npc->view.bottom = 16 * 0x200;
      npc->y -= 5 * 0x200;
      break;

    case 20:
      npc->act = 21;
      npc->act_wait = 1;
      npc->tgt_x = npc->x;
      npc->tgt_y = npc->y;
      // Fallthrough
    case 21:
      npc->x = npc->tgt_x + (m_rand(-1, 1) * 0x200);
      npc->y = npc->tgt_y + (m_rand(-1, 1) * 0x200);

      if (++npc->act_wait > 30) npc->act = 30;

      break;

    case 30:
      npc->act = 31;
      npc->act_wait = 1;
      npc->xvel = -0x800;
      npc->x = npc->tgt_x;
      npc->y = npc->tgt_y;
      snd_play_sound(-1, 44, SOUND_MODE_PLAY);
      // Fallthrough
    case 31:
      npc->xvel += 0x20;
      npc->x += npc->xvel;
      ++npc->act_wait;
      npc->y = npc->tgt_y + (m_rand(-1, 1) * 0x200);

      if (npc->act_wait > 10) npc->dir = 2;

      if (npc->act_wait > 200) npc->act = 40;

      break;

    case 40:
      npc->act = 41;
      npc->act_wait = 2;
      npc->dir = 0;
      npc->y -= 48 * 0x200;
      npc->xvel = -0x1000;
      // Fallthrough
    case 41:
      npc->x += npc->xvel;
      npc->y += npc->yvel;

      npc->act_wait += 2;

      if (npc->act_wait > 1200) npc->cond = 0;

      break;
  }

  if (npc->act_wait % 4 == 0 && npc->act >= 20) {
    snd_play_sound(-1, 34, SOUND_MODE_PLAY);
    if (npc->dir == 0)
      caret_spawn(npc->x + (10 * 0x200), npc->y + (10 * 0x200), CARET_EXHAUST, DIR_RIGHT);
    else
      caret_spawn(npc->x - (10 * 0x200), npc->y + (10 * 0x200), CARET_EXHAUST, DIR_LEFT);
  }

  static const rect_t rc_left[2] = {
    {224, 64, 256, 80},
    {256, 64, 288, 96},
  };

  static const rect_t rc_right[2] = {
    {224, 80, 256, 96},
    {288, 64, 320, 96},
  };

  if (npc->dir == 0)
    npc->rect = &rc_left[npc->anim];
  else
    npc->rect = &rc_right[npc->anim];
}

// Scooter (broken)
void npc_act_193(npc_t *npc) {
  static const rect_t rc = {256, 96, 320, 112};

  switch (npc->act) {
    case 0:
      npc->act = 1;
      npc->y =
          npc->y;  // This line probably isn't accurate to the original source code, but it produces the same assembly
      npc->x += 24 * 0x200;
      break;
  }

  npc->rect = &rc;
}

// Blue robot (broken)
void npc_act_194(npc_t *npc) {
  static const rect_t rc = {192, 120, 224, 128};

  if (npc->act == 0) {
    npc->act = 1;
    npc->y += 4 * 0x200;
  }

  npc->rect = &rc;
}

// Grate
void npc_act_195(npc_t *npc) {
  static const rect_t rc = {112, 64, 128, 80};
  npc->rect = &rc;
}

// Ironhead motion wall
void npc_act_196(npc_t *npc) {
  static const rect_t rc_left = {112, 64, 144, 80};
  static const rect_t rc_right = {112, 80, 144, 96};

  npc->x -= 6 * 0x200;

  if (npc->x <= 19 * 0x10 * 0x200) npc->x += 22 * 0x10 * 0x200;

  if (npc->dir == 0)
    npc->rect = &rc_left;
  else
    npc->rect = &rc_right;
}

// Porcupine Fish
void npc_act_197(npc_t *npc) {
  static const rect_t rc[4] = {
    {0, 0, 16, 16},
    {16, 0, 32, 16},
    {32, 0, 48, 16},
    {48, 0, 64, 16},
  };

  switch (npc->act) {
    case 0:
      npc->act = 10;
      npc->anim_wait = 0;
      npc->yvel = m_rand(-0x200, 0x200);
      npc->xvel = 0x800;
      // Fallthrough
    case 10:
      if (++npc->anim_wait > 2) {
        npc->anim_wait = 0;
        ++npc->anim;
      }

      if (npc->anim > 1) npc->anim = 0;

      if (npc->xvel < 0) {
        npc->damage = 3;
        npc->act = 20;
      }

      break;

    case 20:
      npc->damage = 3;

      if (++npc->anim_wait > 0) {
        npc->anim_wait = 0;
        ++npc->anim;
      }

      if (npc->anim > 3) npc->anim = 2;

      if (npc->x < 48 * 0x200) {
        npc->snd_die = 0;
        npc_kill(npc, TRUE);
      }

      break;
  }

  if (npc->flags & 2) npc->yvel = 0x200;
  if (npc->flags & 8) npc->yvel = -0x200;

  npc->xvel -= 12;

  npc->x += npc->xvel;
  npc->y += npc->yvel;

  npc->rect = &rc[npc->anim];
}

// Ironhead projectile
void npc_act_198(npc_t *npc) {
  static const rect_t rc_right[3] = {
    {208, 48, 224, 72},
    {224, 48, 240, 72},
    {240, 48, 256, 72},
  };

  switch (npc->act) {
    case 0:
      if (++npc->act_wait > 20) {
        npc->act = 1;
        npc->xvel = 0;
        npc->yvel = 0;
        npc->count1 = 0;
      }

      break;

    case 1:
      npc->xvel += 0x20;
      break;
  }

  if (++npc->anim_wait > 0) {
    npc->anim_wait = 0;
    ++npc->anim;
  }

  if (npc->anim > 2) npc->anim = 0;

  npc->x += npc->xvel;
  npc->y += npc->yvel;

  npc->rect = &rc_right[npc->anim];

  if (++npc->count1 > 100) npc->cond = 0;

  if (npc->count1 % 4 == 1) snd_play_sound(-1, 46, SOUND_MODE_PLAY);
}

// Water/wind particles
void npc_act_199(npc_t *npc) {
  static const rect_t rect[5] = {
    {72, 16, 74, 18}, {74, 16, 76, 18}, {76, 16, 78, 18},
    {78, 16, 80, 18}, {80, 16, 82, 18},
  };

  switch (npc->act) {
    case 0:
      npc->act = 1;
      npc->anim = m_rand(0, 2);

      switch (npc->dir) {
        case 0:
          npc->xvel = -1;
          break;

        case 1:
          npc->yvel = -1;
          break;

        case 2:
          npc->xvel = 1;
          break;

        case 3:
          npc->yvel = 1;
          break;
      }

      npc->xvel *= (m_rand(4, 8) * 0x200) / 2;
      npc->yvel *= (m_rand(4, 8) * 0x200) / 2;
      break;
  }

  if (++npc->anim_wait > 6) {
    npc->anim_wait = 0;
    ++npc->anim;
  }

  if (npc->anim > 4) {
    npc->cond = 0;
    return;
  }

  npc->x += npc->xvel;
  npc->y += npc->yvel;

  npc->rect = &rect[npc->anim];
}
