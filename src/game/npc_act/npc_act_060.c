#include "game/npc_act/npc_act.h"

// Toroko
void npc_act_060(npc_t *npc) {
  static const rect_t rc_left[8] = {
    {0, 64, 16, 80},  {16, 64, 32, 80}, {32, 64, 48, 80},   {16, 64, 32, 80},
    {48, 64, 64, 80}, {16, 64, 32, 80}, {112, 64, 128, 80}, {128, 64, 144, 80},
  };

  static const rect_t rc_right[8] = {
    {0, 80, 16, 96},  {16, 80, 32, 96}, {32, 80, 48, 96},   {16, 80, 32, 96},
    {48, 80, 64, 96}, {16, 80, 32, 96}, {112, 80, 128, 96}, {128, 80, 144, 96},
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

      if (npc->x - (16 * 0x200) < player.x && npc->x + (16 * 0x200) > player.x && npc->y - (16 * 0x200) < player.y &&
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
      npc->anim = 1;
      npc->anim_wait = 0;
      // Fallthrough
    case 4:
      if (++npc->anim_wait > 2) {
        npc->anim_wait = 0;
        ++npc->anim;
      }

      if (npc->anim > 4) npc->anim = 1;

      if (npc->flags & 1) {
        npc->dir = 2;
        npc->xvel = 0x200;
      }

      if (npc->flags & 4) {
        npc->dir = 0;
        npc->xvel = -0x200;
      }

      if (npc->dir == 0)
        npc->xvel = -0x400;
      else
        npc->xvel = 0x400;

      break;

    case 6:
      npc->act = 7;
      npc->act_wait = 0;
      npc->anim = 1;
      npc->anim_wait = 0;
      npc->yvel = -0x400;
      // Fallthrough
    case 7:
      if (++npc->anim_wait > 2) {
        npc->anim_wait = 0;
        ++npc->anim;
      }

      if (npc->anim > 4) npc->anim = 1;

      if (npc->dir == 0)
        npc->xvel = -0x100;
      else
        npc->xvel = 0x100;

      if (npc->act_wait++ != 0 && npc->flags & 8) npc->act = 3;

      break;

    case 8:
      npc->anim = 1;
      npc->act_wait = 0;
      npc->act = 9;
      npc->yvel = -0x200;
      // Fallthrough
    case 9:
      if (npc->act_wait++ != 0 && npc->flags & 8) npc->act = 0;

      break;

    case 10:
      npc->act = 11;
      npc->anim = 6;
      npc->yvel = -0x400;
      snd_play_sound(-1, 50, SOUND_MODE_PLAY);

      if (npc->dir == 0)
        npc->xvel = -0x100;
      else
        npc->xvel = 0x100;

      break;

    case 11:
      if (npc->act_wait++ != 0 && npc->flags & 8) {
        npc->act = 12;
        npc->anim = 7;
        npc->bits |= NPC_INTERACTABLE;
      }

      break;

    case 12:
      npc->xvel = 0;
      break;
  }

  npc->yvel += 0x40;

  if (npc->xvel > 0x400) npc->xvel = 0x400;
  if (npc->xvel < -0x400) npc->xvel = -0x400;

  if (npc->yvel > 0x5FF) npc->yvel = 0x5FF;

  npc->x += npc->xvel;
  npc->y += npc->yvel;

  if (npc->dir == 0)
    npc->rect = &rc_left[npc->anim];
  else
    npc->rect = &rc_right[npc->anim];
}

// King
void npc_act_061(npc_t *npc) {
  int i;

  static const rect_t rc_left[11] = {
    // NpcRegu
    {224, 32, 240, 48},  //  0 - Stood
    {240, 32, 256, 48},  //  1 - Blinking
    {256, 32, 272, 48},  //  2 - Injured - falling backwards
    {272, 32, 288, 48},  //  3 - Lying down
    {288, 32, 304, 48},  //  4 - Walking - frame 1
    {224, 32, 240, 48},  //  5 - Walking - frame 2
    {304, 32, 320, 48},  //  6 - Walking - frame 3
    {224, 32, 240, 48},  //  7 - Walking - frame 4
    {272, 32, 288, 48},  //  8 - Dying - frame 1
    {0, 0, 0, 0},        //  9 - Dying - frame 2
    // NpcSym
    {112, 32, 128, 48},  // 10 - King's sword
  };

  static const rect_t rc_right[11] = {
    // NpcRegu
    {224, 48, 240, 64},  //  0 - Stood
    {240, 48, 256, 64},  //  1 - Blinking
    {256, 48, 272, 64},  //  2 - Injured - falling backwards
    {272, 48, 288, 64},  //  3 - Lying down
    {288, 48, 304, 64},  //  4 - Walking - frame 1
    {224, 48, 240, 64},  //  5 - Walking - frame 2
    {304, 48, 320, 64},  //  6 - Walking - frame 3
    {224, 48, 240, 64},  //  7 - Walking - frame 4
    {272, 48, 288, 64},  //  8 - Dying - frame 1
    {0, 0, 0, 0},        //  9 - Dying - frame 2
    // NpcSym
    {112, 32, 128, 48},  // 10 - King's sword
  };

  switch (npc->act) {
    case 0:  // Stood
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

    case 2:  // Blink
      if (++npc->act_wait > 8) {
        npc->act = 1;
        npc->anim = 0;
      }

      break;

    case 5:  // Lying down
      npc->anim = 3;
      npc->xvel = 0;
      break;

    case 6:  // Being knocked-back
      npc->act = 7;
      npc->act_wait = 0;
      npc->anim_wait = 0;
      npc->yvel = -0x400;
      // Fallthrough
    case 7:
      npc->anim = 2;

      if (npc->dir == 0)
        npc->xvel = -0x200;
      else
        npc->xvel = 0x200;

      // If touching ground, enter 'lying down' state (the `act_wait` check is probably
      // so he doesn't do it before he even leaves the ground in the first place)
      if (npc->act_wait++ != 0 && npc->flags & 8) npc->act = 5;

      break;

    case 8:  // Walking
      npc->act = 9;
      npc->anim = 4;
      npc->anim_wait = 0;
      // Fallthrough
    case 9:
      if (++npc->anim_wait > 4) {
        npc->anim_wait = 0;
        ++npc->anim;
      }

      if (npc->anim > 7) npc->anim = 4;

      if (npc->dir == 0)
        npc->xvel = -0x200;
      else
        npc->xvel = 0x200;

      break;

    case 10:  // Running
      npc->act = 11;
      npc->anim = 4;
      npc->anim_wait = 0;
      // Fallthrough
    case 11:
      if (++npc->anim_wait > 2) {
        npc->anim_wait = 0;
        ++npc->anim;
      }

      if (npc->anim > 7) npc->anim = 4;

      if (npc->dir == 0)
        npc->xvel = -0x400;
      else
        npc->xvel = 0x400;

      break;

    case 20:  // Spawn his sword, before entering his 'idle' state
      npc_spawn(NPC_KINGS_SWORD, 0, 0, 0, 0, 2, npc, 0x100);
      npc->anim = 0;
      npc->act = 0;
      break;

    case 30:  // Flying through air after being attacked by Misery
      npc->act = 31;
      npc->act_wait = 0;
      npc->anim_wait = 0;
      npc->yvel = 0;
      // Fallthrough
    case 31:
      npc->anim = 2;

      if (npc->dir == 0)
        npc->xvel = -0x600;
      else
        npc->xvel = 0x600;

      if (npc->flags & 1) {
        npc->dir = 2;
        npc->act = 7;
        npc->act_wait = 0;
        npc->anim_wait = 0;
        npc->yvel = -0x400;
        npc->xvel = 0x200;
        snd_play_sound(-1, 71, SOUND_MODE_PLAY);
        npc_death_fx(npc->x, npc->y, 0x800, 4);
      }

      break;

    case 40:  // Dying
      npc->act = 42;
      npc->act_wait = 0;
      npc->anim = 8;
      snd_play_sound(-1, 29, SOUND_MODE_PLAY);
      // Fallthrough
    case 42:
      if (++npc->anim > 9) npc->anim = 8;

      if (++npc->act_wait > 100) {
        for (i = 0; i < 4; ++i)
          npc_spawn(NPC_SMOKE, npc->x + (m_rand(-12, 12) * 0x200), npc->y + (m_rand(-12, 12) * 0x200),
                    m_rand(-341, 341), m_rand(-0x600, 0), 0, NULL, 0x100);

        npc->act = 50;
        npc->surf = SURFACE_ID_NPC_SYM;
        npc->anim = 10;
        npc->rect_prev = NULL; // ensure texrect gets updated
      }

      break;

    case 60:  // Leap (used to attack Balrog in the Sand Zone storehouse)
      npc->anim = 6;
      npc->act = 61;
      npc->yvel = -0x5FF;
      npc->xvel = 0x400;
      npc->count2 = 1;
      break;

    case 61:  // Leap - part 2
      npc->yvel += 0x40;

      if (npc->flags & 8) {
        npc->act = 0;
        npc->count2 = 0;
        npc->xvel = 0;
      }

      break;
  }

  // Apply gravity and speed-caps during most states
  if (npc->act < 30 || npc->act >= 40) {
    npc->yvel += 0x40;

    if (npc->xvel > 0x400) npc->xvel = 0x400;
    if (npc->xvel < -0x400) npc->xvel = -0x400;

    if (npc->yvel > 0x5FF) npc->yvel = 0x5FF;
  }

  npc->x += npc->xvel;
  npc->y += npc->yvel;

  if (npc->dir == 0)
    npc->rect = &rc_left[npc->anim];
  else
    npc->rect = &rc_right[npc->anim];
}

// Kazuma at computer
void npc_act_062(npc_t *npc) {
  static const rect_t rc_left[3] = {
    {272, 192, 288, 216},
    {288, 192, 304, 216},
    {304, 192, 320, 216},
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

// Toroko with stick
void npc_act_063(npc_t *npc) {
  static const rect_t rc_left[6] = {
    {64, 64, 80, 80}, {80, 64, 96, 80}, {64, 64, 80, 80},
    {96, 64, 112, 80}, {112, 64, 128, 80}, {128, 64, 144, 80},
  };

  static const rect_t rc_right[6] = {
    {64, 80, 80, 96}, {80, 80, 96, 96}, {64, 80, 80, 96},
    {96, 80, 112, 96}, {112, 80, 128, 96}, {128, 80, 144, 96},
  };

  switch (npc->act) {
    case 0:
      npc->act = 1;
      npc->act_wait = 0;
      npc->anim_wait = 0;
      npc->yvel = -0x400;
      // Fallthrough
    case 1:
      if (npc->yvel > 0) npc->bits &= ~NPC_IGNORE_SOLIDITY;

      if (++npc->anim_wait > 2) {
        npc->anim_wait = 0;
        ++npc->anim;
      }

      if (npc->anim > 3) npc->anim = 0;

      if (npc->dir == 0)
        npc->xvel = -0x100;
      else
        npc->xvel = 0x100;

      if (npc->act_wait++ != 0 && npc->flags & 8) npc->act = 2;

      break;

    case 2:
      npc->act = 3;
      npc->act_wait = 0;
      npc->anim = 0;
      npc->anim_wait = 0;
      // Fallthrough
    case 3:
      if (++npc->anim_wait > 2) {
        npc->anim_wait = 0;
        ++npc->anim;
      }

      if (npc->anim > 3) npc->anim = 0;

      if (++npc->act_wait > 50) {
        npc->act_wait = 40;
        npc->xvel *= -1;

        if (npc->dir == 0)
          npc->dir = 2;
        else
          npc->dir = 0;
      }

      if (npc->act_wait > 35) npc->bits |= NPC_SHOOTABLE;

      if (npc->dir == 0)
        npc->xvel -= 0x40;
      else
        npc->xvel += 0x40;

      if (npc->shock) {
        npc->act = 4;
        npc->anim = 4;
        npc->yvel = -0x400;
        npc->bits &= ~NPC_SHOOTABLE;
        npc->damage = 0;
      }

      break;

    case 4:
      if (npc->dir == 0)
        npc->xvel = -0x100;
      else
        npc->xvel = 0x100;

      if (npc->act_wait++ != 0 && npc->flags & 8) {
        npc->act = 5;
        npc->bits |= NPC_INTERACTABLE;
      }

      break;

    case 5:
      npc->xvel = 0;
      npc->anim = 5;
      break;
  }

  npc->yvel += 0x40;

  if (npc->xvel > 0x400) npc->xvel = 0x400;
  if (npc->xvel < -0x400) npc->xvel = -0x400;

  if (npc->yvel > 0x5FF) npc->yvel = 0x5FF;

  npc->x += npc->xvel;
  npc->y += npc->yvel;

  if (npc->dir == 0)
    npc->rect = &rc_left[npc->anim];
  else
    npc->rect = &rc_right[npc->anim];
}

// First Cave Critter
void npc_act_064(npc_t *npc) {
  static const rect_t rc_left[3] = {
    {0, 0, 16, 16},
    {16, 0, 32, 16},
    {32, 0, 48, 16},
  };

  static const rect_t rc_right[3] = {
    {0, 16, 16, 32},
    {16, 16, 32, 32},
    {32, 16, 48, 32},
  };

  switch (npc->act) {
    case 0:  // Initialize
      npc->y += 3 * 0x200;
      npc->act = 1;
      // Fallthrough
    case 1:  // Waiting
      // Look at player
      if (npc->x > player.x)
        npc->dir = 0;
      else
        npc->dir = 2;

      if (npc->tgt_x < 100) ++npc->tgt_x;

      // Open eyes near player
      if (npc->act_wait >= 8 && npc->x - (112 * 0x200) < player.x && npc->x + (112 * 0x200) > player.x &&
          npc->y - (80 * 0x200) < player.y && npc->y + (80 * 0x200) > player.y) {
        npc->anim = 1;
      } else {
        if (npc->act_wait < 8) ++npc->act_wait;

        npc->anim = 0;
      }

      // Jump if attacked
      if (npc->shock) {
        npc->act = 2;
        npc->anim = 0;
        npc->act_wait = 0;
      }

      // Jump if player is nearby
      if (npc->act_wait >= 8 && npc->tgt_x >= 100 && npc->x - (64 * 0x200) < player.x && npc->x + (64 * 0x200) > player.x &&
          npc->y - (80 * 0x200) < player.y && npc->y + (48 * 0x200) > player.y) {
        npc->act = 2;
        npc->anim = 0;
        npc->act_wait = 0;
      }
      break;

    case 2:  // Going to jump
      if (++npc->act_wait > 8) {
        // Set jump state
        npc->act = 3;
        npc->anim = 2;

        // Jump
        npc->yvel = -0x5FF;
        snd_play_sound(-1, 30, SOUND_MODE_PLAY);

        // Jump in facing direction
        if (npc->dir == 0)
          npc->xvel = -0x100;
        else
          npc->xvel = 0x100;
      }
      break;

    case 3:  // Jumping
      // Land
      if (npc->flags & 8) {
        npc->xvel = 0;
        npc->act_wait = 0;
        npc->anim = 0;
        npc->act = 1;
        snd_play_sound(-1, 23, SOUND_MODE_PLAY);
      }
      break;
  }

  // Gravity
  npc->yvel += 0x40;
  if (npc->yvel > 0x5FF) npc->yvel = 0x5FF;

  // Move
  npc->x += npc->xvel;
  npc->y += npc->yvel;

  // Set framerect
  if (npc->dir == 0)
    npc->rect = &rc_left[npc->anim];
  else
    npc->rect = &rc_right[npc->anim];
}

// First Cave Bat
void npc_act_065(npc_t *npc) {
  switch (npc->act) {
    case 0:
      npc->tgt_x = npc->x;
      npc->tgt_y = npc->y;
      npc->count1 = 120;
      npc->act = 1;
      npc->act_wait = m_rand(0, 50);
      // Fallthrough

    case 1:
      if (++npc->act_wait < 50) break;

      npc->act_wait = 0;
      npc->act = 2;
      npc->yvel = 0x300;

      break;

    case 2:
      if (player.x < npc->x)
        npc->dir = 0;
      else
        npc->dir = 2;

      if (npc->tgt_y < npc->y) npc->yvel -= 0x10;
      if (npc->tgt_y > npc->y) npc->yvel += 0x10;

      if (npc->yvel > 0x300) npc->yvel = 0x300;
      if (npc->yvel < -0x300) npc->yvel = -0x300;

      break;
  }

  npc->x += npc->xvel;
  npc->y += npc->yvel;

  static const rect_t rect_left[4] = {
    {32, 32, 48, 48},
    {48, 32, 64, 48},
    {64, 32, 80, 48},
    {80, 32, 96, 48},
  };

  static const rect_t rect_right[4] = {
    {32, 48, 48, 64},
    {48, 48, 64, 64},
    {64, 48, 80, 64},
    {80, 48, 96, 64},
  };

  if (++npc->anim_wait > 1) {
    npc->anim_wait = 0;
    ++npc->anim;
  }

  if (npc->anim > 2) npc->anim = 0;

  if (npc->dir == 0)
    npc->rect = &rect_left[npc->anim];
  else
    npc->rect = &rect_right[npc->anim];
}

// Misery bubble
void npc_act_066(npc_t *npc) {
  static const rect_t rect[4] = {
    {32, 192, 56, 216},
    {56, 192, 80, 216},
    {32, 216, 56, 240},
    {56, 216, 80, 240},
  };

  switch (npc->act) {
    case 0:
      int a;
      for (a = 0; a < npc_list_max; ++a)
        if (npc_list[a].event_num == 1000) break;

      if (a == NPC_MAX) break;

      npc->tgt_x = npc_list[a].x;
      npc->tgt_y = npc_list[a].y;
      npc->count1 = a;

      unsigned char deg;
      deg = m_atan2(npc->x - npc->tgt_x, npc->y - npc->tgt_y);
      npc->xvel = m_cos(deg) * 2;
      npc->yvel = m_sin(deg) * 2;

      npc->act = 1;
      // Fallthrough
    case 1:
      if (++npc->anim_wait > 1) {
        npc->anim_wait = 0;
        ++npc->anim;
      }

      if (npc->anim > 1) npc->anim = 0;

      if (npc->x - (3 * 0x200) < npc->tgt_x && npc->x + (3 * 0x200) > npc->tgt_x && npc->y - (3 * 0x200) < npc->tgt_y &&
          npc->y + (3 * 0x200) > npc->tgt_y) {
        npc->act = 2;
        npc->anim = 2;
        npc_list[npc->count1].cond = 0;
        snd_play_sound(-1, 21, SOUND_MODE_PLAY);
      }

      break;

    case 2:
      npc->xvel -= 0x20;
      npc->yvel -= 0x20;

      if (npc->xvel < -0x5FF) npc->xvel = -0x5FF;
      if (npc->yvel < -0x5FF) npc->yvel = -0x5FF;

      if (npc->y < -8 * 0x200) npc->cond = 0;

      if (++npc->anim_wait > 3) {
        npc->anim_wait = 0;
        ++npc->anim;
      }

      if (npc->anim > 3) npc->anim = 2;

      break;
  }

  npc->x += npc->xvel;
  npc->y += npc->yvel;

  npc->rect = &rect[npc->anim];
}

// Misery (floating)
void npc_act_067(npc_t *npc) {
  switch (npc->act) {
    case 0:
      npc->act = 1;
      npc->tgt_x = npc->x;
      npc->tgt_y = npc->y;
      npc->anim = 0;
      snd_play_sound(-1, 29, SOUND_MODE_PLAY);
      // Fallthrough
    case 1:
      npc->x = npc->tgt_x + (m_rand(-1, 1) * 0x200);

      if (++npc->act_wait == 0x20) npc->act = 10;

      break;

    case 10:
      npc->act = 11;
      npc->act_wait = 0;
      npc->anim = 0;
      npc->yvel = 0x200;
      // Fallthrough

    case 11:
      if (npc->tgt_y < npc->y) npc->yvel -= 0x10;
      if (npc->tgt_y > npc->y) npc->yvel += 0x10;

      if (npc->yvel > 0x100) npc->yvel = 0x100;
      if (npc->yvel < -0x100) npc->yvel = -0x100;

      break;

    case 13:
      npc->anim = 1;

      npc->yvel += 0x40;
      if (npc->yvel > 0x5FF) npc->yvel = 0x5FF;

      if (npc->flags & 8) {
        snd_play_sound(-1, 23, SOUND_MODE_PLAY);
        npc->yvel = 0;
        npc->act = 14;
        npc->bits |= NPC_IGNORE_SOLIDITY;
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
        snd_play_sound(-1, 21, SOUND_MODE_PLAY);
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
        snd_play_sound(-1, 101, SOUND_MODE_PLAY);
        // SetFlash(0, 0, FLASH_MODE_FLASH);
        npc->act = 27;
        npc->anim = 7;
      }

      break;

    case 27:
      if (++npc->act_wait == 50) npc->act = 14;

      break;
  }

  npc->x += npc->xvel;
  npc->y += npc->yvel;

  static const rect_t rc_left[8] = {
    {80, 0, 96, 16},   {96, 0, 112, 16},  {112, 0, 128, 16}, {128, 0, 144, 16},
    {144, 0, 160, 16}, {160, 0, 176, 16}, {176, 0, 192, 16}, {144, 0, 160, 16},
  };

  static const rect_t rc_right[8] = {
    {80, 16, 96, 32},   {96, 16, 112, 32},  {112, 16, 128, 32}, {128, 16, 144, 32},
    {144, 16, 160, 32}, {160, 16, 176, 32}, {176, 16, 192, 32}, {144, 16, 160, 32},
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

  if (npc->act == 1 && npc->anim_wait < 32)
    npc->rect_delta.bottom = (++npc->anim_wait / 2) - 16;
}

// Balrog (running)
void npc_act_068(npc_t *npc) {
  switch (npc->act) {
    case 0:
      npc->act = 1;
      npc->anim = 0;
      npc->act_wait = 30;

      if (npc->x > player.x)
        npc->dir = 0;
      else
        npc->dir = 2;
      // Fallthrough
    case 1:
      if (--npc->act_wait) break;

      npc->act = 2;
      ++npc->count1;

      break;

    case 2:
      npc->act = 3;
      npc->act_wait = 0;
      npc->anim = 1;
      npc->anim_wait = 0;
      // Fallthrough
    case 3:
      if (++npc->anim_wait > 3) {
        npc->anim_wait = 0;

        if (++npc->anim == 2 || npc->anim == 4)
          snd_play_sound(-1, 23, SOUND_MODE_PLAY);
      }

      if (npc->anim > 4) npc->anim = 1;

      if (npc->dir == 0)
        npc->xvel -= 0x10;
      else
        npc->xvel += 0x10;

      if (npc->act_wait >= 8 && npc->x - (12 * 0x200) < player.x && npc->x + (12 * 0x200) > player.x &&
          npc->y - (12 * 0x200) < player.y && npc->y + (8 * 0x200) > player.y) {
        npc->act = 10;
        npc->anim = 5;
        player.cond |= 2;
        // DamageMyChar(2);
        break;
      }

      ++npc->act_wait;

      if (npc->flags & 5 || npc->act_wait > 75) {
        npc->act = 9;
        npc->anim = 0;
        break;
      }

      if ((npc->count1 % 3) == 0 && npc->act_wait > 25) {
        npc->act = 4;
        npc->anim = 7;
        npc->yvel = -0x400;
        break;
      }

      break;

    case 4:
      if (npc->flags & 8) {
        npc->act = 9;
        npc->anim = 8;
        // SetQuake(30);
        snd_play_sound(-1, 26, SOUND_MODE_PLAY);
      }

      if (npc->act_wait >= 8 && npc->x - (12 * 0x200) < player.x && npc->x + (12 * 0x200) > player.x &&
          npc->y - (12 * 0x200) < player.y && npc->y + (8 * 0x200) > player.y) {
        npc->act = 10;
        npc->anim = 5;
        player.cond |= 2;
        // DamageMyChar(2);
      }

      break;

    case 9:
      npc->xvel = (npc->xvel * 4) / 5;

      if (npc->xvel != 0) break;

      npc->act = 0;

      break;

    case 10:
      player.x = npc->x;
      player.y = npc->y;

      npc->xvel = (npc->xvel * 4) / 5;

      if (npc->xvel != 0) break;

      npc->act = 11;
      npc->act_wait = 0;
      npc->anim = 5;
      npc->anim_wait = 0;

      break;

    case 11:
      player.x = npc->x;
      player.y = npc->y;

      if (++npc->anim_wait > 2) {
        npc->anim_wait = 0;
        ++npc->anim;
      }

      if (npc->anim > 6) npc->anim = 5;

      if (++npc->act_wait > 100) npc->act = 20;

      break;

    case 20:
      snd_play_sound(-1, 25, SOUND_MODE_PLAY);
      player.cond &= ~2;

      if (npc->dir == 0) {
        player.x += 4 * 0x200;
        player.y -= 8 * 0x200;
        player.xvel = 0x5FF;
        player.yvel = -0x200;
        player.dir = 2;
        npc->dir = 2;
      } else {
        player.x -= 4 * 0x200;
        player.y -= 8 * 0x200;
        player.xvel = -0x5FF;
        player.yvel = -0x200;
        player.dir = 0;
        npc->dir = 0;
      }

      npc->act = 21;
      npc->act_wait = 0;
      npc->anim = 7;
      // Fallthrough
    case 21:
      if (++npc->act_wait < 50) break;

      npc->act = 0;
      break;
  }

  npc->yvel += 0x20;

  if (npc->xvel < -0x400) npc->xvel = -0x400;
  if (npc->xvel > 0x400) npc->xvel = 0x400;

  if (npc->yvel > 0x5FF) npc->yvel = 0x5FF;

  npc->x += npc->xvel;
  npc->y += npc->yvel;

  static const rect_t rect_left[9] = {
    {0, 0, 40, 24},    {0, 48, 40, 72},    {0, 0, 40, 24},    {40, 48, 80, 72}, {0, 0, 40, 24},
    {80, 48, 120, 72}, {120, 48, 160, 72}, {120, 0, 160, 24}, {80, 0, 120, 24},
  };

  static const rect_t rect_right[9] = {
    {0, 24, 40, 48},   {0, 72, 40, 96},    {0, 24, 40, 48},    {40, 72, 80, 96},  {0, 24, 40, 48},
    {80, 72, 120, 96}, {120, 72, 160, 96}, {120, 24, 160, 48}, {80, 24, 120, 48},
  };

  if (npc->dir == 0)
    npc->rect = &rect_left[npc->anim];
  else
    npc->rect = &rect_right[npc->anim];
}

// Pignon
void npc_act_069(npc_t *npc) {
  static const rect_t rc_left[6] = {
    {48, 0, 64, 16}, {64, 0, 80, 16}, {80, 0, 96, 16},
    {96, 0, 112, 16}, {48, 0, 64, 16}, {112, 0, 128, 16},
  };

  static const rect_t rc_right[6] = {
    {48, 16, 64, 32}, {64, 16, 80, 32}, {80, 16, 96, 32},
    {96, 16, 112, 32}, {48, 16, 64, 32}, {112, 16, 128, 32},
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

// Sparkle
void npc_act_070(npc_t *npc) {
  static const rect_t rect[4] = {
    {96, 48, 112, 64},
    {112, 48, 128, 64},
    {128, 48, 144, 64},
    {144, 48, 160, 64},
  };

  if (++npc->anim_wait > 3) {
    npc->anim_wait = 0;
    ++npc->anim;
  }

  if (npc->anim > 3) npc->anim = 0;

  npc->rect = &rect[npc->anim];
}

// Chinfish
void npc_act_071(npc_t *npc) {
  switch (npc->act) {
    case 0:
      npc->act = 1;
      npc->tgt_x = npc->x;
      npc->tgt_y = npc->y;
      npc->yvel = 0x80;
      // Fallthrough
    case 1:
      if (npc->tgt_y < npc->y) npc->yvel -= 8;
      if (npc->tgt_y > npc->y) npc->yvel += 8;

      if (npc->yvel > 0x100) npc->yvel = 0x100;
      if (npc->yvel < -0x100) npc->yvel = -0x100;

      break;
  }

  npc->x += npc->xvel;
  npc->y += npc->yvel;

  static const rect_t rc_left[3] = {
    {64, 32, 80, 48},
    {80, 32, 96, 48},
    {96, 32, 112, 48},
  };

  static const rect_t rc_right[3] = {
    {64, 48, 80, 64},
    {80, 48, 96, 64},
    {96, 48, 112, 64},
  };

  if (++npc->anim_wait > 4) {
    npc->anim_wait = 0;
    ++npc->anim;
  }

  if (npc->anim > 1) npc->anim = 0;

  if (npc->shock) npc->anim = 2;

  if (npc->dir == 0)
    npc->rect = &rc_left[npc->anim];
  else
    npc->rect = &rc_right[npc->anim];
}

// Sprinkler
void npc_act_072(npc_t *npc) {
  if (npc->dir == 0) {
    if (++npc->anim_wait > 1) {
      npc->anim_wait = 0;
      ++npc->anim;
    }

    if (npc->anim > 1) {
      npc->anim = 0;
      return;
    }

    if (player.x < npc->x + (((VID_WIDTH / 2) + 160) * 0x200) &&
        player.x > npc->x - (((VID_WIDTH / 2) + 160) * 0x200) &&
        player.y < npc->y + (((VID_HEIGHT / 2) + 120) * 0x200) &&
        player.y > npc->y - (((VID_HEIGHT / 2) + 120) * 0x200)) {
      if (++npc->act % 2)
        npc_spawn(73, npc->x, npc->y, m_rand(-0x200, 0x200) * 2, m_rand(-0x200, 0x80) * 3, 0, NULL, 0x100);

      npc_spawn(73, npc->x, npc->y, m_rand(-0x200, 0x200) * 2, m_rand(-0x200, 0x80) * 3, 0, NULL, 0x100);
    }
  }

  static const rect_t rect[2] = {
    {224, 48, 240, 64},
    {240, 48, 256, 64},
  };

  npc->rect = &rect[npc->anim];
}

// Water droplet
void npc_act_073(npc_t *npc) {
  static const rect_t rect[5] = {
    {72, 16, 74, 18}, {74, 16, 76, 18}, {76, 16, 78, 18},
    {78, 16, 80, 18}, {80, 16, 82, 18},
  };

  npc->yvel += 0x20;
  npc->anim = m_rand(0, 4);

  if (npc->yvel > 0x5FF) npc->yvel = 0x5FF;

  npc->x += npc->xvel;
  npc->y += npc->yvel;

  npc->rect = &rect[npc->anim];

  if (npc->dir == 2) {
    npc->rect_delta.top = 2;
    npc->rect_delta.bottom = 2;
  }

  if (++npc->act_wait > 10) {
    if (npc->flags & 1) npc->cond = 0;
    if (npc->flags & 4) npc->cond = 0;
    if (npc->flags & 8) npc->cond = 0;
    if (npc->flags & 0x100) npc->cond = 0;
  }

  if (npc->y > stage_data->height * 0x200 * 0x10) npc->cond = 0;
}

// Jack
void npc_act_074(npc_t *npc) {
  static const rect_t rc_left[6] = {
    {64, 0, 80, 16}, {80, 0, 96, 16}, {96, 0, 112, 16},
    {64, 0, 80, 16}, {112, 0, 128, 16}, {64, 0, 80, 16},
  };

  static const rect_t rc_right[6] = {
    {64, 16, 80, 32}, {80, 16, 96, 32}, {96, 16, 112, 32},
    {64, 16, 80, 32}, {112, 16, 128, 32}, {64, 16, 80, 32},
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

    case 8:
      npc->act = 9;
      npc->anim = 2;
      npc->anim_wait = 0;
      // Fallthrough
    case 9:
      if (++npc->anim_wait > 4) {
        npc->anim_wait = 0;
        ++npc->anim;
      }

      if (npc->anim > 5) npc->anim = 2;

      if (npc->dir == 0)
        npc->xvel = -0x200;
      else
        npc->xvel = 0x200;

      break;
  }

  npc->yvel += 0x40;

  if (npc->xvel > 0x400) npc->xvel = 0x400;
  if (npc->xvel < -0x400) npc->xvel = -0x400;

  if (npc->yvel > 0x5FF) npc->yvel = 0x5FF;

  npc->x += npc->xvel;
  npc->y += npc->yvel;

  if (npc->dir == 0)
    npc->rect = &rc_left[npc->anim];
  else
    npc->rect = &rc_right[npc->anim];
}

// Kanpachi (fishing)
void npc_act_075(npc_t *npc) {
  static const rect_t rc_left[2] = {
    {272, 32, 296, 56},
    {296, 32, 320, 56},
  };

  switch (npc->act) {
    case 0:
      npc->act = 1;
      npc->anim = 0;
      npc->anim_wait = 0;
      // Fallthrough
    case 1:
      if (npc->x - (48 * 0x200) < player.x && npc->x + (48 * 0x200) > player.x && npc->y - (48 * 0x200) < player.y &&
          npc->y + (16 * 0x200) > player.y)
        npc->anim = 1;
      else
        npc->anim = 0;

      break;
  }

  npc->rect = &rc_left[npc->anim];
}

// Flowers
void npc_act_076(npc_t *npc) {
  static const rect_t rect[8] = {
    {0, 0, 16, 16},  {16, 0, 32, 16}, {32, 0, 48, 16},  {48, 0, 64, 16},
    {64, 0, 80, 16}, {80, 0, 96, 16}, {96, 0, 112, 16}, {112, 0, 128, 16},
  };
  npc->rect = &rect[npc->event_num];
}

// Yamashita
void npc_act_077(npc_t *npc) {
  static const rect_t rc[3] = {
    {0, 16, 48, 48},
    {48, 16, 96, 48},
    {96, 16, 144, 48},
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
    npc->rect = &rc[npc->anim];
  else
    npc->rect = &rc[2];
}

// Pot
void npc_act_078(npc_t *npc) {
  static const rect_t rc[2] = {
    {160, 48, 176, 64},
    {176, 48, 192, 64},
  };

  if (npc->dir == 0)
    npc->rect = &rc[0];
  else
    npc->rect = &rc[1];
}

// Mahin
void npc_act_079(npc_t *npc) {
  static const rect_t rc_left[3] = {
    {0, 0, 16, 16},
    {16, 0, 32, 16},
    {32, 0, 48, 16},
  };

  static const rect_t rc_right[3] = {
    {0, 16, 16, 32},
    {16, 16, 32, 32},
    {32, 16, 48, 32},
  };

  switch (npc->act) {
    case 0:
      npc->act = 1;
      npc->anim = 2;
      npc->anim_wait = 0;
      break;

    case 2:
      npc->anim = 0;

      if (m_rand(0, 120) == 10) {
        npc->act = 3;
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

    case 3:
      if (++npc->act_wait > 8) {
        npc->act = 2;
        npc->anim = 0;
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
