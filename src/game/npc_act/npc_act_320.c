#include "game/npc_act/npc_act.h"

// Curly (carried, shooting)
void npc_act_320(npc_t *npc) {
  static const rect_t rc_left[3] = {
    {16, 96, 32, 112},
    {48, 96, 64, 112},
    {96, 96, 112, 112},
  };

  static const rect_t rc_right[3] = {
    {16, 112, 32, 128},
    {48, 112, 64, 128},
    {96, 112, 112, 128},
  };

  if (npc->act == 0) {
    npc->act = 1;
    npc->x = player.x;
    npc->y = player.y;
    npc_spawn(321, 0, 0, 0, 0, 0, npc, 0x100);
  }

  if (player.flags & 8) {
    if (player.up) {
      npc->tgt_x = player.x;
      npc->tgt_y = player.y - (10 * 0x200);
      npc->anim = 1;
    } else {
      npc->anim = 0;

      if (player.dir == 0) {
        npc->tgt_x = player.x + (7 * 0x200);
        npc->tgt_y = player.y - (3 * 0x200);
      } else {
        npc->tgt_x = player.x - (7 * 0x200);
        npc->tgt_y = player.y - (3 * 0x200);
      }
    }
  } else {
    if (player.up) {
      npc->tgt_x = player.x;
      npc->tgt_y = player.y + (8 * 0x200);
      npc->anim = 2;
    } else if (player.down) {
      npc->tgt_x = player.x;
      npc->tgt_y = player.y - (8 * 0x200);
      npc->anim = 1;
    } else {
      npc->anim = 0;

      if (player.dir == 0) {
        npc->tgt_x = player.x + (7 * 0x200);
        npc->tgt_y = player.y - (3 * 0x200);
      } else {
        npc->tgt_x = player.x - (7 * 0x200);
        npc->tgt_y = player.y - (3 * 0x200);
      }
    }
  }

  npc->x += (npc->tgt_x - npc->x) / 2;
  npc->y += (npc->tgt_y - npc->y) / 2;

  if (player.anim % 2) npc->y -= 1 * 0x200;

  if (player.dir == 0)
    npc->rect = &rc_right[npc->anim];
  else
    npc->rect = &rc_left[npc->anim];
}

// Curly's Nemesis
void npc_act_321(npc_t *npc) {
  static const rect_t rc_left[3] = {
    {136, 152, 152, 168},
    {152, 152, 168, 168},
    {168, 152, 184, 168},
  };

  static const rect_t rc_right[3] = {
    {136, 168, 152, 184},
    {152, 168, 168, 184},
    {168, 168, 184, 184},
  };

  int dir = 0;

  if (npc->parent == NULL) return;

  switch (npc->parent->anim) {
    case 0:
      if (player.dir == 0) {
        npc->x = npc->parent->x + (8 * 0x200);
        dir = 2;
      } else {
        npc->x = npc->parent->x - (8 * 0x200);
        dir = 0;
      }

      npc->y = npc->parent->y;
      break;

    case 1:
      if (player.dir == 0)  // Does the same thing whether this is false or true
        npc->x = npc->parent->x;
      else
        npc->x = npc->parent->x;

      dir = 1;
      npc->y = npc->parent->y - (10 * 0x200);
      break;

    case 2:
      if (player.dir == 0)  // Does the same thing whether this is false or true
        npc->x = npc->parent->x;
      else
        npc->x = npc->parent->x;

      dir = 3;
      npc->y = npc->parent->y + (10 * 0x200);
      break;
  }

  npc->anim = npc->parent->anim;

  if (game_flags & GFLAG_INPUT_ENABLED && bullet_count_by_class(43) < 2 && input_trig & IN_FIRE) {
    bullet_spawn(43, npc->parent->x, npc->parent->y, dir);
    caret_spawn(npc->parent->x, npc->parent->y, CARET_SHOOT, DIR_LEFT);
    snd_play_sound(PRIO_NORMAL, 117, FALSE);
  }

  if (player.dir == 0)
    npc->rect = &rc_right[npc->anim];
  else
    npc->rect = &rc_left[npc->anim];
}

// Deleet
void npc_act_322(npc_t *npc) {
  static const rect_t rc[3] = {
    {272, 216, 296, 240},
    {296, 216, 320, 240},
    {160, 216, 184, 240},
  };

  if (npc->act < 2 && npc->life <= 968) {
    npc->act = 2;
    npc->act_wait = 0;
    npc->bits &= ~NPC_SHOOTABLE;
    npc->bits |= NPC_INVULNERABLE;
    snd_play_sound(PRIO_NORMAL, 22, FALSE);
  }

  switch (npc->act) {
    case 0:
      npc->act = 1;

      if (npc->dir == 0)
        npc->y += 8 * 0x200;
      else
        npc->x += 8 * 0x200;
      // Fallthrough
    case 1:
      if (npc->shock)
        ++npc->count1;
      else
        npc->count1 = 0;

      npc->rect = &rc[npc->count1 / 2 % 2];
      break;

    case 2:
      npc->anim = 2;

      switch (npc->act_wait) {
        // Interestingly, this NPC counts down at 50 frames per second,
        // while NPC206 (the Egg Corridor Counter Bomb), counts at 60.
        case 50 * 0:
          npc_spawn(207, npc->x + (4 * 0x200), npc->y, 0, 0, 0, NULL, 0x180);
          break;

        case 50 * 1:
          npc_spawn(207, npc->x + (4 * 0x200), npc->y, 0, 0, 1, NULL, 0x180);
          break;

        case 50 * 2:
          npc_spawn(207, npc->x + (4 * 0x200), npc->y, 0, 0, 2, NULL, 0x180);
          break;

        case 50 * 3:
          npc_spawn(207, npc->x + (4 * 0x200), npc->y, 0, 0, 3, NULL, 0x180);
          break;

        case 50 * 4:
          npc_spawn(207, npc->x + (4 * 0x200), npc->y, 0, 0, 4, NULL, 0x180);
          break;

        case 50 * 5:
          npc->hit.back = 48 * 0x200;
          npc->hit.front = 48 * 0x200;
          npc->hit.top = 48 * 0x200;
          npc->hit.bottom = 48 * 0x200;
          npc->damage = 12;
          snd_play_sound(PRIO_NORMAL, 26, FALSE);
          npc_spawn_death_fx(npc->x, npc->y, 0x6000, 40, 0);
          cam_start_quake_small(10);

          if (npc->dir == 0) {
            stage_delete_tile(npc->x / 0x200 / 0x10, (npc->y - (8 * 0x200)) / 0x200 / 0x10);
            stage_delete_tile(npc->x / 0x200 / 0x10, (npc->y + (8 * 0x200)) / 0x200 / 0x10);
          } else {
            stage_delete_tile((npc->x - (8 * 0x200)) / 0x200 / 0x10, npc->y / 0x200 / 0x10);
            stage_delete_tile((npc->x + (8 * 0x200)) / 0x200 / 0x10, npc->y / 0x200 / 0x10);
          }

          npc->cond |= NPCCOND_KILLED;
          break;
      }

      ++npc->act_wait;
      npc->rect = &rc[2];
      break;
  }
}

// Bute (spinning)
void npc_act_323(npc_t *npc) {
  static const rect_t rc[4] = {
    {216, 32, 232, 56},
    {232, 32, 248, 56},
    {216, 56, 232, 80},
    {232, 56, 248, 80},
  };

  if (++npc->anim_wait > 3) {
    npc->anim_wait = 0;

    if (++npc->anim > 3) npc->anim = 0;
  }

  switch (npc->act) {
    case 0:
      npc->act = 1;

      switch (npc->dir) {
        case 0:
          npc->xvel = -0x600;
          break;

        case 2:
          npc->xvel = 0x600;
          break;

        case 1:
          npc->yvel = -0x600;
          break;

        case 3:
          npc->yvel = 0x600;
          break;
      }
      // Fallthrough
    case 1:
      if (++npc->act_wait == 16) npc->bits &= ~NPC_IGNORE_SOLIDITY;

      npc->x += npc->xvel;
      npc->y += npc->yvel;

      if (npc->flags & 0xFF) npc->act = 10;

      if (npc->act_wait > 20) {
        switch (npc->dir) {
          case 0:
            if (npc->x <= player.x + (32 * 0x200)) npc->act = 10;
            break;

          case 2:
            if (npc->x >= player.x - (32 * 0x200)) npc->act = 10;
            break;

          case 1:
            if (npc->y <= player.y + (32 * 0x200)) npc->act = 10;
            break;

          case 3:
            if (npc->y >= player.y - (32 * 0x200)) npc->act = 10;
            break;
        }
      }

      break;
  }

  if (npc->act == 10) {
    npc->class_num = 309;
    npc->anim = 0;
    npc->act = 11;
    npc->bits |= NPC_SHOOTABLE;
    npc->bits &= ~NPC_IGNORE_SOLIDITY;
    npc->damage = 5;
    npc->view.top = 8 * 0x200;
  }

  npc->rect = &rc[npc->anim];
}

// Bute generator
void npc_act_324(npc_t *npc) {
  switch (npc->act) {
    case 10:
      npc->act = 11;
      npc->act_wait = 0;
      // Fallthrough
    case 11:
      if (++npc->act_wait % 50 == 1) npc_spawn(323, npc->x, npc->y, 0, 0, npc->dir, NULL, 0x100);

      if (npc->act_wait > 351) npc->act = 0;

      break;
  }
}

// Heavy Press lightning
void npc_act_325(npc_t *npc) {
  static const rect_t rc[7] = {
    {240, 96, 272, 128}, {272, 96, 304, 128}, {240, 128, 272, 160}, {240, 0, 256, 96},
    {256, 0, 272, 96},   {272, 0, 288, 96},   {288, 0, 304, 96},
  };

  switch (npc->act) {
    case 0:
      npc->act = 1;
      snd_play_sound(PRIO_NORMAL, 29, FALSE);
      // Fallthrough
    case 1:
      if (++npc->anim_wait > 0) {
        npc->anim_wait = 0;
        ++npc->anim;
      }

      if (npc->anim > 2) npc->anim = 0;

      if (++npc->act_wait > 50) {
        npc->act = 10;
        npc->anim_wait = 0;
        npc->anim = 3;
        npc->damage = 10;
        npc->view.front = 8 * 0x200;
        npc->view.top = 12 * 0x200;
        snd_play_sound(PRIO_NORMAL, 101, FALSE);
        npc_spawn_death_fx(npc->x, npc->y + (84 * 0x200), 0, 3, 0);
      }

      break;

    case 10:
      if (++npc->anim_wait > 2) {
        npc->anim_wait = 0;
        ++npc->anim;
      }

      if (npc->anim > 6) {
        npc->cond = 0;
        return;
      }

      break;
  }

  npc->rect = &rc[npc->anim];
}

// Sue/Itoh becoming human
void npc_act_326(npc_t *npc) {
  switch (npc->act) {
    case 0:
      npc->act = 1;
      npc->y -= 8 * 0x200;
      npc->x += 16 * 0x200;
      npc->anim = 0;
      // Fallthrough
    case 1:
      if (++npc->act_wait > 80) {
        npc->act = 10;
        npc->act_wait = 0;
        break;
      }

      if (npc->dir == 0) {
        if (npc->act_wait == 30) npc->anim = 1;
        if (npc->act_wait == 40) npc->anim = 0;
      } else {
        if (npc->act_wait == 50) npc->anim = 1;
        if (npc->act_wait == 60) npc->anim = 0;
      }

      break;

    case 10:
      if (++npc->act_wait > 50) {
        npc->act = 15;
        npc->anim = 4;

        if (npc->dir == 0)
          npc->act_wait = 0;
        else
          npc->act_wait = -20;

        break;
      }

      if (npc->act_wait / 2 % 2)
        npc->anim = 2;
      else
        npc->anim = 3;

      break;

    case 15:
      if (++npc->act_wait > 40) {
        npc->act_wait = 0;
        npc->act = 20;
      }

      break;

    case 20:
      npc->yvel += 0x40;
      if (npc->yvel > 0x5FF) npc->yvel = 0x5FF;

      npc->y += npc->yvel;

      if (++npc->act_wait > 50) {
        npc->act = 30;
        npc->act_wait = 0;
        npc->anim = 6;

        if (npc->dir == 0)
          npc_spawn(327, npc->x, npc->y - (16 * 0x200), 0, 0, 0, npc, 0x100);
        else
          npc_spawn(327, npc->x, npc->y - (8 * 0x200), 0, 0, 0, npc, 0x100);

        break;
      }

      break;

    case 30:
      if (++npc->act_wait == 30) npc->anim = 7;

      if (npc->act_wait == 40) npc->act = 40;

      break;

    case 40:
      npc->act = 41;
      npc->act_wait = 0;
      npc->anim = 0;
      // Fallthrough
    case 41:
      if (++npc->act_wait == 30) npc->anim = 1;

      if (npc->act_wait == 40) npc->anim = 0;

      break;
  }

  static const rect_t rc_itoh[8] = {
    {0, 128, 16, 152},  {16, 128, 32, 152}, {32, 128, 48, 152},  {48, 128, 64, 152},
    {64, 128, 80, 152}, {80, 128, 96, 152}, {96, 128, 112, 152}, {112, 128, 128, 152},
  };

  static const rect_t rc_sue[8] = {
    {128, 128, 144, 152}, {144, 128, 160, 152}, {160, 128, 176, 152}, {176, 128, 192, 152},
    {192, 128, 208, 152}, {208, 128, 224, 152}, {224, 128, 240, 152}, {32, 152, 48, 176},
  };

  if (npc->dir == 0)
    npc->rect = &rc_itoh[npc->anim];
  else
    npc->rect = &rc_sue[npc->anim];
}

// Sneeze
void npc_act_327(npc_t *npc) {
  static const rect_t rc[2] = {
    {240, 80, 256, 96},
    {256, 80, 272, 96},
  };

  ++npc->act_wait;

  switch (npc->act) {
    case 0:
      if (npc->act_wait < 4) npc->y -= 2 * 0x200;

      if (npc->parent->anim == 7) {
        npc->anim = 1;
        npc->act = 1;
        npc->tgt_x = npc->x;
        npc->tgt_y = npc->y;
      }
      break;

    case 1:
      if (npc->act_wait < 48) {
        npc->x = npc->tgt_x + (m_rand(-1, 1) * 0x200);
        npc->y = npc->tgt_y + (m_rand(-1, 1) * 0x200);
      } else {
        npc->x = npc->tgt_x;
        npc->y = npc->tgt_y;
      }
      break;
  }

  if (npc->act_wait > 70) npc->cond = 0;

  npc->rect = &rc[npc->anim];
}

// Thingy that turns Sue and Itoh into humans for 4 seconds
void npc_act_328(npc_t *npc) {
  static const rect_t rc = {96, 0, 128, 48};
  npc->rect = &rc;
}

// Laboratory fan
void npc_act_329(npc_t *npc) {
  static const rect_t rc[2] = {
    {48, 0, 64, 16},
    {64, 0, 80, 16},
  };

  if (++npc->anim_wait / 2 % 2)
    npc->rect = &rc[0];
  else
    npc->rect = &rc[1];
}

// Rolling
void npc_act_330(npc_t *npc) {
  static const rect_t rc[3] = {
    {144, 136, 160, 152},
    {160, 136, 176, 152},
    {176, 136, 192, 152},
  };

  switch (npc->act) {
    case 0:
      stage_set_tile(npc->x / 0x200 / 0x10, npc->y / 0x200 / 0x10, 0);

      if (npc->dir == 0)
        npc->act = 10;
      else
        npc->act = 30;

      break;

    case 10:
      npc->xvel -= 0x40;
      npc->yvel = 0;

      if (npc->flags & 1) npc->act = 20;

      break;

    case 20:
      npc->xvel = 0;
      npc->yvel -= 0x40;

      if (npc->flags & 2) npc->act = 30;

      break;

    case 30:
      npc->xvel += 0x40;
      npc->yvel = 0;

      if (npc->flags & 4) npc->act = 40;

      break;

    case 40:
      npc->xvel = 0;
      npc->yvel += 0x40;

      if (npc->flags & 8) npc->act = 10;

      break;
  }

  if (npc->xvel < -0x400) npc->xvel = -0x400;
  if (npc->xvel > 0x400) npc->xvel = 0x400;

  if (npc->yvel < -0x400) npc->yvel = -0x400;
  if (npc->yvel > 0x400) npc->yvel = 0x400;

  npc->x += npc->xvel;
  npc->y += npc->yvel;

  if (++npc->anim_wait > 1) {
    npc->anim_wait = 0;
    ++npc->anim;
  }

  if (npc->anim > 2) npc->anim = 0;

  npc->rect = &rc[npc->anim];
}

// Ballos bone projectile
void npc_act_331(npc_t *npc) {
  static const rect_t rc[4] = {
    {288, 80, 304, 96},
    {304, 80, 320, 96},
    {288, 96, 304, 112},
    {304, 96, 320, 112},
  };

  switch (npc->act) {
    case 0:
      npc->act = 1;
      // Fallthrough
    case 1:
      if (npc->flags & 8) {
        npc->yvel = -0x200;
        npc->act = 10;
      }

      break;

    case 10:
      if (npc->flags & 8) {
        npc->cond = 0;
        caret_spawn(npc->x, npc->y, CARET_PROJECTILE_DISSIPATION, DIR_LEFT);
      }

      break;
  }

  npc->yvel += 0x40;
  if (npc->yvel > 0x5FF) npc->yvel = 0x5FF;

  npc->y += npc->yvel;
  npc->x += npc->xvel;

  if (++npc->anim_wait > 3) {
    npc->anim_wait = 0;

    if (npc->dir == 0)
      ++npc->anim;
    else
      --npc->anim;

    if (npc->anim < 0) npc->anim += 4;
    if (npc->anim > 3) npc->anim -= 4;
  }

  npc->rect = &rc[npc->anim];
}

// Ballos shockwave
void npc_act_332(npc_t *npc) {
  static const rect_t rc[3] = {
    {144, 96, 168, 120},
    {168, 96, 192, 120},
    {192, 96, 216, 120},
  };

  int xvel;

  switch (npc->act) {
    case 0:
      snd_play_sound(PRIO_NORMAL, 44, FALSE);
      npc->act = 1;

      if (npc->dir == 0)
        npc->xvel = -0x400;
      else
        npc->xvel = 0x400;
      // Fallthrough
    case 1:
      if (++npc->anim_wait > 1) {
        npc->anim_wait = 0;

        if (++npc->anim > 2) npc->anim = 0;
      }

      if (++npc->act_wait % 6 == 1) {
        if (npc->dir == 0)
          xvel = (m_rand(-0x10, -4) * 0x200) / 8;
        else
          xvel = (m_rand(4, 0x10) * 0x200) / 8;

        npc_spawn(331, npc->x, npc->y, xvel, -0x400, 0, 0, 0x100);

        snd_play_sound(PRIO_NORMAL, 12, FALSE);
      }

      break;
  }

  if (npc->flags & 1) npc->cond = 0;
  if (npc->flags & 4) npc->cond = 0;

  npc->x += npc->xvel;

  npc->rect = &rc[npc->anim];
}

// Ballos lightning
void npc_act_333(npc_t *npc) {
  static const rect_t rc[2] = {
    {80, 120, 104, 144},
    {104, 120, 128, 144},
  };

  switch (npc->act) {
    case 0:
      npc->act = 1;
      npc->tgt_x = npc->x;
      npc->tgt_y = npc->y;
      snd_play_sound(PRIO_NORMAL, 103, FALSE);
      npc->y = player.y;
      // Fallthrough
    case 1:
      if (++npc->act_wait / 2 % 2)
        npc->anim = 0;
      else
        npc->anim = 1;

      if (npc->dir == 0 && npc->act_wait == 20)
        npc_spawn(146, npc->tgt_x, npc->tgt_y, 0, 0, 0, NULL, 0x100);

      if (npc->act_wait > 40) npc->cond = 0;

      break;
  }

  npc->rect = &rc[npc->anim];
}

// Sweat
void npc_act_334(npc_t *npc) {
  static const rect_t rc_left[2] = {
    {160, 184, 168, 200},
    {168, 184, 176, 200},
  };

  static const rect_t rc_right[2] = {
    {176, 184, 184, 200},
    {184, 184, 192, 200},
  };

  switch (npc->act) {
    case 0:
      npc->act = 10;

      if (npc->dir == 0) {
        npc->x += 10 * 0x200;
        npc->y -= 18 * 0x200;
      } else {
        npc->x = player.x - (10 * 0x200);
        npc->y = player.y - (2 * 0x200);
      }
      // Fallthrough
    case 10:
      if (++npc->act_wait / 8 % 2)
        npc->anim = 0;
      else
        npc->anim = 1;

      if (npc->act_wait >= 64) npc->cond = 0;

      break;
  }

  if (npc->dir == 0)
    npc->rect = &rc_left[npc->anim];
  else
    npc->rect = &rc_right[npc->anim];
}

// Ikachan
void npc_act_335(npc_t *npc) {
  static const rect_t rc[3] = {
    {0, 16, 16, 32},
    {16, 16, 32, 32},
    {32, 16, 48, 32},
  };

  switch (npc->act) {
    case 0:
      npc->act = 1;
      npc->act_wait = m_rand(3, 20);
      // Fallthrough
    case 1:
      if (--npc->act_wait <= 0) {
        npc->act = 2;
        npc->act_wait = m_rand(10, 50);
        npc->anim = 1;
        npc->xvel = 0x600;
      }

      break;

    case 2:
      if (--npc->act_wait <= 0) {
        npc->act = 3;
        npc->act_wait = m_rand(40, 50);
        npc->anim = 2;
        npc->yvel = m_rand(-0x100, 0x100);
      }

      break;

    case 3:
      if (--npc->act_wait <= 0) {
        npc->act = 1;
        npc->act_wait = 0;
        npc->anim = 0;
      }

      break;
  }

  npc->xvel -= 0x10;

  npc->x += npc->xvel;
  npc->y += npc->yvel;

  npc->rect = &rc[npc->anim];
}

// Ikachan generator
void npc_act_336(npc_t *npc) {
  int y;

  switch (npc->act) {
    case 0:
      if (player.shock) npc->cond = 0;

      break;

    case 10:
      if (++npc->act_wait % 4 == 1) {
        y = npc->y + (m_rand(0, 13) * 0x200 * 0x10);
        npc_spawn(335, npc->x, y, 0, 0, 0, 0, 0);
      }

      break;
  }
}

// Numhachi
void npc_act_337(npc_t *npc) {
  static const rect_t rc_left[2] = {
    {256, 112, 288, 152},
    {288, 112, 320, 152},
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
      if (++npc->anim_wait > 50) {
        npc->anim_wait = 0;
        ++npc->anim;
      }

      if (npc->anim > 1) npc->anim = 0;

      break;
  }

  npc->yvel += 0x40;
  if (npc->yvel > 0x5FF) npc->yvel = 0x5FF;

  npc->x += npc->xvel;
  npc->y += npc->yvel;

  npc->rect = &rc_left[npc->anim];
}

// Green Devil
void npc_act_338(npc_t *npc) {
  static const rect_t rc_left[2] = {
    {288, 0, 304, 16},
    {304, 0, 320, 16},
  };

  static const rect_t rc_right[2] = {
    {288, 16, 304, 32},
    {304, 16, 320, 32},
  };

  switch (npc->act) {
    case 0:
      npc->act = 1;
      npc->view.top = 8 * 0x200;
      npc->view.bottom = 8 * 0x200;
      npc->damage = 3;
      npc->bits |= NPC_SHOOTABLE;
      npc->tgt_y = npc->y;
      npc->yvel = (m_rand(-10, 10) * 0x200) / 2;
      // Fallthrough
    case 1:
      if (++npc->anim_wait > 2) {
        npc->anim_wait = 0;
        ++npc->anim;
      }

      if (npc->anim > 1) npc->anim = 0;

      if (npc->y < npc->tgt_y)
        npc->yvel += 0x80;
      else
        npc->yvel -= 0x80;

      if (npc->dir == 0)
        npc->xvel -= 0x20;
      else
        npc->xvel += 0x20;

      if (npc->xvel > 0x400) npc->xvel = 0x400;
      if (npc->xvel < -0x400) npc->xvel = -0x400;

      if (npc->x < 0 || npc->y < 0 || npc->x > stage_data->width * 0x200 * 0x10 ||
          npc->y > stage_data->height * 0x200 * 0x10) {
        npc_show_death_damage(npc);
        return;
      }

      break;
  }

  npc->x += npc->xvel;
  npc->y += npc->yvel;

  if (npc->dir == 0)
    npc->rect = &rc_left[npc->anim];
  else
    npc->rect = &rc_right[npc->anim];
}

// Green Devil generator
void npc_act_339(npc_t *npc) {
  switch (npc->act) {
    case 0:
      npc->act = 1;
      npc->act_wait = m_rand(0, 40);
      // Fallthrough
    case 1:
      if (npc->act_wait) {
        --npc->act_wait;
      } else {
        npc->act = 0;
        npc_spawn(338, npc->x, npc->y + (m_rand(-0x10, 0x10) * 0x200), 0, 0, npc->dir, NULL, 0x100);
      }

      break;
  }
}
