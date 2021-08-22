#include "game/npc_act/npc_act.h"

// Toroko (frenzied)
void npc_act_140(npc_t *npc) {
  int i;

  static const rect_t rc_left[14] = {
    {0, 0, 32, 32},     {32, 0, 64, 32},   {64, 0, 96, 32},   {96, 0, 128, 32},
    {128, 0, 160, 32},  {160, 0, 192, 32}, {192, 0, 224, 32}, {224, 0, 256, 32},
    {0, 64, 32, 96},    {32, 64, 64, 96},  {64, 64, 96, 96},  {96, 64, 128, 96},
    {128, 64, 160, 96}, {0, 0, 0, 0},
  };

  static const rect_t rc_right[14] = {
    {0, 32, 32, 64},     {32, 32, 64, 64},   {64, 32, 96, 64},   {96, 32, 128, 64},
    {128, 32, 160, 64},  {160, 32, 192, 64}, {192, 32, 224, 64}, {224, 32, 256, 64},
    {0, 96, 32, 128},    {32, 96, 64, 128},  {64, 96, 96, 128},  {96, 96, 128, 128},
    {128, 96, 160, 128}, {0, 0, 0, 0},
  };

  switch (npc->act) {
    case 0:
      npc->act = 1;
      npc->anim = 9;
      npc->act_wait = 0;
      npc->bits &= ~NPC_INTERACTABLE;
      // Fallthrough
    case 1:
      if (++npc->act_wait > 50) {
        npc->act = 2;
        npc->act_wait = 0;
        npc->anim = 8;
      }

      break;

    case 2:
      if (++npc->anim > 10) npc->anim = 9;

      if (++npc->act_wait > 50) {
        npc->act = 3;
        npc->act_wait = 0;
        npc->anim = 0;
      }

      break;

    case 3:
      if (++npc->act_wait > 50) {
        npc->act = 10;
        npc->bits |= NPC_SHOOTABLE;
      }

      break;

    case 10:
      npc->bits = npc->bits;
      npc->act = 11;
      npc->anim = 0;
      npc->anim_wait = 0;
      npc->act_wait = m_rand(20, 130);
      npc->xvel = 0;
      // Fallthrough
    case 11:
      if (npc->x > player.x)
        npc->dir = 0;
      else
        npc->dir = 2;

      if (++npc->anim_wait > 4) {
        npc->anim_wait = 0;
        ++npc->anim;
      }

      if (npc->anim > 1) npc->anim = 0;

      if (bullet_count_by_arm(6) || bullet_count_by_arm(3) > 3)
        npc->act = 20;

      if (npc->act_wait != 0) {
        --npc->act_wait;
      } else {
        if (m_rand(0, 99) % 2)
          npc->act = 20;
        else
          npc->act = 50;
      }

      break;

    case 20:
      npc->act = 21;
      npc->anim = 2;
      npc->act_wait = 0;
      // Fallthrough
    case 21:
      if (++npc->act_wait > 10) {
        npc->act = 22;
        npc->act_wait = 0;
        npc->anim = 3;
        npc->yvel = -0x5FF;

        if (npc->dir == 0)
          npc->xvel = -0x200;
        else
          npc->xvel = 0x200;
      }

      break;

    case 22:
      if (++npc->act_wait > 10) {
        npc->act = 23;
        npc->act_wait = 0;
        npc->anim = 6;
        npc_spawn(141, 0, 0, 0, 0, 0, npc, 0);
      }

      break;

    case 23:
      if (++npc->act_wait > 30) {
        npc->act = 24;
        npc->act_wait = 0;
        npc->anim = 7;
      }

      if (npc->x > player.x)
        npc->dir = 0;
      else
        npc->dir = 2;

      break;

    case 24:
      if (++npc->act_wait > 3) {
        npc->act = 25;
        npc->anim = 3;
      }

      break;

    case 25:
      if (npc->flags & 8) {
        npc->act = 26;
        npc->act_wait = 0;
        npc->anim = 2;
        snd_play_sound(PRIO_NORMAL, 26, FALSE);
        cam_start_quake_small(20);
      }

      break;

    case 26:
      npc->xvel = (npc->xvel * 8) / 9;

      if (++npc->act_wait > 20) {
        npc->act = 10;
        npc->anim = 0;
      }

      break;

    case 50:
      npc->act = 51;
      npc->act_wait = 0;
      npc->anim = 4;
      npc_spawn(141, 0, 0, 0, 0, 0, npc, 0);
      // Fallthrough
    case 51:
      if (++npc->act_wait > 30) {
        npc->act = 52;
        npc->act_wait = 0;
        npc->anim = 5;
      }

      if (npc->x > player.x)
        npc->dir = 0;
      else
        npc->dir = 2;

      break;

    case 52:
      if (++npc->act_wait > 3) {
        npc->act = 10;
        npc->anim = 0;
      }

      break;

    case 100:
      npc->anim = 3;
      npc->act = 101;
      npc->bits &= ~NPC_SHOOTABLE;
      npc->damage = 0;

      for (i = 0; i < 8; ++i)
        npc_spawn(4, npc->x + (m_rand(-12, 12) * 0x200), npc->y + (m_rand(-12, 12) * 0x200), m_rand(-341, 341),
          m_rand(-0x600, 0), 0, NULL, 0x100);

      break;

    case 101:
      if (npc->flags & 8) {
        npc->act = 102;
        npc->act_wait = 0;
        npc->anim = 2;
        snd_play_sound(PRIO_NORMAL, 26, FALSE);
        cam_start_quake_small(20);
      }

      break;

    case 102:
      npc->xvel = (npc->xvel * 8) / 9;

      if (++npc->act_wait > 50) {
        npc->act = 103;
        npc->act_wait = 0;
        npc->anim = 10;
      }

      break;

    case 103:
      if (++npc->act_wait > 50) {
        npc->anim = 9;
        npc->act = 104;
        npc->act_wait = 0;
      }

      break;

    case 104:
      if (++npc->anim > 10) npc->anim = 9;

      if (++npc->act_wait > 100) {
        npc->act_wait = 0;
        npc->anim = 9;
        npc->act = 105;
      }

      break;

    case 105:
      if (++npc->act_wait > 50) {
        npc->anim_wait = 0;
        npc->act = 106;
        npc->anim = 11;
      }

      break;

    case 106:
      if (++npc->anim_wait > 50) {
        npc->anim_wait = 0;
        ++npc->anim;
      }

      if (npc->anim > 12) npc->anim = 12;

      break;

    case 140:
      npc->act = 141;
      npc->act_wait = 0;
      npc->anim = 12;
      snd_play_sound(PRIO_NORMAL, 29, FALSE);
      // Fallthrough
    case 141:
      if (++npc->anim > 13) npc->anim = 12;

      if (++npc->act_wait > 100) {
        for (i = 0; i < 4; ++i)
          npc_spawn(4, npc->x + (m_rand(-12, 12) * 0x200), npc->y + (m_rand(-12, 12) * 0x200), m_rand(-341, 341),
            m_rand(-0x600, 0), 0, NULL, 0x100);

        npc->cond = 0;
      }

      break;
  }

  if (npc->act > 100 && npc->act < 105 && npc->act_wait % 9 == 0)
    npc_spawn(4, npc->x + (m_rand(-12, 12) * 0x200), npc->y + (m_rand(-12, 12) * 0x200), m_rand(-341, 341),
      m_rand(-0x600, 0), 0, NULL, 0x100);

  npc->yvel += 0x20;

  if (npc->yvel > 0x5FF) npc->yvel = 0x5FF;
  if (npc->yvel < -0x5FF) npc->yvel = -0x5FF;

  npc->x += npc->xvel;
  npc->y += npc->yvel;

  if (npc->dir == 0)
    npc->rect = &rc_left[npc->anim];
  else
    npc->rect = &rc_right[npc->anim];
}

// Toroko block projectile
void npc_act_141(npc_t *npc) {
  int i;
  u8 deg;

  static const rect_t rect[2] = {
    {288, 32, 304, 48},
    {304, 32, 320, 48},
  };

  switch (npc->act) {
    case 0:
      npc->act = 1;
      npc->act_wait = 0;
      // Fallthrough
    case 1:
      if (npc->parent->dir == 0)
        npc->x = npc->parent->x + (10 * 0x200);
      else
        npc->x = npc->parent->x - (10 * 0x200);

      npc->y = npc->parent->y - (8 * 0x200);

      if (npc->parent->act == 24 || npc->parent->act == 52) {
        npc->act = 10;

        if (npc->parent->dir == 0)
          npc->x = npc->parent->x - (16 * 0x200);
        else
          npc->x = npc->parent->x + (16 * 0x200);

        npc->y = npc->parent->y;

        deg = m_atan2(npc->x - player.x, npc->y - player.y);
        npc->yvel = m_sin(deg) * 4;
        npc->xvel = m_cos(deg) * 4;

        snd_play_sound(PRIO_NORMAL, 39, FALSE);
      }

      break;

    case 10:
      if (npc->flags & 0xF) {
        npc->act = 20;
        npc->act_wait = 0;
        caret_spawn(npc->x, npc->y, CARET_PROJECTILE_DISSIPATION, DIR_LEFT);
        snd_play_sound(PRIO_NORMAL, 12, FALSE);

        for (i = 0; i < 4; ++i)
          npc_spawn(4, npc->x, npc->y, m_rand(-0x200, 0x200), m_rand(-0x200, 0x200), 0, NULL, 0x100);

        break;
      }

      npc->x += npc->xvel;
      npc->y += npc->yvel;

      break;

    case 20:
      npc->x += npc->xvel;
      npc->y += npc->yvel;

      if (++npc->act_wait > 4) {
        for (i = 0; i < 4; ++i)
          npc_spawn(4, npc->x, npc->y, m_rand(-0x200, 0x200), m_rand(-0x200, 0x200), 0, NULL, 0x100);

        npc->class_num = 142;
        npc->anim = 0;
        npc->act = 20;
        npc->xvel = 0;
        npc->bits &= ~NPC_INVULNERABLE;
        npc->bits |= NPC_SHOOTABLE;
        npc->damage = 1;
      }

      break;
  }

  if (++npc->anim > 1) npc->anim = 0;

  npc->rect = &rect[npc->anim];
}

// Flower Cub
void npc_act_142(npc_t *npc) {
  static const rect_t rect[5] = {
    {0, 128, 16, 144}, {16, 128, 32, 144}, {32, 128, 48, 144},
    {48, 128, 64, 144}, {64, 128, 80, 144},
  };

  switch (npc->act) {
    case 10:
      npc->act = 11;
      npc->anim = 0;
      npc->act_wait = 0;
      // Fallthrough
    case 11:
      if (++npc->act_wait > 30) {
        npc->act = 12;
        npc->anim = 1;
        npc->anim_wait = 0;
      }

      break;

    case 12:
      if (++npc->anim_wait > 8) {
        npc->anim_wait = 0;
        ++npc->anim;
      }

      if (npc->anim == 3) {
        npc->act = 20;
        npc->yvel = -0x200;

        if (player.x < npc->x)
          npc->xvel = -0x200;
        else
          npc->xvel = 0x200;
      }

      break;

    case 20:
      if (npc->yvel > -0x80)
        npc->anim = 4;
      else
        npc->anim = 3;

      if (npc->flags & 8) {
        npc->anim = 2;
        npc->act = 21;
        npc->act_wait = 0;
        npc->xvel = 0;
        snd_play_sound(PRIO_NORMAL, 23, FALSE);
      }

      break;

    case 21:
      if (++npc->act_wait > 10) {
        npc->act = 10;
        npc->anim = 0;
      }

      break;
  }

  npc->yvel += 0x40;

  if (npc->yvel > 0x5FF) npc->yvel = 0x5FF;
  if (npc->yvel < -0x5FF) npc->yvel = -0x5FF;

  npc->x += npc->xvel;
  npc->y += npc->yvel;

  npc->rect = &rect[npc->anim];
}

// Jenka (collapsed)
void npc_act_143(npc_t *npc) {
  static const rect_t rc_left[1] = {{208, 32, 224, 48}};

  static const rect_t rc_right[1] = {{208, 48, 224, 64}};

  if (npc->dir == 0)
    npc->rect = &rc_left[npc->anim];
  else
    npc->rect = &rc_right[npc->anim];
}

// Toroko (teleporting in)
void npc_act_144(npc_t *npc) {
  static const rect_t rc_left[5] = {
    {0, 64, 16, 80}, {16, 64, 32, 80}, {32, 64, 48, 80},
    {16, 64, 32, 80}, {128, 64, 144, 80},
  };

  static const rect_t rc_right[5] = {
    {0, 80, 16, 96}, {16, 80, 32, 96}, {32, 80, 48, 96},
    {16, 80, 32, 96}, {128, 80, 144, 96},
  };

  switch (npc->act) {
    case 0:
      npc->act = 1;
      npc->anim = 0;
      npc->anim_wait = 0;
      npc->tgt_x = npc->x;
      snd_play_sound(PRIO_NORMAL, 29, FALSE);
      // Fallthrough
    case 1:
      if (++npc->act_wait == 64) {
        npc->act = 2;
        npc->act_wait = 0;
      }

      break;

    case 2:
      if (++npc->anim_wait > 2) {
        npc->anim_wait = 0;
        ++npc->anim;
      }

      if (npc->anim > 3) npc->anim = 2;

      if (npc->flags & 8) {
        npc->act = 4;
        npc->act_wait = 0;
        npc->anim = 4;
        snd_play_sound(PRIO_NORMAL, 23, FALSE);
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
  }

  if (npc->act > 1) {
    npc->yvel += 0x20;
    if (npc->yvel > 0x5FF) npc->yvel = 0x5FF;

    npc->y += npc->yvel;
  }

  if (npc->dir == 0)
    npc->rect = &rc_left[npc->anim];
  else
    npc->rect = &rc_right[npc->anim];

  if (npc->act == 1) {
    npc->rect_delta.bottom = npc->act_wait / 4;
    npc->rect_prev = NULL; // force texrect update
    if (npc->act_wait / 2 % 2)
      npc->x = npc->tgt_x;
    else
      npc->x = npc->tgt_x + (1 * 0x200);
  }
}

// King's sword
void npc_act_145(npc_t *npc) {
  static const rect_t rc_left[1] = {{96, 32, 112, 48}};
  static const rect_t rc_right[1] = {{112, 32, 128, 48}};

  switch (npc->act) {
    case 0:
      if (npc->parent->count2 == 0) {
        if (npc->parent->dir == 0)
          npc->dir = 0;
        else
          npc->dir = 2;
      } else {
        if (npc->parent->dir == 0)
          npc->dir = 2;
        else
          npc->dir = 0;
      }

      if (npc->dir == 0)
        npc->x = npc->parent->x - (10 * 0x200);
      else
        npc->x = npc->parent->x + (10 * 0x200);

      npc->y = npc->parent->y;

      break;
  }

  if (npc->dir == 0)
    npc->rect = &rc_left[npc->anim];
  else
    npc->rect = &rc_right[npc->anim];
}

// Lightning
void npc_act_146(npc_t *npc) {
  static const rect_t rect[5] = {
      {0, 0, 0, 0}, {256, 0, 272, 240}, {272, 0, 288, 240},
      {288, 0, 304, 240}, {304, 0, 320, 240},
  };

  switch (npc->act) {
    case 0:
      npc->act = 1;

      if (npc->dir == 2)
        cam_start_flash(0, 0, FLASH_MODE_FLASH);
        // Fallthrough
      case 1:
        if (++npc->act_wait > 10) {
          npc->act = 2;
          snd_play_sound(PRIO_NORMAL, 101, FALSE);
        }

      break;

    case 2:
      if (++npc->anim_wait > 2) {
        npc->anim_wait = 0;
        ++npc->anim;
      }

      if (npc->anim == 2) npc->damage = 10;

      if (npc->anim > 4) {
        npc_spawn_death_fx(npc->x, npc->y, 0x1000, 8, FALSE);
        npc->cond = 0;
        return;
      }

      break;
  }

  npc->rect = &rect[npc->anim];
}

// Critter (purple)
void npc_act_147(npc_t *npc) {
  int xvel, yvel;
  u8 deg;

  static const rect_t rc_left[6] = {
    {0, 96, 16, 112},  {16, 96, 32, 112}, {32, 96, 48, 112},
    {48, 96, 64, 112}, {64, 96, 80, 112}, {80, 96, 96, 112},
  };

  static const rect_t rc_right[6] = {
    {0, 112, 16, 128},  {16, 112, 32, 128}, {32, 112, 48, 128},
    {48, 112, 64, 128}, {64, 112, 80, 128}, {80, 112, 96, 128},
  };

  switch (npc->act) {
    case 0:
      npc->y += 3 * 0x200;
      npc->act = 1;
      // Fallthrough
    case 1:
      if (npc->act_wait >= 8 && npc->x - (96 * 0x200) < player.x && npc->x + (96 * 0x200) > player.x &&
          npc->y - (96 * 0x200) < player.y && npc->y + (32 * 0x200) > player.y) {
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

      if (npc->act_wait >= 8 && npc->x - (48 * 0x200) < player.x && npc->x + (48 * 0x200) > player.x &&
          npc->y - (96 * 0x200) < player.y && npc->y + (32 * 0x200) > player.y) {
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
        snd_play_sound(PRIO_NORMAL, 30, FALSE);

        if (npc->x > player.x)
          npc->dir = 0;
        else
          npc->dir = 2;
      }

      break;

    case 3:
      if (npc->yvel > 0x100) {
        npc->tgt_y = npc->y;
        npc->act = 4;
        npc->anim = 3;
        npc->act_wait = 0;
        npc->act_wait = 0;  // Duplicate line
      }

      break;

    case 4:
      if (npc->x < player.x)
        npc->dir = 2;
      else
        npc->dir = 0;

      ++npc->act_wait;

      if (npc->flags & 7 || npc->act_wait > 60) {
        npc->damage = 3;
        npc->act = 5;
        npc->anim = 2;
        break;
      }

      if (npc->act_wait % 4 == 1) snd_play_sound(PRIO_NORMAL, 109, FALSE);

      if (npc->flags & 8) npc->yvel = -0x200;

      if (npc->act_wait % 30 == 6) {
        deg = m_atan2(npc->x - player.x, npc->y - player.y);
        deg += (u8)m_rand(-6, 6);
        yvel = m_sin(deg) * 3;
        xvel = m_cos(deg) * 3;

        npc_spawn(148, npc->x, npc->y, xvel, yvel, 0, NULL, 0x100);
        snd_play_sound(PRIO_NORMAL, 39, FALSE);
      }

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
        snd_play_sound(PRIO_NORMAL, 23, FALSE);
      }

      break;
  }

  if (npc->act != 4) {
    npc->yvel += 0x20;
    if (npc->yvel > 0x5FF) npc->yvel = 0x5FF;
  } else {
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

// Purple Critter's projectile
void npc_act_148(npc_t *npc) {
  if (npc->flags & 0xFF) {
    caret_spawn(npc->x, npc->y, CARET_PROJECTILE_DISSIPATION, DIR_LEFT);
    npc->cond = 0;
  }

  npc->y += npc->yvel;
  npc->x += npc->xvel;

  static const rect_t rect_left[2] = {
    {96, 96, 104, 104},
    {104, 96, 112, 104},
  };

  if (++npc->anim > 1) npc->anim = 0;

  npc->rect = &rect_left[npc->anim];

  if (++npc->count1 > 300) {
    caret_spawn(npc->x, npc->y, CARET_PROJECTILE_DISSIPATION, DIR_LEFT);
    npc->cond = 0;
  }
}

// Moving block (horizontal)
void npc_act_149(npc_t *npc) {
  int i;

  switch (npc->act) {
    case 0:
      npc->x += 8 * 0x200;
      npc->y += 8 * 0x200;

      if (npc->dir == 0)
        npc->act = 10;
      else
        npc->act = 20;

      npc->xvel = 0;
      npc->yvel = 0;

      npc->bits |= NPC_SOLID_HARD;
      break;

    case 10:
      npc->bits &= ~NPC_REAR_AND_TOP_DONT_HURT;
      npc->damage = 0;

      if (player.x < npc->x + (25 * 0x200) && player.x > npc->x - (25 * 0x10 * 0x200) &&
          player.y < npc->y + (25 * 0x200) && player.y > npc->y - (25 * 0x200)) {
        npc->act = 11;
        npc->act_wait = 0;
      }

      break;

    case 11:
      if (++npc->act_wait % 10 == 6) snd_play_sound(PRIO_NORMAL, 107, FALSE);

      if (npc->flags & 1) {
        npc->xvel = 0;
        npc->dir = 2;
        npc->act = 20;
        cam_start_quake_small(10);
        snd_play_sound(PRIO_NORMAL, 26, FALSE);

        for (i = 0; i < 4; ++i)
          npc_spawn(4, npc->x - (16 * 0x200), npc->y + (m_rand(-12, 12) * 0x200), m_rand(-341, 341), m_rand(-0x600, 0),
            0, NULL, 0x100);

        break;
      }

      if (player.flags & 1) {
        npc->bits |= NPC_REAR_AND_TOP_DONT_HURT;
        npc->damage = 100;
      } else {
        npc->bits &= ~NPC_REAR_AND_TOP_DONT_HURT;
        npc->damage = 0;
      }

      npc->xvel -= 0x20;

      break;

    case 20:
      npc->bits &= ~NPC_REAR_AND_TOP_DONT_HURT;
      npc->damage = 0;

      if (player.x > npc->x - (25 * 0x200) && player.x < npc->x + (25 * 0x10 * 0x200) &&
          player.y < npc->y + (25 * 0x200) && player.y > npc->y - (25 * 0x200)) {
        npc->act = 21;
        npc->act_wait = 0;
      }

      break;

    case 21:
      if (++npc->act_wait % 10 == 6) snd_play_sound(PRIO_NORMAL, 107, FALSE);

      if (npc->flags & 4) {
        npc->xvel = 0;
        npc->dir = 0;
        npc->act = 10;
        cam_start_quake_small(10);
        snd_play_sound(PRIO_NORMAL, 26, FALSE);

        for (i = 0; i < 4; ++i)
          npc_spawn(4, npc->x + (16 * 0x200), npc->y + (m_rand(-12, 12) * 0x200), m_rand(-341, 341), m_rand(-0x600, 0),
            0, NULL, 0x100);

        break;
      }

      if (player.flags & 4) {
        npc->bits |= NPC_REAR_AND_TOP_DONT_HURT;
        npc->damage = 100;
      } else {
        npc->bits &= ~NPC_REAR_AND_TOP_DONT_HURT;
        npc->damage = 0;
      }

      npc->xvel += 0x20;

      break;
  }

  if (npc->xvel > 0x200) npc->xvel = 0x200;
  if (npc->xvel < -0x200) npc->xvel = -0x200;

  npc->x += npc->xvel;

  static const rect_t rect = {16, 0, 48, 32};
  npc->rect = &rect;
}

// Quote
void npc_act_150(npc_t *npc) {
  int i;

  static const rect_t rc_left[9] = {
    {0, 0, 16, 16},  {48, 0, 64, 16}, {144, 0, 160, 16}, {16, 0, 32, 16},   {0, 0, 16, 16},
    {32, 0, 48, 16}, {0, 0, 16, 16},  {160, 0, 176, 16}, {112, 0, 128, 16},
  };

  static const rect_t rc_right[9] = {
    {0, 16, 16, 32},  {48, 16, 64, 32}, {144, 16, 160, 32}, {16, 16, 32, 32},   {0, 16, 16, 32},
    {32, 16, 48, 32}, {0, 16, 16, 32},  {160, 16, 176, 32}, {112, 16, 128, 32},
  };

  switch (npc->act) {
    case 0:
      npc->act = 1;
      npc->anim = 0;

      if (npc->dir > 10) {
        npc->x = player.x;
        npc->y = player.y;
        npc->dir -= 10;
      }
      break;

    case 2:
      npc->anim = 1;
      break;

    case 10:
      npc->act = 11;

      for (i = 0; i < 4; ++i)
        npc_spawn(4, npc->x, npc->y, m_rand(-0x155, 0x155), m_rand(-0x600, 0), 0, NULL, 0x100);

      snd_play_sound(PRIO_NORMAL, 71, FALSE);
      // Fallthrough
    case 11:
      npc->anim = 2;
      break;

    case 20:
      npc->act = 21;
      npc->act_wait = 64;
      snd_play_sound(PRIO_NORMAL, 29, FALSE);
      // Fallthrough
    case 21:
      if (--npc->act_wait == 0) npc->cond = 0;

      break;

    case 50:
      npc->act = 51;
      npc->anim = 3;
      npc->anim_wait = 0;
      // Fallthrough
    case 51:
      if (++npc->anim_wait > 4) {
        npc->anim_wait = 0;
        ++npc->anim;
      }

      if (npc->anim > 6) npc->anim = 3;

      if (npc->dir == 0)
        npc->x -= 1 * 0x200;
      else
        npc->x += 1 * 0x200;

      break;

    case 60:
      npc->act = 61;
      npc->anim = 7;
      npc->tgt_x = npc->x;
      npc->tgt_y = npc->y;
      // Fallthrough
    case 61:
      npc->tgt_y += 0x100;
      npc->x = npc->tgt_x + (m_rand(-1, 1) * 0x200);
      npc->y = npc->tgt_y + (m_rand(-1, 1) * 0x200);
      break;

    case 70:
      npc->act = 71;
      npc->act_wait = 0;
      npc->anim = 3;
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

      if (npc->anim > 6) npc->anim = 3;

      break;

    case 80:
      npc->anim = 8;
      break;

    case 99:
    case 100:
      npc->act = 101;
      npc->anim = 3;
      npc->anim_wait = 0;
      // Fallthrough
    case 101:
      npc->yvel += 0x40;

      if (npc->yvel > 0x5FF) npc->yvel = 0x5FF;

      if (npc->flags & 8) {
        npc->yvel = 0;
        npc->act = 102;
      }

      npc->y += npc->yvel;
      break;

    case 102:
      if (++npc->anim_wait > 8) {
        npc->anim_wait = 0;
        ++npc->anim;
      }

      if (npc->anim > 6) npc->anim = 3;

      break;
  }

  if (npc->dir == 0)
    npc->rect = &rc_left[npc->anim];
  else
    npc->rect = &rc_right[npc->anim];

  if (npc->act == 21) {
    npc->rect_prev = NULL; // force texrect update
    npc->rect_delta.bottom = (npc->act_wait / 4);
    if (npc->act_wait / 2 % 2) npc->rect_delta.left = 1;
  }

  // Use a different sprite if the player is wearing the Mimiga Mask
  if (player.equip & EQUIP_MIMIGA_MASK) {
    npc->rect_delta.top = 32;
    npc->rect_delta.bottom = 32;
    npc->rect_prev = NULL; // force texrect update
  }
}

// Blue robot (standing)
void npc_act_151(npc_t *npc) {
  static const rect_t rc_left[2] = {
    {192, 0, 208, 16},
    {208, 0, 224, 16},
  };

  static const rect_t rc_right[2] = {
    {192, 16, 208, 32},
    {208, 16, 224, 32},
  };

  switch (npc->act) {
    case 0:
      npc->act = 1;
      npc->anim = 0;
      npc->anim_wait = 0;
      // Fallthrough

    case 1:
      if (m_rand(0, 100) == 0) {
        npc->act = 2;
        npc->act_wait = 0;
        npc->anim = 1;
      }

      break;

    case 2:
      if (++npc->act_wait > 16) {
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

// Shutter stuck
void npc_act_152(npc_t *npc) {
  static const rect_t rc = {0, 0, 0, 0};

  switch (npc->act) {
    case 0:
      if (npc->dir == 2) npc->y += 16 * 0x200;

      npc->act = 1;
      break;
  }

  npc->rect = &rc;
}

static const rect_t rc_kit_left[21] = {
  {0, 0, 24, 24},    {24, 0, 48, 24},   {48, 0, 72, 24},   {0, 0, 24, 24},
  {72, 0, 96, 24},   {0, 0, 24, 24},    {96, 0, 120, 24},  {120, 0, 144, 24},
  {144, 0, 168, 24}, {168, 0, 192, 24}, {192, 0, 216, 24}, {216, 0, 240, 24},
  {240, 0, 264, 24}, {264, 0, 288, 24}, {0, 48, 24, 72},   {24, 48, 48, 72},
  {48, 48, 72, 72},  {72, 48, 96, 72},  {288, 0, 312, 24}, {24, 48, 48, 72},
  {96, 48, 120, 72}
};

static const rect_t rc_kit_right[21] = {
  {0, 24, 24, 48},    {24, 24, 48, 48},   {48, 24, 72, 48},   {0, 24, 24, 48},
  {72, 24, 96, 48},   {0, 24, 24, 48},    {96, 24, 120, 48},  {120, 24, 144, 48},
  {144, 24, 168, 48}, {168, 24, 192, 48}, {192, 24, 216, 48}, {216, 24, 240, 48},
  {240, 24, 264, 48}, {264, 24, 288, 48}, {0, 72, 24, 96},    {24, 72, 48, 96},
  {48, 72, 72, 96},   {72, 72, 96, 96},   {288, 24, 312, 48}, {24, 72, 48, 96},
  {96, 72, 120, 96}
};

// Gaudi
void npc_act_153(npc_t *npc) {
  if (npc->x > player.x + (((VID_WIDTH / 2) + 160) * 0x200) || npc->x < player.x - (((VID_WIDTH / 2) + 160) * 0x200) ||
      npc->y > player.y + (((VID_HEIGHT / 2) + 120) * 0x200) || npc->y < player.y - (((VID_HEIGHT / 2) + 120) * 0x200))
    return;

  switch (npc->act) {
    case 0:
      npc->act = 1;
      npc->xvel = 0;
      npc->anim = 0;
      npc->y += 3 * 0x200;
      // Fallthrough
    case 1:
      if (m_rand(0, 100) == 1) {
        npc->act = 2;
        npc->anim = 1;
        npc->act_wait = 0;
      }

      if (m_rand(0, 100) == 1) {
        if (npc->dir == 0)
          npc->dir = 2;
        else
          npc->dir = 0;
      }

      if (m_rand(0, 100) == 1) npc->act = 10;

      break;

    case 2:
      if (++npc->act_wait > 20) {
        npc->act = 1;
        npc->anim = 0;
      }

      break;

    case 10:
      npc->act = 11;
      npc->act_wait = m_rand(25, 100);
      npc->anim = 2;
      npc->anim_wait = 0;
      // Fallthrough
    case 11:
      if (++npc->anim_wait > 3) {
        npc->anim_wait = 0;
        ++npc->anim;
      }

      if (npc->anim > 5) npc->anim = 2;

      if (npc->dir == 0)
        npc->xvel = -0x200;
      else
        npc->xvel = 0x200;

      if (npc->act_wait != 0) {
        --npc->act_wait;
      } else {
        npc->act = 1;
        npc->anim = 0;
        npc->xvel = 0;
      }

      if (npc->dir == 0 && npc->flags & 1) {
        npc->anim = 2;
        npc->yvel = -0x5FF;
        npc->act = 20;

        if (!(player.cond & PLRCOND_INVISIBLE)) snd_play_sound(PRIO_NORMAL, 30, FALSE);
      } else if (npc->dir == 2 && npc->flags & 4) {
        npc->anim = 2;
        npc->yvel = -0x5FF;
        npc->act = 20;

        if (!(player.cond & PLRCOND_INVISIBLE)) snd_play_sound(PRIO_NORMAL, 30, FALSE);
      }

      break;

    case 20:
      if (npc->dir == 0 && npc->flags & 1)
        ++npc->count1;
      else if (npc->dir == 2 && npc->flags & 4)
        ++npc->count1;
      else
        npc->count1 = 0;

      if (npc->count1 > 10) {
        if (npc->dir == 0)
          npc->dir = 2;
        else
          npc->dir = 0;
      }

      if (npc->dir == 0)
        npc->xvel = -0x100;
      else
        npc->xvel = 0x100;

      if (npc->flags & 8) {
        npc->act = 21;
        npc->anim = 20;
        npc->act_wait = 0;
        npc->xvel = 0;

        if (!(player.cond & PLRCOND_INVISIBLE)) snd_play_sound(PRIO_NORMAL, 23, FALSE);
      }

      break;

    case 21:
      if (++npc->act_wait > 10) {
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
    npc->rect = &rc_kit_left[npc->anim];
  else
    npc->rect = &rc_kit_right[npc->anim];

  if (npc->life <= 985) {
    npc->class_num = 154;
    npc->act = 0;
  }
}

// Gaudi (dead)
void npc_act_154(npc_t *npc) {
  switch (npc->act) {
    case 0:
      npc->bits &= ~NPC_SHOOTABLE;
      npc->bits &= ~NPC_IGNORE_SOLIDITY;
      npc->damage = 0;
      npc->act = 1;
      npc->anim = 9;
      npc->yvel = -0x200;

      if (npc->dir == 0)
        npc->xvel = 0x100;
      else
        npc->xvel = -0x100;

      snd_play_sound(PRIO_NORMAL, 53, FALSE);
      break;

    case 1:
      if (npc->flags & 8) {
        npc->anim = 10;
        npc->anim_wait = 0;
        npc->act = 2;
        npc->act_wait = 0;
      }

      break;

    case 2:
      npc->xvel = (npc->xvel * 8) / 9;

      if (++npc->anim_wait > 3) {
        npc->anim_wait = 0;
        ++npc->anim;
      }

      if (npc->anim > 11) npc->anim = 10;

      if (++npc->act_wait > 50) npc->cond |= NPCCOND_KILLED;

      break;
  }

  npc->yvel += 0x20;
  if (npc->yvel > 0x5FF) npc->yvel = 0x5FF;

  npc->x += npc->xvel;
  npc->y += npc->yvel;

  if (npc->dir == 0)
    npc->rect = &rc_kit_left[npc->anim];
  else
    npc->rect = &rc_kit_right[npc->anim];
}

// Gaudi (flying)
void npc_act_155(npc_t *npc) {
  u8 deg;
  int xvel, yvel;

  if (npc->x > player.x + (((VID_WIDTH / 2) + 160) * 0x200) || npc->x < player.x - (((VID_WIDTH / 2) + 160) * 0x200) ||
      npc->y > player.y + (((VID_HEIGHT / 2) + 120) * 0x200) || npc->y < player.y - (((VID_HEIGHT / 2) + 120) * 0x200))
    return;

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
      npc->act_wait = m_rand(70, 150);
      npc->anim = 14;
      // Fallthrough
    case 1:
      if (++npc->anim > 15) npc->anim = 14;

      if (npc->act_wait != 0) {
        --npc->act_wait;
      } else {
        npc->act = 2;
        npc->anim = 18;
      }

      break;

    case 2:
      if (++npc->anim > 19) npc->anim = 18;

      if (++npc->act_wait > 30) {
        deg = m_atan2(npc->x - player.x, npc->y - player.y);
        deg += (u8)m_rand(-6, 6);
        yvel = m_sin(deg) * 3;
        xvel = m_cos(deg) * 3;
        npc_spawn(156, npc->x, npc->y, xvel, yvel, 0, NULL, 0x100);

        if (!(player.cond & PLRCOND_INVISIBLE)) snd_play_sound(PRIO_NORMAL, 39, FALSE);

        npc->act = 1;
        npc->act_wait = m_rand(70, 150);
        npc->anim = 14;
        npc->anim_wait = 0;
      }

      break;
  }

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

  npc->x += npc->xvel;
  npc->y += npc->yvel;

  if (npc->dir == 0)
    npc->rect = &rc_kit_left[npc->anim];
  else
    npc->rect = &rc_kit_right[npc->anim];

  if (npc->life <= 985) {
    npc->class_num = 154;
    npc->act = 0;
  }
}

// Gaudi projectile
void npc_act_156(npc_t *npc) {
  if (npc->flags & 0xFF) {
    caret_spawn(npc->x, npc->y, CARET_PROJECTILE_DISSIPATION, DIR_LEFT);
    npc->cond = 0;
  }

  npc->y += npc->yvel;
  npc->x += npc->xvel;

  static const rect_t rect_left[3] = {
      {96, 112, 112, 128},
      {112, 112, 128, 128},
      {128, 112, 144, 128},
  };

  if (++npc->anim > 2) npc->anim = 0;

  npc->rect = &rect_left[npc->anim];

  if (++npc->count1 > 300) {
    caret_spawn(npc->x, npc->y, CARET_PROJECTILE_DISSIPATION, DIR_LEFT);
    npc->cond = 0;
  }
}

// Moving block (vertical)
void npc_act_157(npc_t *npc) {
  int i;

  switch (npc->act) {
    case 0:
      npc->x += 8 * 0x200;
      npc->y += 8 * 0x200;

      if (npc->dir == 0)
        npc->act = 10;
      else
        npc->act = 20;

      npc->xvel = 0;
      npc->yvel = 0;
      npc->bits |= NPC_SOLID_HARD;

      break;

    case 10:
      npc->bits &= ~NPC_REAR_AND_TOP_DONT_HURT;
      npc->damage = 0;

      if (player.y < npc->y + (25 * 0x200) && player.y > npc->y - (25 * 0x10 * 0x200) &&
          player.x < npc->x + (25 * 0x200) && player.x > npc->x - (25 * 0x200)) {
        npc->act = 11;
        npc->act_wait = 0;
      }

      break;

    case 11:
      if (++npc->act_wait % 10 == 6) snd_play_sound(PRIO_NORMAL, 107, FALSE);

      if (npc->flags & 2) {
        npc->yvel = 0;
        npc->dir = 2;
        npc->act = 20;
        cam_start_quake_small(10);
        snd_play_sound(PRIO_NORMAL, 26, FALSE);

        for (i = 0; i < 4; ++i)
          npc_spawn(4, npc->x + (m_rand(-12, 12) * 0x200), npc->y - (16 * 0x200), m_rand(-341, 341), m_rand(-0x600, 0),
                    0, NULL, 0x100);

        break;
      }

      if (player.flags & 2) {
        npc->bits |= NPC_REAR_AND_TOP_DONT_HURT;
        npc->damage = 100;
      } else {
        npc->bits &= ~NPC_REAR_AND_TOP_DONT_HURT;
        npc->damage = 0;
      }

      npc->yvel -= 0x20;

      break;

    case 20:
      npc->bits &= ~NPC_REAR_AND_TOP_DONT_HURT;
      npc->damage = 0;

      if (player.y > npc->y - (25 * 0x200) && player.y < npc->y + (25 * 0x10 * 0x200) &&
          player.x < npc->x + (25 * 0x200) && player.x > npc->x - (25 * 0x200)) {
        npc->act = 21;
        npc->act_wait = 0;
      }

      break;

    case 21:
      if (++npc->act_wait % 10 == 6) snd_play_sound(PRIO_NORMAL, 107, FALSE);

      if (npc->flags & 8) {
        npc->yvel = 0;
        npc->dir = 0;
        npc->act = 10;
        cam_start_quake_small(10);
        snd_play_sound(PRIO_NORMAL, 26, FALSE);

        for (i = 0; i < 4; ++i)
          npc_spawn(4, npc->x + (m_rand(-12, 12) * 0x200), npc->y + (16 * 0x200), m_rand(-341, 341), m_rand(-0x600, 0),
                    0, NULL, 0x100);

        break;
      }

      if (player.flags & 8) {
        npc->bits |= NPC_REAR_AND_TOP_DONT_HURT;
        npc->damage = 100;
      } else {
        npc->bits &= ~NPC_REAR_AND_TOP_DONT_HURT;
        npc->damage = 0;
      }

      npc->yvel += 0x20;

      break;
  }

  if (npc->yvel > 0x200) npc->yvel = 0x200;
  if (npc->yvel < -0x200) npc->yvel = -0x200;

  npc->y += npc->yvel;

  static const rect_t rect = {16, 0, 48, 32};
  npc->rect = &rect;
}

// Fish Missile
void npc_act_158(npc_t *npc) {
  int dir;

  static const rect_t rect[8] = {
    {0, 224, 16, 240},  {16, 224, 32, 240}, {32, 224, 48, 240},  {48, 224, 64, 240},
    {64, 224, 80, 240}, {80, 224, 96, 240}, {96, 224, 112, 240}, {112, 224, 128, 240},
  };

  switch (npc->act) {
    case 0:
      npc->act = 1;

      switch (npc->dir) {
        case 0:
          npc->count1 = 0xA0;
          break;

        case 1:
          npc->count1 = 0xE0;
          break;

        case 2:
          npc->count1 = 0x20;
          break;

        case 3:
          npc->count1 = 0x60;
          break;
      }
      // Fallthrough
    case 1:
      npc->xvel = 2 * m_cos(npc->count1);
      npc->yvel = 2 * m_sin(npc->count1);
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

// Monster X (defeated)
void npc_act_159(npc_t *npc) {
  int i;

  static const rect_t rect = {144, 128, 192, 200};

  switch (npc->act) {
    case 0:
      npc->act = 1;

      for (i = 0; i < 8; ++i)
        npc_spawn(4, npc->x + (m_rand(-16, 16) * 0x200), npc->y + (m_rand(-16, 16) * 0x200), m_rand(-341, 341),
          m_rand(-341, 341), 0, NULL, 0x100);
      // Fallthrough
    case 1:
      if (++npc->act_wait > 50) {
        npc->act = 2;
        npc->xvel = -0x100;
      }

      if (npc->act_wait / 2 % 2)
        npc->x += 0x200;
      else
        npc->x -= 0x200;

      break;

    case 2:
      ++npc->act_wait;
      npc->yvel += 0x40;

      if (npc->y > 40 * 0x10 * 0x200) npc->cond = 0;

      break;
  }

  npc->y += npc->yvel;
  npc->x += npc->xvel;

  npc->rect = &rect;

  if (npc->act_wait % 8 == 1)
    npc_spawn(4, npc->x + (m_rand(-16, 16) * 0x200), npc->y + (m_rand(-16, 16) * 0x200), m_rand(-341, 341),
      m_rand(-341, 341), 0, NULL, 0x100);
}
