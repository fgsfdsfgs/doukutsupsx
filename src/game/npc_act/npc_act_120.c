#include "game/npc_act/npc_act.h"

// Colon (1)
void npc_act_120(npc_t *npc) {
  static const rect_t rect[2] = {
    {64, 0, 80, 16},
    {64, 16, 80, 32},
  };

  if (npc->dir == 0)
    npc->rect = &rect[0];
  else
    npc->rect = &rect[1];
}

// Colon (2)
void npc_act_121(npc_t *npc) {
  static const rect_t rect[3] = {
    {0, 0, 16, 16},
    {16, 0, 32, 16},
    {112, 0, 128, 16},
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

    npc->rect = &rect[npc->anim];
  } else {
    npc->rect = &rect[2];

    if (++npc->act_wait > 100) {
      npc->act_wait = 0;
      caret_spawn(npc->x, npc->y, CARET_ZZZ, DIR_LEFT);
    }
  }
}

// Colon (attacking)
void npc_act_122(npc_t *npc) {
  static const rect_t rc_left[10] = {
    {0, 0, 16, 16}, {16, 0, 32, 16}, {32, 0, 48, 16},  {0, 0, 16, 16},    {48, 0, 64, 16},
    {0, 0, 16, 16}, {80, 0, 96, 16}, {96, 0, 112, 16}, {112, 0, 128, 16}, {128, 0, 144, 16},
  };

  static const rect_t rc_right[10] = {
    {0, 16, 16, 32}, {16, 16, 32, 32}, {32, 16, 48, 32},  {0, 16, 16, 32},    {48, 16, 64, 32},
    {0, 16, 16, 32}, {80, 16, 96, 32}, {96, 16, 112, 32}, {112, 16, 128, 32}, {128, 16, 144, 32},
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

    case 10:
      npc->life = 1000;
      npc->act = 11;
      npc->act_wait = m_rand(0, 50);
      npc->anim = 0;
      npc->damage = 0;
      // Fallthrough
    case 11:
      if (npc->act_wait != 0)
        --npc->act_wait;
      else
        npc->act = 13;

      break;

    case 13:
      npc->act = 14;
      npc->act_wait = m_rand(0, 50);

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

      if (npc->anim > 5) npc->anim = 2;

      if (npc->dir == 0)
        npc->xvel -= 0x40;
      else
        npc->xvel += 0x40;

      if (npc->act_wait != 0) {
        --npc->act_wait;
      } else {
        npc->bits |= NPC_SHOOTABLE;
        npc->act = 15;
        npc->anim = 2;
        npc->yvel = -0x200;
        npc->damage = 2;
      }

      break;

    case 15:
      if (npc->flags & 8) {
        npc->bits |= NPC_SHOOTABLE;
        npc->xvel = 0;
        npc->act = 10;
        npc->damage = 0;
      }

      break;

    case 20:
      if (npc->flags & 8) {
        npc->xvel = 0;
        npc->act = 21;
        npc->damage = 0;

        if (npc->anim == 6)
          npc->anim = 8;
        else
          npc->anim = 9;

        npc->act_wait = m_rand(300, 400);
      }

      break;

    case 21:
      if (npc->act_wait) {
        --npc->act_wait;
      } else {
        npc->bits |= NPC_SHOOTABLE;
        npc->life = 1000;
        npc->act = 11;
        npc->act_wait = m_rand(0, 50);
        npc->anim = 0;
      }

      break;
  }

  if (npc->act > 10 && npc->act < 20 && npc->life != 1000) {
    npc->act = 20;
    npc->yvel = -0x200;
    npc->anim = m_rand(6, 7);
    npc->bits &= ~NPC_SHOOTABLE;
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

// Curly boss projectile
void npc_act_123(npc_t *npc) {
  static const rect_t rect[4] = {
    {192, 0, 208, 16},
    {208, 0, 224, 16},
    {224, 0, 240, 16},
    {240, 0, 256, 16},
  };

  bool do_break = FALSE;

  switch (npc->act) {
    case 0:
      npc->act = 1;
      caret_spawn(npc->x, npc->y, CARET_SHOOT, DIR_LEFT);
      snd_play_sound(PRIO_NORMAL, 32, FALSE);

      switch (npc->dir) {
        case 0:
          npc->xvel = -0x1000;
          npc->yvel = m_rand(-0x80, 0x80);
          break;

        case 1:
          npc->yvel = -0x1000;
          npc->xvel = m_rand(-0x80, 0x80);
          break;

        case 2:
          npc->xvel = 0x1000;
          npc->yvel = m_rand(-0x80, 0x80);
          break;

        case 3:
          npc->yvel = 0x1000;
          npc->xvel = m_rand(-0x80, 0x80);
          break;
      }

      break;

    case 1:
      switch (npc->dir) {
        case 0:
          if (npc->flags & 1) do_break = TRUE;
          break;

        case 1:
          if (npc->flags & 2) do_break = TRUE;
          break;

        case 2:
          if (npc->flags & 4) do_break = TRUE;
          break;

        case 3:
          if (npc->flags & 8) do_break = TRUE;
          break;
      }

      npc->x += npc->xvel;
      npc->y += npc->yvel;

      break;
  }

  if (do_break) {
    caret_spawn(npc->x, npc->y, CARET_PROJECTILE_DISSIPATION, DIR_RIGHT);
    snd_play_sound(PRIO_NORMAL, 28, FALSE);
    npc->cond = 0;
  }

  npc->rect = &rect[npc->dir];
}

// Sunstone
void npc_act_124(npc_t *npc) {
  static const rect_t rect[2] = {
    {160, 0, 192, 32},
    {192, 0, 224, 32},
  };

  switch (npc->act) {
    case 0:
      npc->act = 1;
      npc->x += 8 * 0x200;
      npc->y += 8 * 0x200;
      // Fallthrough
    case 1:
      npc->bits &= ~NPC_IGNORE_SOLIDITY;
      npc->anim = 0;
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
      if (++npc->act_wait % 8 == 0) snd_play_sound(PRIO_NORMAL, 26, FALSE);

      cam_start_quake_small(20);
      break;
  }

  npc->rect = &rect[npc->anim];
}

// Hidden item
void npc_act_125(npc_t *npc) {
  if (npc->life < 990) {
    npc_spawn_death_fx(npc->x, npc->y, npc->view.back, 8, 0);
    snd_play_sound(PRIO_NORMAL, 70, FALSE);

    if (npc->dir == 0)
      npc_spawn(87, npc->x, npc->y, 0, 0, 2, NULL, 0);
    else
      npc_spawn(86, npc->x, npc->y, 0, 0, 2, NULL, 0);

    npc->cond = 0;
  }

  static const rect_t rc[2] = {
      {0, 96, 16, 112},
      {16, 96, 32, 112},
  };

  if (npc->dir == 0)
    npc->rect = &rc[0];
  else
    npc->rect = &rc[1];
}

// Puppy (running)
void npc_act_126(npc_t *npc) {
  static const rect_t rc_left[6] = {
    {48, 144, 64, 160}, {64, 144, 80, 160},  {48, 144, 64, 160},
    {80, 144, 96, 160}, {96, 144, 112, 160}, {112, 144, 128, 160},
  };

  static const rect_t rc_right[6] = {
    {48, 160, 64, 176}, {64, 160, 80, 176},  {48, 160, 64, 176},
    {80, 160, 96, 176}, {96, 160, 112, 176}, {112, 160, 128, 176},
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

      if (npc->x - (96 * 0x200) < player.x && npc->x + (96 * 0x200) > player.x && npc->y - (32 * 0x200) < player.y &&
          npc->y + (16 * 0x200) > player.y) {
        if (npc->x > player.x)
          npc->dir = 0;
        else
          npc->dir = 2;
      }

      if (npc->x - (32 * 0x200) < player.x && npc->x + (32 * 0x200) > player.x && npc->y - (32 * 0x200) < player.y &&
          npc->y + (16 * 0x200) > player.y) {
        if (npc->x > player.x)
          npc->dir = 2;
        else
          npc->dir = 0;

        npc->act = 10;
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
      npc->anim = 4;
      npc->anim_wait = 0;
      // Fallthrough
    case 11:
      if (npc->flags & 8) {
        if (++npc->anim_wait > 2) {
          npc->anim_wait = 0;
          ++npc->anim;
        }

        if (npc->anim > 5) npc->anim = 4;
      } else {
        npc->anim = 5;
        npc->anim_wait = 0;
      }

      if (npc->xvel < 0 && npc->flags & 1) {
        npc->xvel /= -2;
        npc->dir = 2;
      }

      if (npc->xvel > 0 && npc->flags & 4) {
        npc->xvel /= -2;
        npc->dir = 0;
      }

      if (npc->dir == 0)
        npc->xvel -= 0x40;
      else
        npc->xvel += 0x40;

      if (npc->xvel > 0x5FF) npc->xvel = 0x400;

      if (npc->xvel < -0x5FF) npc->xvel = -0x400;

      break;
  }

  if (input_trig & IN_DOWN)
    npc->bits |= NPC_INTERACTABLE;
  else
    npc->bits &= ~NPC_INTERACTABLE;

  npc->yvel += 0x40;
  if (npc->yvel > 0x5FF) npc->yvel = 0x5FF;

  npc->x += npc->xvel;
  npc->y += npc->yvel;

  if (npc->dir == 0)
    npc->rect = &rc_left[npc->anim];
  else
    npc->rect = &rc_right[npc->anim];
}

// Machine gun trail (Level 2)
void npc_act_127(npc_t *npc) {
  static const rect_t rcV[3] = {
    {112, 48, 128, 64},
    {112, 64, 128, 80},
    {112, 80, 128, 96},
  };

  static const rect_t rcH[3] = {
    {64, 80, 80, 96},
    {80, 80, 96, 96},
    {96, 80, 112, 96},
  };

  if (++npc->anim_wait > 0) {
    npc->anim_wait = 0;

    if (++npc->anim > 2) {
      npc->cond = 0;
      return;
    }
  }

  if (npc->dir == 0)
    npc->rect = &rcH[npc->anim];
  else
    npc->rect = &rcV[npc->anim];
}

// Machine gun trail (Level 3)
void npc_act_128(npc_t *npc) {
  static const rect_t rc_left[5] = {
      {0, 0, 0, 0}, {176, 16, 184, 32}, {184, 16, 192, 32},
      {192, 16, 200, 32}, {200, 16, 208, 32},
  };

  static const rect_t rc_right[5] = {
      {0, 0, 0, 0}, {232, 16, 240, 32}, {224, 16, 232, 32},
      {216, 16, 224, 32}, {208, 16, 216, 32},
  };

  static const rect_t rc_up[5] = {
      {0, 0, 0, 0}, {176, 32, 192, 40}, {176, 40, 192, 48},
      {192, 32, 208, 40}, {192, 40, 208, 48},
  };

  static const rect_t rc_down[5] = {
      {0, 0, 0, 0}, {208, 32, 224, 40}, {208, 40, 224, 48},
      {224, 32, 232, 40}, {224, 40, 232, 48},
  };

  if (npc->act == 0) {
    npc->act = 1;

    if (npc->dir == 0 || npc->dir == 2) {
      npc->view.front = 4 * 0x200;
      npc->view.top = 8 * 0x200;
    } else {
      npc->view.front = 8 * 0x200;
      npc->view.top = 4 * 0x200;
    }
  }

  if (++npc->anim > 4) {
    npc->cond = 0;
    return;
  }

  switch (npc->dir) {
    case 0:
      npc->rect = &rc_left[npc->anim];
      break;

    case 1:
      npc->rect = &rc_up[npc->anim];
      break;

    case 2:
      npc->rect = &rc_right[npc->anim];
      break;

    case 3:
      npc->rect = &rc_down[npc->anim];
      break;
  }
}

// Fireball trail (Level 2 & 3)
void npc_act_129(npc_t *npc) {
  static const rect_t rect[18] = {
    {128, 48, 144, 64}, {144, 48, 160, 64}, {160, 48, 176, 64},
    {128, 64, 144, 80}, {144, 64, 160, 80}, {160, 64, 176, 80},
    {128, 80, 144, 96}, {144, 80, 160, 96}, {160, 80, 176, 96},
    {176, 48, 192, 64}, {192, 48, 208, 64}, {208, 48, 224, 64},
    {176, 64, 192, 80}, {192, 64, 208, 80}, {208, 64, 224, 80},
    {176, 80, 192, 96}, {192, 80, 208, 96}, {208, 80, 224, 96},
  };

  if (++npc->anim_wait > 1) {
    npc->anim_wait = 0;

    if (++npc->anim > 2) {
      npc->cond = 0;
      return;
    }
  }

  npc->y += npc->yvel;

  npc->rect = &rect[(npc->dir * 3) + npc->anim];
}

// Puppy (sitting, wagging tail)
void npc_act_130(npc_t *npc) {
  static const rect_t rc_left[4] = {
    {48, 144, 64, 160},
    {64, 144, 80, 160},
    {48, 144, 64, 160},
    {80, 144, 96, 160},
  };

  static const rect_t rc_right[4] = {
    {48, 160, 64, 176},
    {64, 160, 80, 176},
    {48, 160, 64, 176},
    {80, 160, 96, 176},
  };

  switch (npc->act) {
    case 0:
      npc->act = 1;
      npc->anim = 0;
      npc->anim_wait = 0;
      npc->bits |= NPC_INTERACTABLE;
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

      if (npc->x - (96 * 0x200) < player.x && npc->x + (96 * 0x200) > player.x && npc->y - (32 * 0x200) < player.y &&
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

// Puppy (sleeping)
void npc_act_131(npc_t *npc) {
  static const rect_t rc_left[1] = {{144, 144, 160, 160}};

  static const rect_t rc_right[1] = {{144, 160, 160, 176}};

  if (++npc->act_wait > 100) {
    npc->act_wait = 0;
    caret_spawn(npc->x, npc->y, CARET_ZZZ, DIR_LEFT);
  }

  if (npc->dir == 0)
    npc->rect = &rc_left[npc->anim];
  else
    npc->rect = &rc_right[npc->anim];
}

// Puppy (barking)
void npc_act_132(npc_t *npc) {
  static const rect_t rc_left[5] = {
    {48, 144, 64, 160}, {64, 144, 80, 160}, {96, 144, 112, 160},
    {96, 144, 112, 160}, {128, 144, 144, 160},
  };

  static const rect_t rc_right[5] = {
    {48, 160, 64, 176}, {64, 160, 80, 176}, {96, 160, 112, 176},
    {96, 160, 112, 176}, {128, 160, 144, 176},
  };

  if (npc->act < 100) {
    if (player.x < npc->x)
      npc->dir = 0;
    else
      npc->dir = 2;
  }

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

      if (npc->x - (64 * 0x200) < player.x && npc->x + (64 * 0x200) > player.x && npc->y - (16 * 0x200) < player.y &&
          npc->y + (16 * 0x200) > player.y) {
        if (++npc->anim_wait > 4) {
          npc->anim_wait = 0;
          ++npc->anim;
        }

        if (npc->anim > 4) npc->anim = 2;

        if (npc->anim == 4 && npc->anim_wait == 0) snd_play_sound(PRIO_NORMAL, 105, FALSE);
      } else {
        if (npc->anim == 4) npc->anim = 2;
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
      npc->anim = 0;
      npc->anim_wait = 0;
      // Fallthrough
    case 11:
      if (m_rand(0, 120) == 10) {
        npc->act = 12;
        npc->act_wait = 0;
        npc->anim = 1;
      }

      break;

    case 12:
      if (++npc->act_wait > 8) {
        npc->act = 11;
        npc->anim = 0;
      }

      break;

    case 100:
      npc->act = 101;
      npc->count1 = 0;
      // Fallthrough
    case 101:
      if (++npc->anim_wait > 4) {
        npc->anim_wait = 0;
        ++npc->anim;
      }

      if (npc->anim > 4) {
        if (npc->count1 < 3) {
          npc->anim = 2;
          ++npc->count1;
        } else {
          npc->anim = 0;
          npc->count1 = 0;
        }
      }

      if (npc->anim == 4 && npc->anim_wait == 0) snd_play_sound(PRIO_NORMAL, 105, FALSE);

      break;

    case 120:
      npc->anim = 0;
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

// Jenka
void npc_act_133(npc_t *npc) {
  static const rect_t rc_left[2] = {
    {176, 32, 192, 48},
    {192, 32, 208, 48},
  };

  static const rect_t rc_right[2] = {
    {176, 48, 192, 64},
    {192, 48, 208, 64},
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

// Armadillo
void npc_act_134(npc_t *npc) {
  static const rect_t rc_left[3] = {
    {224, 0, 256, 16},
    {256, 0, 288, 16},
    {288, 0, 320, 16},
  };

  static const rect_t rc_right[3] = {
    {224, 16, 256, 32},
    {256, 16, 288, 32},
    {288, 16, 320, 32},
  };

  switch (npc->act) {
    case 0:
      npc->act = 1;
      npc->anim = 2;
      npc->bits &= ~NPC_SHOOTABLE;
      npc->bits |= NPC_INVULNERABLE;
      // Fallthrough
    case 1:
      if (player.x > npc->x - (320 * 0x200) && player.x < npc->x + (320 * 0x200) && player.y > npc->y - (160 * 0x200) &&
          player.y < npc->y + (64 * 0x200))  // TODO: Maybe do something about this for widescreen/tallscreen?
      {
        npc->act = 10;
        npc->bits |= NPC_SHOOTABLE;
        npc->bits &= ~NPC_INVULNERABLE;
      }

      break;

    case 10:
      if (++npc->anim_wait > 4) {
        npc->anim_wait = 0;
        ++npc->anim;
      }

      if (npc->anim > 1) npc->anim = 0;

      if (npc->dir == 0 && npc->flags & 1) npc->dir = 2;
      if (npc->dir == 2 && npc->flags & 4) npc->dir = 0;

      if (npc->dir == 0)
        npc->x -= 0x100;
      else
        npc->x += 0x100;

      if (bullet_count_by_arm(6)) {
        npc->act = 20;
        npc->act_wait = 0;
        npc->anim = 2;
        npc->bits &= ~NPC_SHOOTABLE;
        npc->bits |= NPC_INVULNERABLE;
      }

      break;

    case 20:
      if (++npc->act_wait > 100) {
        npc->act = 10;
        npc->anim = 0;
        npc->anim_wait = 0;
        npc->bits |= NPC_SHOOTABLE;
        npc->bits &= ~NPC_INVULNERABLE;
      }

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

// Skeleton
void npc_act_135(npc_t *npc) {
  unsigned char deg;
  int xvel, yvel;

  static const rect_t rc_left[2] = {
    {256, 32, 288, 64},
    {288, 32, 320, 64},
  };

  static const rect_t rc_right[2] = {
    {256, 64, 288, 96},
    {288, 64, 320, 96},
  };

  if (player.x < npc->x - (352 * 0x200) || player.x > npc->x + (352 * 0x200) || player.y < npc->y - (160 * 0x200) ||
      player.y > npc->y + (64 * 0x200))
    npc->act = 0;

  switch (npc->act) {
    case 0:
      npc->act = 1;
      npc->xvel = 0;
      // Fallthrough
    case 1:
      if (player.x > npc->x - (320 * 0x200) && player.x < npc->x + (320 * 0x200) && player.y > npc->y - (160 * 0x200) &&
          player.y < npc->y + (64 * 0x200))
        npc->act = 10;

      if (npc->flags & 8) npc->anim = 0;

      break;

    case 10:
      npc->xvel = 0;
      npc->act = 11;
      npc->act_wait = 0;
      npc->anim = 0;
      // Fallthrough
    case 11:
      if (++npc->act_wait >= 5 && npc->flags & 8) {
        npc->act = 20;
        npc->anim = 1;
        npc->count1 = 0;
        npc->yvel = -0x200 * m_rand(1, 3);

        if (npc->shock) {
          if (npc->x < player.x)
            npc->xvel -= 0x100;
          else
            npc->xvel += 0x100;
        } else {
          if (npc->x < player.x)
            npc->xvel += 0x100;
          else
            npc->xvel -= 0x100;
        }
      }

      break;

    case 20:
      if (npc->yvel > 0 && npc->count1 == 0) {
        ++npc->count1;
        deg = m_atan2(npc->x - player.x, npc->y + (4 * 0x200) - player.y);
        yvel = m_sin(deg) * 2;
        xvel = m_cos(deg) * 2;
        npc_spawn(50, npc->x, npc->y, xvel, yvel, 0, NULL, 0x180);
        snd_play_sound(PRIO_NORMAL, 39, FALSE);
      }

      if (npc->flags & 8) {
        npc->act = 10;
        npc->anim = 0;
      }

      break;
  }

  if (npc->act >= 10) {
    if (npc->x > player.x)
      npc->dir = 0;
    else
      npc->dir = 2;
  }

  npc->yvel += 0x200 / 10;
  if (npc->yvel > 0x5FF) npc->yvel = 0x5FF;

  if (npc->xvel > 0x5FF) npc->xvel = 0x5FF;
  if (npc->xvel < -0x5FF) npc->xvel = -0x5FF;

  npc->y += npc->yvel;
  npc->x += npc->xvel;

  if (npc->dir == 0)
    npc->rect = &rc_left[npc->anim];
  else
    npc->rect = &rc_right[npc->anim];
}

// Puppy (carried)
void npc_act_136(npc_t *npc) {
  static const rect_t rc_left[2] = {
    {48, 144, 64, 160},
    {64, 144, 80, 160},
  };

  static const rect_t rc_right[2] = {
    {48, 160, 64, 176},
    {64, 160, 80, 176},
  };

  switch (npc->act) {
    case 0:
      npc->bits &= ~NPC_INTERACTABLE;
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

  if (player.dir == 0)
    npc->dir = 0;
  else
    npc->dir = 2;

  npc->y = player.y - (10 * 0x200);

  if (npc->dir == 0) {
    npc->x = player.x + (4 * 0x200);
    npc->rect = &rc_left[npc->anim];
  } else {
    npc->x = player.x - (4 * 0x200);
    npc->rect = &rc_right[npc->anim];
  }

  if (player.anim % 2)
    npc->rect_delta.top = 1;

  npc->rect_prev = NULL; // force texrect update
}

// Large door (frame)
void npc_act_137(npc_t *npc) {
  static const rect_t rc = {96, 136, 128, 188};

  npc->rect = &rc;
}

// Large door
void npc_act_138(npc_t *npc) {
  static const rect_t rc_left = {96, 112, 112, 136};
  static const rect_t rc_right = {112, 112, 128, 136};

  switch (npc->act) {
    case 0:
      npc->act = 1;

      if (npc->dir == 0) {
        npc->rect = &rc_left;
        npc->x += 8 * 0x200;
      } else {
        npc->rect = &rc_right;
        npc->x -= 8 * 0x200;
      }

      npc->tgt_x = npc->x;
      break;

    case 10:
      npc->act = 11;
      npc->anim = 1;
      npc->act_wait = 0;
      npc->bits |= NPC_IGNORE_SOLIDITY;
      // Fallthrough
    case 11:
      if (++npc->act_wait % 8 == 0)
        snd_play_sound(PRIO_NORMAL, 26, FALSE);

      if (npc->dir == 0) {
        npc->rect = &rc_left;
        npc->rect_delta.left = npc->act_wait / 8;
      } else {
        npc->x = npc->tgt_x + ((npc->act_wait / 8) * 0x200);
        npc->rect = &rc_right;
        npc->rect_delta.right = -npc->act_wait / 8;
      }

      if (npc->act_wait == 104)
        npc->cond = 0;

      npc->rect_prev = NULL; // force texrect update

      break;
  }
}

// Doctor
void npc_act_139(npc_t *npc) {
  static const rect_t rc_left[3] = {
    {0, 128, 24, 160},
    {24, 128, 48, 160},
    {48, 128, 72, 160},
  };

  static const rect_t rc_right[3] = {
    {0, 160, 24, 192},
    {24, 160, 48, 192},
    {48, 160, 72, 192},
  };

  switch (npc->act) {
    case 0:
      npc->act = 1;
      npc->xvel = 0;
      npc->yvel = 0;
      npc->y -= 8 * 0x200;
      // Fallthrough
    case 1:
      if (npc->flags & 8)
        npc->anim = 0;
      else
        npc->anim = 2;

      npc->yvel += 0x40;
      break;

    case 10:
      npc->act = 11;
      npc->anim = 1;
      npc->anim_wait = 0;
      npc->count1 = 0;
      // Fallthrough
    case 11:
      if (++npc->anim_wait > 6) {
        npc->anim_wait = 0;
        ++npc->anim;
      }

      if (npc->anim > 1) {
        npc->anim = 0;
        ++npc->count1;
      }

      if (npc->count1 > 8) {
        npc->anim = 0;
        npc->act = 1;
      }

      break;

    case 20:
      npc->act = 21;
      npc->act_wait = 0;
      npc->anim = 2;
      npc->tgt_y = npc->y - (32 * 0x200);
      // Fallthrough
    case 21:
      if (npc->y < npc->tgt_y)
        npc->yvel += 0x20;
      else
        npc->yvel -= 0x20;

      if (npc->yvel > 0x200) npc->yvel = 0x200;
      if (npc->yvel < -0x200) npc->yvel = -0x200;

      break;

    case 30:
      npc->act = 31;
      npc->xvel = 0;
      npc->yvel = 0;
      npc->act_wait = (npc->rect->bottom - npc->rect->top) * 2;
      snd_play_sound(PRIO_NORMAL, 29, FALSE);
      // Fallthrough
    case 31:
      --npc->act_wait;
      npc->anim = 0;

      if (npc->act_wait == 0) npc->cond = 0;

      break;

    case 40:
      npc->act = 41;
      npc->act_wait = 0;
      npc->xvel = 0;
      npc->yvel = 0;
      snd_play_sound(PRIO_NORMAL, 29, FALSE);
      // Fallthrough
    case 41:
      npc->anim = 2;

      if (++npc->act_wait < 64) break;

      npc->act = 20;
      break;
  }

  npc->x += npc->xvel;
  npc->y += npc->yvel;

  if (npc->dir == 0)
    npc->rect = &rc_left[npc->anim];
  else
    npc->rect = &rc_right[npc->anim];

  if (npc->act == 31 || npc->act == 41) {
    npc->rect_delta.bottom = npc->rect->top + (npc->act_wait / 2) - npc->rect->bottom;
    if (npc->act_wait / 2 % 2) npc->rect_delta.left = 1;
    npc->rect_prev = NULL; // force texrect update
  }
}
