#include "game/npc_act/npc_act.h"

// Dragon Zombie
void npc_act_200(npc_t *npc) {
  u8 deg;
  int xvel, yvel;

  static const rect_t rc_left[6] = {
    {0, 0, 40, 40}, {40, 0, 80, 40}, {80, 0, 120, 40},
    {120, 0, 160, 40}, {160, 0, 200, 40}, {200, 0, 240, 40},
  };

  static const rect_t rc_right[6] = {
    {0, 40, 40, 80}, {40, 40, 80, 80}, {80, 40, 120, 80},
    {120, 40, 160, 80}, {160, 40, 200, 80}, {200, 40, 240, 80},
  };

  if (npc->act < 100 && npc->life < 950) {
    snd_play_sound(-1, 72, SOUND_MODE_PLAY);
    npc_death_fx(npc->x, npc->y, npc->view.back, 8);
    npc_spawn_exp(npc->x, npc->y, npc->exp);
    npc->act = 100;
    npc->bits &= ~NPC_SHOOTABLE;
    npc->damage = 0;
  }

  switch (npc->act) {
    case 0:
      npc->act = 10;
      npc->count1 = 0;
      // Fallthrough
    case 10:
      if (++npc->anim_wait > 30) {
        npc->anim_wait = 0;
        ++npc->anim;
      }

      if (npc->anim > 1) npc->anim = 0;

      if (npc->count1) --npc->count1;

      if (npc->count1 == 0 && player.x > npc->x - (112 * 0x200) && player.x < npc->x + (112 * 0x200)) npc->act = 20;

      break;

    case 20:
      npc->act = 21;
      npc->act_wait = 0;
      // Fallthrough
    case 21:
      if (++npc->act_wait / 2 % 2)
        npc->anim = 2;
      else
        npc->anim = 3;

      if (npc->act_wait > 30) npc->act = 30;

      if (player.x < npc->x)
        npc->dir = 0;
      else
        npc->dir = 2;

      break;

    case 30:
      npc->act = 31;
      npc->act_wait = 0;
      npc->anim = 4;
      npc->tgt_x = player.x;
      npc->tgt_y = player.y;
      // Fallthrough
    case 31:
      if (++npc->act_wait < 40 && npc->act_wait % 8 == 1) {
        if (npc->dir == 0)
          deg = m_atan2(npc->x - (14 * 0x200) - npc->tgt_x, npc->y - npc->tgt_y);
        else
          deg = m_atan2(npc->x + (14 * 0x200) - npc->tgt_x, npc->y - npc->tgt_y);

        deg += (u8)m_rand(-6, 6);

        yvel = m_sin(deg) * 3;
        xvel = m_cos(deg) * 3;

        if (npc->dir == 0)
          npc_spawn(202, npc->x - (14 * 0x200), npc->y, xvel, yvel, 0, NULL, 0x100);
        else
          npc_spawn(202, npc->x + (14 * 0x200), npc->y, xvel, yvel, 0, NULL, 0x100);

        if (!(player.cond & 2)) snd_play_sound(-1, 33, SOUND_MODE_PLAY);
      }

      if (npc->act_wait > 60) {
        npc->act = 10;
        npc->count1 = m_rand(100, 200);
        npc->anim_wait = 0;
      }

      break;

    case 100:
      npc->anim = 5;
      break;
  }

  if (npc->dir == 0)
    npc->rect = &rc_left[npc->anim];
  else
    npc->rect = &rc_right[npc->anim];
}

// Dragon Zombie (dead)
void npc_act_201(npc_t *npc) {
  static const rect_t rc_left[1] = {{200, 0, 240, 40}};

  static const rect_t rc_right[1] = {{200, 40, 240, 80}};

  if (npc->dir == 0)
    npc->rect = &rc_left[npc->anim];
  else
    npc->rect = &rc_right[npc->anim];
}

// Dragon Zombie projectile
void npc_act_202(npc_t *npc) {
  if (npc->flags & 0xFF) {
    npc->cond = 0;
    // SetCaret(npc->x, npc->y, CARET_PROJECTILE_DISSIPATION, DIR_LEFT);
  }

  npc->y += npc->yvel;
  npc->x += npc->xvel;

  static const rect_t rect_left[3] = {
    {184, 216, 200, 240},
    {200, 216, 216, 240},
    {216, 216, 232, 240},
  };

  if (++npc->anim_wait > 1) {
    npc->anim_wait = 0;
    ++npc->anim;
  }

  if (npc->anim > 2) npc->anim = 0;

  npc->rect = &rect_left[npc->anim];

  if (++npc->count1 > 300) {
    npc->cond = 0;
    // SetCaret(npc->x, npc->y, CARET_PROJECTILE_DISSIPATION, DIR_LEFT);
  }
}

// Critter (destroyed Egg Corridor)
void npc_act_203(npc_t *npc) {
  static const rect_t rc_left[3] = {
    {0, 80, 16, 96},
    {16, 80, 32, 96},
    {32, 80, 48, 96},
  };

  static const rect_t rc_right[3] = {
    {0, 96, 16, 112},
    {16, 96, 32, 112},
    {32, 96, 48, 112},
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

      if (npc->act_wait >= 8 && npc->x - (112 * 0x200) < player.x && npc->x + (112 * 0x200) > player.x &&
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

      if (npc->act_wait >= 8 && npc->x - (48 * 0x200) < player.x && npc->x + (48 * 0x200) > player.x &&
          npc->y - (80 * 0x200) < player.y && npc->y + (48 * 0x200) > player.y) {
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

        if (!(player.cond & 2)) snd_play_sound(-1, 30, SOUND_MODE_PLAY);

        if (npc->dir == 0)
          npc->xvel = -0x100;
        else
          npc->xvel = 0x100;
      }

      break;

    case 3:
      if (npc->flags & 8) {
        npc->xvel = 0;
        npc->act_wait = 0;
        npc->anim = 0;
        npc->act = 1;

        if (!(player.cond & 2)) snd_play_sound(-1, 23, SOUND_MODE_PLAY);
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

// Falling spike (small)
void npc_act_204(npc_t *npc) {
  static const rect_t rc[2] = {
    {240, 80, 256, 96},
    {240, 144, 256, 160},
  };

  switch (npc->act) {
    case 0:
      npc->act = 1;
      npc->tgt_x = npc->x;
      // Fallthrough
    case 1:
      if (player.x > npc->x - (12 * 0x200) && player.x < npc->x + (12 * 0x200) && player.y > npc->y) npc->act = 2;

      break;

    case 2:
      if (++npc->act_wait / 6 % 2)
        npc->x = npc->tgt_x - (1 * 0x200);
      else
        npc->x = npc->tgt_x;

      if (npc->act_wait > 30) {
        npc->act = 3;
        npc->anim = 1;
      }

      break;

    case 3:
      npc->yvel += 0x20;

      if (npc->flags & 0xFF) {
        if (!(player.cond & 2)) snd_play_sound(-1, 12, SOUND_MODE_PLAY);

        npc_death_fx(npc->x, npc->y, npc->view.back, 4);
        npc->cond = 0;
        return;
      }

      break;
  }

  if (npc->yvel > 0xC00) npc->yvel = 0xC00;

  npc->y += npc->yvel;

  npc->rect = &rc[npc->anim];
}

// Falling spike (large)
void npc_act_205(npc_t *npc) {
  static const rect_t rc[2] = {
    {112, 80, 128, 112},
    {128, 80, 144, 112},
  };

  switch (npc->act) {
    case 0:
      npc->act = 1;
      npc->tgt_x = npc->x;
      npc->y += 4 * 0x200;
      // Fallthrough
    case 1:
      if (player.x > npc->x - (12 * 0x200) && player.x < npc->x + (12 * 0x200) && player.y > npc->y) npc->act = 2;

      break;

    case 2:
      if (++npc->act_wait / 6 % 2)
        npc->x = npc->tgt_x - (1 * 0x200);
      else
        npc->x = npc->tgt_x;

      if (npc->act_wait > 30) {
        npc->act = 3;
        npc->anim = 1;
        npc->act_wait = 0;
      }

      break;

    case 3:
      npc->yvel += 0x20;

      if (player.y > npc->y) {
        npc->bits &= ~NPC_SOLID_HARD;
        npc->damage = 0x7F;
      } else {
        npc->bits |= NPC_SOLID_HARD;
        npc->damage = 0;
      }

      if (++npc->act_wait > 8 && npc->flags & 0xFF) {
        npc->bits |= NPC_SOLID_HARD;
        npc->act = 4;
        npc->act_wait = 0;
        npc->yvel = 0;
        npc->damage = 0;
        snd_play_sound(-1, 12, SOUND_MODE_PLAY);
        npc_death_fx(npc->x, npc->y, npc->view.back, 4);
        // SetBullet(24, npc->x, npc->y, 0);
        return;
      }

      break;

    case 4:
      if (++npc->act_wait > 4) {
        npc->act = 5;
        npc->bits |= NPC_SHOOTABLE;
      }

      break;
  }

  if (npc->yvel > 0xC00) npc->yvel = 0xC00;

  npc->y += npc->yvel;

  npc->rect = &rc[npc->anim];
}

// Counter Bomb
void npc_act_206(npc_t *npc) {
  switch (npc->act) {
    case 0:
      npc->act = 1;
      npc->tgt_x = npc->x;
      npc->tgt_y = npc->y;
      npc->count1 = 120;
      npc->act_wait = m_rand(0, 50);
      // Fallthrough
    case 1:
      if (++npc->act_wait < 50) break;

      npc->act_wait = 0;
      npc->act = 2;
      npc->yvel = 0x300;
      break;

    case 2:
      if (player.x > npc->x - (80 * 0x200) && player.x < npc->x + (80 * 0x200)) {
        npc->act_wait = 0;
        npc->act = 3;
      }

      if (npc->shock) {
        npc->act_wait = 0;
        npc->act = 3;
      }

      break;

    case 3:
      switch (npc->act_wait) {
        // Interestingly, this NPC counts down at 60 frames
        // per second, while NPC322 (Deleet) counts at 50.
        case 60 * 0:
          npc_spawn(207, npc->x + (16 * 0x200), npc->y + (4 * 0x200), 0, 0, 0, NULL, 0x100);
          break;

        case 60 * 1:
          npc_spawn(207, npc->x + (16 * 0x200), npc->y + (4 * 0x200), 0, 0, 1, NULL, 0x100);
          break;

        case 60 * 2:
          npc_spawn(207, npc->x + (16 * 0x200), npc->y + (4 * 0x200), 0, 0, 2, NULL, 0x100);
          break;

        case 60 * 3:
          npc_spawn(207, npc->x + (16 * 0x200), npc->y + (4 * 0x200), 0, 0, 3, NULL, 0x100);
          break;

        case 60 * 4:
          npc_spawn(207, npc->x + (16 * 0x200), npc->y + (4 * 0x200), 0, 0, 4, NULL, 0x100);
          break;

        case 60 * 5:
          npc->hit.back = 128 * 0x200;
          npc->hit.front = 128 * 0x200;
          npc->hit.top = 100 * 0x200;
          npc->hit.bottom = 100 * 0x200;
          npc->damage = 30;
          snd_play_sound(-1, 35, SOUND_MODE_PLAY);
          npc_death_fx(npc->x, npc->y, 0x10000, 100);
          // SetQuake(20);
          npc->cond |= 8;
          break;
      }

      ++npc->act_wait;
      break;
  }

  if (npc->act > 1) {
    if (npc->tgt_y < npc->y) npc->yvel -= 0x10;
    if (npc->tgt_y > npc->y) npc->yvel += 0x10;

    if (npc->yvel > 0x100) npc->yvel = 0x100;
    if (npc->yvel < -0x100) npc->yvel = -0x100;
  }

  npc->x += npc->xvel;
  npc->y += npc->yvel;

  static const rect_t rect_left[3] = {
    {80, 80, 120, 120},
    {120, 80, 160, 120},
    {160, 80, 200, 120},
  };

  if (++npc->anim_wait > 4) {
    npc->anim_wait = 0;
    ++npc->anim;
  }

  if (npc->anim > 2) npc->anim = 0;

  npc->rect = &rect_left[npc->anim];
}

// Counter Bomb's countdown
void npc_act_207(npc_t *npc) {
  static const rect_t rc[5] = {
    {0, 144, 16, 160}, {16, 144, 32, 160}, {32, 144, 48, 160},
    {48, 144, 64, 160}, {64, 144, 80, 160},
  };

  switch (npc->act) {
    case 0:
      npc->act = 1;
      npc->anim = npc->dir;
      snd_play_sound(-1, 43, SOUND_MODE_PLAY);
      // Fallthrough
    case 1:
      npc->x += 1 * 0x200;

      if (++npc->act_wait > 8) {
        npc->act_wait = 0;
        npc->act = 2;
      }

      break;

    case 2:
      if (++npc->act_wait > 30) {
        npc->cond = 0;
        return;
      }

      break;
  }

  npc->rect = &rc[npc->anim];
}

// Basu (destroyed Egg Corridor)
void npc_act_208(npc_t *npc) {
  static const rect_t rc_left[3] = {
    {248, 80, 272, 104},
    {272, 80, 296, 104},
    {296, 80, 320, 104},
  };

  static const rect_t rc_right[3] = {
    {248, 104, 272, 128},
    {272, 104, 296, 128},
    {296, 104, 320, 128},
  };

  switch (npc->act) {
    case 0:
      if (player.x < npc->x + (16 * 0x200) && player.x > npc->x - (16 * 0x200)) {
        npc->bits |= NPC_SHOOTABLE;
        npc->yvel = -0x200;
        npc->tgt_x = npc->x;
        npc->tgt_y = npc->y;
        npc->act = 1;
        npc->act_wait = 0;
        npc->count1 = npc->dir;
        npc->count2 = 0;
        npc->damage = 6;

        if (npc->dir == 0) {
          npc->x = player.x + (256 * 0x200);
          npc->xvel = -0x2FF;
        } else {
          npc->x = player.x - (256 * 0x200);
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

      if (npc->yvel > 0x200) npc->yvel = 0x200;
      if (npc->yvel < -0x200) npc->yvel = -0x200;

      if (npc->shock) {
        npc->x += npc->xvel / 2;
        npc->y += npc->yvel / 2;
      } else {
        npc->x += npc->xvel;
        npc->y += npc->yvel;
      }

      if (player.x > npc->x + (400 * 0x200) || player.x < npc->x - (400 * 0x200)) {
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

  if (npc->act != 0) {
    if (npc->act_wait < 150) ++npc->act_wait;

    if (npc->act_wait == 150) {
      if (++npc->count2 % 8 == 0 && npc->x < player.x + (160 * 0x200) && npc->x > player.x - (160 * 0x200)) {
        u8 deg;
        int xvel;
        int yvel;

        deg = m_atan2(npc->x - player.x, npc->y - player.y);
        deg += (u8)m_rand(-6, 6);
        yvel = m_sin(deg) * 3;
        xvel = m_cos(deg) * 3;
        npc_spawn(209, npc->x, npc->y, xvel, yvel, 0, NULL, 0x100);
        snd_play_sound(-1, 39, SOUND_MODE_PLAY);
      }

      if (npc->count2 > 16) {
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

// Basu projectile (destroyed Egg Corridor)
void npc_act_209(npc_t *npc) {
  if (npc->flags & 0xFF) {
    npc->cond = 0;
    // SetCaret(npc->x, npc->y, CARET_PROJECTILE_DISSIPATION, DIR_LEFT);
  }

  npc->y += npc->yvel;
  npc->x += npc->xvel;

  static const rect_t rect_left[4] = {
    {232, 96, 248, 112},
    {200, 112, 216, 128},
    {216, 112, 232, 128},
    {232, 112, 248, 128},
  };

  if (++npc->anim_wait > 2) {
    npc->anim_wait = 0;
    ++npc->anim;
  }

  if (npc->anim > 3) npc->anim = 0;

  npc->rect = &rect_left[npc->anim];

  if (++npc->count1 > 300) {
    npc->cond = 0;
    // SetCaret(npc->x, npc->y, CARET_PROJECTILE_DISSIPATION, DIR_LEFT);
  }
}

// Beetle (destroyed Egg Corridor)
void npc_act_210(npc_t *npc) {
  static const rect_t rc_left[2] = {
    {0, 112, 16, 128},
    {16, 112, 32, 128},
  };

  static const rect_t rc_right[2] = {
    {32, 112, 48, 128},
    {48, 112, 64, 128},
  };

  switch (npc->act) {
    case 0:
      if (player.x < npc->x + (16 * 0x200) && player.x > npc->x - (16 * 0x200)) {
        npc->bits |= NPC_SHOOTABLE;
        npc->yvel = -0x200;
        npc->tgt_y = npc->y;
        npc->act = 1;
        npc->damage = 2;

        if (npc->dir == 0) {
          npc->x = player.x + (256 * 0x200);
          npc->xvel = -0x2FF;
        } else {
          npc->x = player.x - (256 * 0x200);
          npc->xvel = 0x2FF;
        }
      } else {
        npc->bits &= ~NPC_SHOOTABLE;
        npc->rect = NULL;
        npc->damage = 0;
        npc->xvel = 0;
        npc->yvel = 0;
        return;
      }

      break;

    case 1:
      if (npc->x > player.x) {
        npc->dir = 0;
        npc->xvel -= 0x10;
      } else {
        npc->dir = 2;
        npc->xvel += 0x10;
      }

      if (npc->xvel > 0x2FF) npc->xvel = 0x2FF;
      if (npc->xvel < -0x2FF) npc->xvel = -0x2FF;

      if (npc->y < npc->tgt_y)
        npc->yvel += 8;
      else
        npc->yvel -= 8;

      if (npc->yvel > 0x200) npc->yvel = 0x200;
      if (npc->yvel < -0x200) npc->yvel = -0x200;

      if (npc->shock) {
        npc->x += npc->xvel / 2;
        npc->y += npc->yvel / 2;
      } else {
        npc->x += npc->xvel;
        npc->y += npc->yvel;
      }

      break;
  }

  if (++npc->anim_wait > 1) {
    npc->anim_wait = 0;
    ++npc->anim;
  }

  if (npc->anim > 1) npc->anim = 0;

  if (npc->dir == 0)
    npc->rect = &rc_left[npc->anim];
  else
    npc->rect = &rc_right[npc->anim];
}

// Spikes (small)
void npc_act_211(npc_t *npc) {
  static const rect_t rects[4] = {
    {256, 200, 272, 216},
    {272, 200, 288, 216},
    {288, 200, 304, 216},
    {304, 200, 320, 216},
  };

  npc->rect = &rects[npc->event_num];
}

// Sky Dragon
void npc_act_212(npc_t *npc) {
  static const rect_t rc_right[4] = {
    {160, 152, 200, 192},
    {200, 152, 240, 192},
    {240, 112, 280, 152},
    {280, 112, 320, 152},
  };

  switch (npc->act) {
    case 0:
      npc->act = 1;
      npc->y -= 4 * 0x200;
      // Fallthrough
    case 1:
      if (++npc->anim_wait > 30) {
        npc->anim_wait = 0;
        ++npc->anim;
      }

      if (npc->anim > 1) npc->anim = 0;

      break;

    case 10:
      npc->act = 11;
      npc->anim = 2;
      npc->anim_wait = 0;
      npc->tgt_y = npc->y - (16 * 0x200);
      npc->tgt_x = npc->x - (6 * 0x200);
      npc->yvel = 0;
      npc->bits |= NPC_IGNORE_SOLIDITY;
      // Fallthrough
    case 11:
      if (npc->x < npc->tgt_x)
        npc->xvel += 8;
      else
        npc->xvel -= 8;

      if (npc->y < npc->tgt_y)
        npc->yvel += 8;
      else
        npc->yvel -= 8;

      npc->x += npc->xvel;
      npc->y += npc->yvel;

      if (++npc->anim_wait > 5) {
        npc->anim_wait = 0;
        ++npc->anim;
      }

      if (npc->anim > 3) npc->anim = 2;

      break;

    case 20:
      npc->act = 21;
      npc->bits |= NPC_IGNORE_SOLIDITY;
      // Fallthrough
    case 21:
      if (npc->y < npc->tgt_y)
        npc->yvel += 0x10;
      else
        npc->yvel -= 0x10;

      npc->xvel += 0x20;

      if (npc->xvel > 0x600) npc->xvel = 0x600;
      if (npc->xvel < -0x600) npc->xvel = -0x600;

      npc->x += npc->xvel;
      npc->y += npc->yvel;

      if (++npc->anim_wait > 2) {
        npc->anim_wait = 0;
        ++npc->anim;
      }

      if (npc->anim > 3) npc->anim = 2;

      break;

    case 30:
      npc->act = 31;
      npc_spawn(297, 0, 0, 0, 0, 0, npc, 0x100);
      break;
  }

  npc->rect = &rc_right[npc->anim];

  // Use different sprite if player is wearing the Mimiga Mask
  if (player.equip & EQUIP_MIMIGA_MASK) {
    if (npc->anim > 1) {
      npc->rect_delta.top = 40;
      npc->rect_delta.bottom = 40;
      npc->rect_prev = NULL; // force texrect update
    }
  }
}

// Night Spirit
void npc_act_213(npc_t *npc) {
  static const rect_t rect[10] = {
    {0, 0, 0, 0},      {0, 0, 48, 48},    {48, 0, 96, 48}, {96, 0, 144, 48}, {144, 0, 192, 48},
    {192, 0, 240, 48}, {240, 0, 288, 48}, {0, 48, 48, 96}, {48, 48, 96, 96}, {96, 48, 144, 96},
  };

  switch (npc->act) {
    case 0:
      npc->anim = 0;
      npc->tgt_x = npc->x;
      npc->tgt_y = npc->y;
      // Fallthrough
    case 1:
      if (player.y > npc->y - (8 * 0x200) && player.y < npc->y + (8 * 0x200)) {
        if (npc->dir == 0)
          npc->y -= 240 * 0x200;
        else
          npc->y += 240 * 0x200;

        npc->act = 10;
        npc->act_wait = 0;
        npc->anim = 1;
        npc->yvel = 0;
        npc->bits |= NPC_SHOOTABLE;
      }

      break;

    case 10:
      if (++npc->anim_wait > 2) {
        npc->anim_wait = 0;
        ++npc->anim;
      }

      if (npc->anim > 3) npc->anim = 1;

      if (++npc->act_wait > 200) {
        npc->act = 20;
        npc->act_wait = 0;
        npc->anim = 4;
      }

      break;

    case 20:
      if (++npc->anim_wait > 2) {
        npc->anim_wait = 0;
        ++npc->anim;
      }

      if (npc->anim > 6) npc->anim = 4;

      if (++npc->act_wait > 50) {
        npc->act = 30;
        npc->act_wait = 0;
        npc->anim = 7;
      }

      break;

    case 30:
      if (++npc->anim_wait > 2) {
        npc->anim_wait = 0;
        ++npc->anim;
      }

      if (npc->anim > 9) npc->anim = 7;

      if (++npc->act_wait % 5 == 1) {
        npc_spawn(214, npc->x, npc->y, (m_rand(2, 12) * 0x200) / 4, m_rand(-0x200, 0x200), 0, NULL, 0x100);
        snd_play_sound(-1, 21, SOUND_MODE_PLAY);
      }

      if (npc->act_wait > 50) {
        npc->act = 10;
        npc->act_wait = 0;
        npc->anim = 1;
      }

      break;

    case 40:
      if (npc->y < npc->tgt_y)
        npc->yvel += 0x40;
      else
        npc->yvel -= 0x40;

      if (npc->yvel < -0x400) npc->yvel = -0x400;
      if (npc->yvel > 0x400) npc->yvel = 0x400;

      if (npc->shock)
        npc->y += npc->yvel / 2;
      else
        npc->y += npc->yvel;

      if (++npc->anim_wait > 2) {
        npc->anim_wait = 0;
        ++npc->anim;
      }

      if (npc->anim > 6) npc->anim = 4;

      if (player.y < npc->tgt_y + (240 * 0x200) && player.y > npc->tgt_y - (240 * 0x200)) {
        npc->act = 20;
        npc->act_wait = 0;
        npc->anim = 4;
      }

      break;
  }

  if (npc->act >= 10 && npc->act <= 30) {
    if (npc->y < player.y)
      npc->yvel += 25;
    else
      npc->yvel -= 25;

    if (npc->yvel < -0x400) npc->yvel = -0x400;
    if (npc->yvel > 0x400) npc->yvel = 0x400;

    if (npc->flags & 2) npc->yvel = 0x200;
    if (npc->flags & 8) npc->yvel = -0x200;

    if (npc->shock)
      npc->y += npc->yvel / 2;
    else
      npc->y += npc->yvel;

    if (player.y > npc->tgt_y + (240 * 0x200) || player.y < npc->tgt_y - (240 * 0x200)) npc->act = 40;
  }

  npc->rect = &rect[npc->anim];
}

// Night Spirit projectile
void npc_act_214(npc_t *npc) {
  static const rect_t rect[3] = {
    {144, 48, 176, 64},
    {176, 48, 208, 64},
    {208, 48, 240, 64},
  };

  switch (npc->act) {
    case 0:
      npc->act = 1;
      npc->bits |= NPC_IGNORE_SOLIDITY;
      // Fallthrough
    case 1:
      if (++npc->anim_wait > 2) {
        npc->anim_wait = 0;
        ++npc->anim;
      }

      if (npc->anim > 2) npc->anim = 0;

      npc->xvel -= 25;

      npc->x += npc->xvel;
      npc->y += npc->yvel;

      if (npc->xvel < 0) npc->bits &= ~NPC_IGNORE_SOLIDITY;

      if (npc->flags & 0xFF) {
        npc_death_fx(npc->x, npc->y, npc->view.back, 4);
        snd_play_sound(-1, 28, SOUND_MODE_PLAY);
        npc->cond = 0;
      }

      break;
  }

  npc->rect = &rect[npc->anim];
}

// Sandcroc (Outer Wall)
void npc_act_215(npc_t *npc) {
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
      if (player.x > npc->x - (12 * 0x200) && player.x < npc->x + (12 * 0x200) && player.y > npc->y &&
          player.y < npc->y + (8 * 0x200)) {
        npc->act = 15;
        npc->act_wait = 0;
      }

      break;

    case 15:
      if (++npc->act_wait > 10) {
        snd_play_sound(-1, 102, SOUND_MODE_PLAY);
        npc->act = 20;
      }

      break;

    case 20:
      if (++npc->anim_wait > 3) {
        ++npc->anim;
        npc->anim_wait = 0;
      }

      if (npc->anim == 3) npc->damage = 15;

      if (npc->anim == 4) {
        npc->bits |= NPC_SHOOTABLE;
        npc->act = 30;
        npc->act_wait = 0;
      }

      break;

    case 30:
      npc->bits |= NPC_SOLID_SOFT;
      npc->damage = 0;
      ++npc->act_wait;

      if (npc->shock) {
        npc->act = 40;
        npc->act_wait = 0;
      }

      break;

    case 40:
      npc->bits |= NPC_IGNORE_SOLIDITY;
      npc->y += 1 * 0x200;

      if (++npc->act_wait == 32) {
        npc->bits &= ~NPC_SOLID_SOFT;
        npc->bits &= ~NPC_SHOOTABLE;
        npc->act = 50;
        npc->act_wait = 0;
      }

      break;

    case 50:
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
    {0, 0, 0, 0}, {0, 96, 48, 128}, {48, 96, 96, 128},
    {96, 96, 144, 128}, {144, 96, 192, 128},
  };

  npc->rect = &rect[npc->anim];
}

// Debug Cat
void npc_act_216(npc_t *npc) {
  static const rect_t rect = {256, 192, 272, 216};

  npc->rect = &rect;
}

// Itoh
void npc_act_217(npc_t *npc) {
  static const rect_t rect[8] = {
    {144, 64, 160, 80}, {160, 64, 176, 80}, {176, 64, 192, 80}, {192, 64, 208, 80},
    {144, 80, 160, 96}, {160, 80, 176, 96}, {144, 80, 160, 96}, {176, 80, 192, 96},
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

    case 10:
      npc->anim = 2;
      npc->xvel = 0;
      break;

    case 20:
      npc->act = 21;
      npc->anim = 2;
      npc->xvel += 0x200;
      npc->yvel -= 0x400;
      break;

    case 21:
      if (npc->flags & 8) {
        npc->anim = 3;
        npc->act = 30;
        npc->act_wait = 0;
        npc->xvel = 0;
        npc->tgt_x = npc->x;
      }

      break;

    case 30:
      npc->anim = 3;

      if (++npc->act_wait / 2 % 2)
        npc->x = npc->tgt_x + (1 * 0x200);
      else
        npc->x = npc->tgt_x;

      break;

    case 40:
      npc->act = 41;
      npc->yvel = -0x200;
      npc->anim = 2;
      // Fallthrough
    case 41:
      if (npc->flags & 8) {
        npc->act = 42;
        npc->anim = 4;
      }

      break;

    case 42:
      npc->xvel = 0;
      npc->anim = 4;
      break;

    case 50:
      npc->act = 51;
      npc->act_wait = 0;
      // Fallthrough
    case 51:
      if (++npc->act_wait > 32) npc->act = 42;

      npc->xvel = 0x200;

      if (++npc->anim_wait > 3) {
        npc->anim_wait = 0;
        ++npc->anim;
      }

      if (npc->anim > 7) npc->anim = 4;

      break;
  }

  npc->yvel += 0x40;
  if (npc->yvel > 0x5FF) npc->yvel = 0x5FF;

  npc->x += npc->xvel;
  npc->y += npc->yvel;

  npc->rect = &rect[npc->anim];
}

// Core giant energy ball projectile
void npc_act_218(npc_t *npc) {
  static const rect_t rc[2] = {
    {256, 120, 288, 152},
    {288, 120, 320, 152},
  };

  npc->x += npc->xvel;
  npc->y += npc->yvel;

  if (++npc->act_wait > 200) npc->cond = 0;

  if (++npc->anim_wait > 2) {
    npc->anim_wait = 0;
    ++npc->anim;
  }

  if (npc->anim > 1) npc->anim = 0;

  npc->rect = &rc[npc->anim];
}

// Smoke generator
void npc_act_219(npc_t *npc) {
  static const rect_t rc = {0, 0, 0, 0};

  if (npc->dir == 0) {
    if (m_rand(0, 40) == 1)
      npc_spawn(4, npc->x + (m_rand(-20, 20) * 0x200), npc->y, 0, -0x200, 0, NULL, 0x100);
  } else {
    npc_spawn(199, npc->x + (m_rand(-0xA0, 0xA0) * 0x200), npc->y + (m_rand(-0x80, 0x80) * 0x200), 0, 0, 2, NULL, 0x100);
  }

  npc->rect = &rc;
}
