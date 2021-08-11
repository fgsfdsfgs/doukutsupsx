#include "game/npc_act/npc_act.h"

// default act func, used as a placeholder in the table
// draw question mark and spam in console
void npc_act_null(npc_t *npc) {
  static const rect_t rect = { 0, 80, 16, 96 };

  // set texrect manually since this lad is not in the table
  if (!npc->rect) {
    npc->rect = npc->rect_prev = &rect;
    npc->texrect.r = rect;
    gfx_set_texrect(&npc->texrect, SURFACE_ID_CARET);
  }

  const int x = TO_INT(npc->x);
  const int y = TO_INT(npc->y);

  if (npc->shock == 0)
    printf("idiot without actfunc: %03d at %04d, %04d\n", npc->class_num, x, y);

  npc->shock = 0xFF;
}

// Null
void npc_act_000(npc_t *npc) {
  static const rect_t rect = { 0, 0, 16, 16 };

  if (npc->act == 0) {
    npc->act = 1;
    if (npc->dir == 2)
      npc->y += 16 * 0x200;
  }

  npc->rect = &rect;
}

// Experience
void npc_act_001(npc_t *npc) {
  // Get framerects
  static const rect_t rect[] = {
    {0, 16, 16, 32},  {16, 16, 32, 32}, {32, 16, 48, 32},
    {48, 16, 64, 32}, {64, 16, 80, 32}, {80, 16, 96, 32},
    {0, 0, 0, 0}
  };

  // In wind
  const int bktype = stage_data->bk_type;
  if (bktype == BACKGROUND_TYPE_AUTOSCROLL || bktype == BACKGROUND_TYPE_CLOUDS_WINDY) {
    if (npc->act == 0) {
      // Set state
      npc->act = 1;

      // Set random speed
      npc->yvel = m_rand(-0x80, 0x80);
      npc->xvel = m_rand(0x7F, 0x100);
    }

    // Blow to the left
    npc->xvel -= 8;

    // Destroy when off-screen
    if (npc->x < 80 * 0x200) npc->cond = 0;

    // Limit speed
    if (npc->xvel < -0x600) npc->xvel = -0x600;

    // Bounce off walls
    if (npc->flags & 1) npc->xvel = 0x100;
    if (npc->flags & 2) npc->yvel = 0x40;
    if (npc->flags & 8) npc->yvel = -0x40;
  }
  // When not in wind
  else {
    if (npc->act == 0) {
      // Set state
      npc->act = 1;
      npc->anim = m_rand(0, 4);

      // m_rand speed
      npc->xvel = m_rand(-0x200, 0x200);
      npc->yvel = m_rand(-0x400, 0);

      // m_rand direction (reverse animation or not)
      if (m_rand(0, 1) != 0)
        npc->dir = 0;
      else
        npc->dir = 2;
    }

    // Gravity
    if (npc->flags & 0x100)
      npc->yvel += 0x15;
    else
      npc->yvel += 0x2A;

    // Bounce off walls
    if (npc->flags & 1 && npc->xvel < 0) npc->xvel *= -1;
    if (npc->flags & 4 && npc->xvel > 0) npc->xvel *= -1;

    // Bounce off ceiling
    if (npc->flags & 2 && npc->yvel < 0) npc->yvel *= -1;

    // Bounce off floor
    if (npc->flags & 8) {
      snd_play_sound(-1, 45, SOUND_MODE_PLAY);
      npc->yvel = -0x280;
      npc->xvel = 2 * npc->xvel / 3;
    }

    // Play bounce song (and try to clip out of floor if stuck)
    if (npc->flags & 0xD) {
      snd_play_sound(-1, 45, SOUND_MODE_PLAY);
      if (++npc->count2 > 2) npc->y -= 1 * 0x200;
    } else {
      npc->count2 = 0;
    }

    // Limit speed
    if (npc->xvel < -0x5FF) npc->xvel = -0x5FF;
    if (npc->xvel > 0x5FF) npc->xvel = 0x5FF;
    if (npc->yvel < -0x5FF) npc->yvel = -0x5FF;
    if (npc->yvel > 0x5FF) npc->yvel = 0x5FF;
  }

  // Move
  npc->y += npc->yvel;
  npc->x += npc->xvel;

  // Animate
  ++npc->anim_wait;

  if (npc->dir == 0) {
    if (npc->anim_wait > 2) {
      npc->anim_wait = 0;

      if (++npc->anim > 5) npc->anim = 0;
    }
  } else {
    if (npc->anim_wait > 2) {
      npc->anim_wait = 0;

      if (--npc->anim < 0) npc->anim = 5;
    }
  }

  npc->rect = &rect[npc->anim];

  // Size
  if (npc->act != 0) {
    switch (npc->info->exp) {
      case 5:
        npc->rect_delta.top = 16;
        npc->rect_delta.bottom = 16;
        npc->rect_prev = NULL; // force texrect update
        break;

      case 20:
        npc->rect_delta.top = 32;
        npc->rect_delta.bottom = 32;
        npc->rect_prev = NULL; // force texrect update
        break;
    }

    npc->act = 1;
  }

  // Delete after 500 frames
  if (++npc->count1 > 500 && npc->anim == 5 && npc->anim_wait == 2)
    npc->cond = 0;

  // Blink after 400 frames
  if (npc->count1 > 400) {
    if (npc->count1 / 2 % 2)
      npc->rect = &rect[6];
  }
}

// Behemoth
void npc_act_002(npc_t *npc) {
  // Rects
  static const rect_t rc_left[] = {
    {32, 0, 64, 24},  {0, 0, 32, 24},    {32, 0, 64, 24},   {64, 0, 96, 24},
    {96, 0, 128, 24}, {128, 0, 160, 24}, {160, 0, 192, 24},
  };
  static const rect_t rc_right[] = {
    {32, 24, 64, 48},   {0, 24, 32, 48},   {32, 24, 64, 48},
    {64, 24, 96, 48},   {96, 24, 128, 48}, {128, 24, 160, 48},
    {160, 24, 192, 48}
  };

  // Turn when touching a wall
  if (npc->flags & 1)
    npc->dir = 2;
  else if (npc->flags & 4)
    npc->dir = 0;

  switch (npc->act) {
    case 0:  // Walking
      if (npc->dir == 0)
        npc->xvel = -0x100;
      else
        npc->xvel = 0x100;

      if (++npc->anim_wait > 8) {
        npc->anim_wait = 0;
        ++npc->anim;
      }

      if (npc->anim > 3) npc->anim = 0;

      if (npc->shock) {
        npc->count1 = 0;
        npc->act = 1;
        npc->anim = 4;
      }

      break;

    case 1:  // Shot
      npc->xvel = (npc->xvel * 7) / 8;

      if (++npc->count1 > 40) {
        if (npc->shock) {
          npc->count1 = 0;
          npc->act = 2;
          npc->anim = 6;
          npc->anim_wait = 0;
          npc->damage = 5;
        } else {
          npc->act = 0;
          npc->anim_wait = 0;
        }
      }
      break;

    case 2:  // Charge
      if (npc->dir == 0)
        npc->xvel = -0x400;
      else
        npc->xvel = 0x400;

      if (++npc->count1 > 200) {
        npc->act = 0;
        npc->damage = 1;
      }

      if (++npc->anim_wait > 5) {
        npc->anim_wait = 0;
        ++npc->anim;
      }

      if (npc->anim > 6) {
        npc->anim = 5;
        // These three lines are missing in the Linux port, because it's based on v1.0.0.4:
        // https://www.cavestory.org/forums/threads/version-1-0-0-5-really-different-than-1-0-0-6.102/#post-3231
        snd_play_sound(-1, 26, SOUND_MODE_PLAY);
        npc_spawn(4, npc->x, npc->y + (3 * 0x200), 0, 0, 0, NULL, 0x100);
        cam_start_quake_small(8);
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

// Dead enemy (to make sure the damage-value doesn't teleport to a newly-loaded NPC)
void npc_act_003(npc_t *npc) {
  static const rect_t rect = {0, 0, 0, 0};
  npc->rect = &rect;
}

// Smoke
void npc_act_004(npc_t *npc) {
  static const rect_t rc_left[] = {
    {16, 0, 17, 1},  {16, 0, 32, 16}, {32, 0, 48, 16},  {48, 0, 64, 16},
    {64, 0, 80, 16}, {80, 0, 96, 16}, {96, 0, 112, 16}, {112, 0, 128, 16},
  };
  static const rect_t rc_up[] = {
    {16, 0, 17, 1},     {80, 48, 96, 64},   {0, 128, 16, 144},
    {16, 128, 32, 144}, {32, 128, 48, 144}, {48, 128, 64, 144},
    {64, 128, 80, 144}, {80, 128, 96, 144},
  };

  u8 deg;

  if (npc->act == 0) {
    // Move in random direction at random speed
    if (npc->dir == 0 || npc->dir == 1) {
      deg = m_rand(0, 0xFF);
      npc->xvel = m_cos(deg) * m_rand(0x200, 0x5FF) / 0x200;
      npc->yvel = m_sin(deg) * m_rand(0x200, 0x5FF) / 0x200;
    }

    // Set state
    npc->anim = m_rand(0, 4);
    npc->anim_wait = m_rand(0, 3);
    npc->act = 1;
  } else {
    // Slight drag
    npc->xvel = (npc->xvel * 20) / 21;
    npc->yvel = (npc->yvel * 20) / 21;

    // Move
    npc->x += npc->xvel;
    npc->y += npc->yvel;
  }

  // Animate
  if (++npc->anim_wait > 4) {
    npc->anim_wait = 0;
    ++npc->anim;
  }

  // Set framerect
  if (npc->anim > 7) {
    // Destroy if over
    npc->cond = 0;
  } else {
    if (npc->dir == 1)
      npc->rect = &rc_up[npc->anim];
    if (npc->dir == 0 || npc->dir == 2)
      npc->rect = &rc_left[npc->anim];
  }
}

// Critter (Green, Egg Corridor)
void npc_act_005(npc_t *npc) {
  static const rect_t rc_left[] = {
    {0, 48, 16, 64},
    {16, 48, 32, 64},
    {32, 48, 48, 64},
  };
  static const rect_t rc_right[] = {
    {0, 64, 16, 80},
    {16, 64, 32, 80},
    {32, 64, 48, 80}
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

      // Open eyes near player
      if (npc->act_wait >= 8 && npc->x - (112 * 0x200) < player.x &&
          npc->x + (112 * 0x200) > player.x && npc->y - (80 * 0x200) < player.y &&
          npc->y + (80 * 0x200) > player.y) {
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
      if (npc->act_wait >= 8 && npc->x - (48 * 0x200) < player.x &&
          npc->x + (48 * 0x200) > player.x && npc->y - (80 * 0x200) < player.y &&
          npc->y + (48 * 0x200) > player.y) {
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
  npc->yvel += 64;
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

// Beetle (Goes left and right, Egg Corridor)
void npc_act_006(npc_t *npc) {
  static const rect_t rc_left[] = {
    {0, 80, 16, 96},  {16, 80, 32, 96}, {32, 80, 48, 96},
    {48, 80, 64, 96}, {64, 80, 80, 96},
  };
  static const rect_t rc_right[] = {
    {0, 96, 16, 112},  {16, 96, 32, 112}, {32, 96, 48, 112},
    {48, 96, 64, 112}, {64, 96, 80, 112}
  };

  switch (npc->act) {
    case 0:  // Initialize
      npc->act = 1;

      if (npc->dir == 0)
        npc->act = 1;
      else
        npc->act = 3;
      break;

    case 1:
      // Accelerate to the left
      npc->xvel -= 0x10;
      if (npc->xvel < -0x400) npc->xvel = -0x400;

      // Move
      if (npc->shock)
        npc->x += npc->xvel / 2;
      else
        npc->x += npc->xvel;

      // Animate
      if (++npc->anim_wait > 1) {
        npc->anim_wait = 0;
        ++npc->anim;
      }

      if (npc->anim > 2) npc->anim = 1;

      // Stop when hitting a wall
      if (npc->flags & 1) {
        npc->act = 2;
        npc->act_wait = 0;
        npc->anim = 0;
        npc->xvel = 0;
        npc->dir = 2;
      }
      break;

    case 2:
      // Wait 60 frames then move to the right
      if (++npc->act_wait > 60) {
        npc->act = 3;
        npc->anim_wait = 0;
        npc->anim = 1;
      }
      break;

    case 3:
      // Accelerate to the right
      npc->xvel += 0x10;
      if (npc->xvel > 0x400) npc->xvel = 0x400;

      // Move
      if (npc->shock)
        npc->x += npc->xvel / 2;
      else
        npc->x += npc->xvel;

      // Animate
      if (++npc->anim_wait > 1) {
        npc->anim_wait = 0;
        ++npc->anim;
      }

      if (npc->anim > 2) npc->anim = 1;

      // Stop when hitting a wall
      if (npc->flags & 4) {
        npc->act = 4;
        npc->act_wait = 0;
        npc->anim = 0;
        npc->xvel = 0;
        npc->dir = 0;
      }
      break;

    case 4:
      // Wait 60 frames then move to the left
      if (++npc->act_wait > 60) {
        npc->act = 1;
        npc->anim_wait = 0;
        npc->anim = 1;
      }
      break;
  }

  // Set framerect
  if (npc->dir == 0)
    npc->rect = &rc_left[npc->anim];
  else
    npc->rect = &rc_right[npc->anim];
}

// Basil
void npc_act_007(npc_t *npc) {
  static const rect_t rc_left[] = {
    {256, 64, 288, 80},
    {256, 80, 288, 96},
    {256, 96, 288, 112},
  };
  static const rect_t rc_right[] = {
    {288, 64, 320, 80},
    {288, 80, 320, 96},
    {288, 96, 320, 112}
  };

  switch (npc->act) {
    case 0:
      npc->x = player.x;  // Spawn beneath player

      if (npc->dir == 0)
        npc->act = 1;
      else
        npc->act = 2;

      break;

    case 1:  // Going left
      npc->xvel -= 0x40;

      // Turn around if far enough away from the player
      if (npc->x < player.x - (192 * 0x200)) npc->act = 2;

      // Turn around if touching a wall
      if (npc->flags & 1) {
        npc->xvel = 0;
        npc->act = 2;
      }

      break;

    case 2:  // Going right
      npc->xvel += 0x40;

      // Turn around if far enough away from the player
      if (npc->x > player.x + (192 * 0x200)) npc->act = 1;

      // Turn around if touching a wall
      if (npc->flags & 4) {
        npc->xvel = 0;
        npc->act = 1;
      }

      break;
  }

  // Face direction Bazil is moving
  if (npc->xvel < 0)
    npc->dir = 0;
  else
    npc->dir = 2;

  // Cap speed
  if (npc->xvel > 0x5FF) npc->xvel = 0x5FF;
  if (npc->xvel < -0x5FF) npc->xvel = -0x5FF;

  // Apply momentum
  npc->x += npc->xvel;

  // Increment animation
  if (++npc->anim_wait > 1) {
    npc->anim_wait = 0;
    ++npc->anim;
  }

  // Loop animation
  if (npc->anim > 2) npc->anim = 0;

  // Update sprite
  if (npc->dir == 0)
    npc->rect = &rc_left[npc->anim];
  else
    npc->rect = &rc_right[npc->anim];
}

// Beetle (Follows you, Egg Corridor)
void npc_act_008(npc_t *npc) {
  static const rect_t rc_left[] = {
    {80, 80, 96, 96},
    {96, 80, 112, 96},
  };
  static const rect_t rc_right[] = {
    {80, 96, 96, 112},
    {96, 96, 112, 112}
  };

  switch (npc->act) {
    case 0:
      if (player.x < npc->x + (16 * 0x200) && player.x > npc->x - (16 * 0x200)) {
        npc->bits |= NPC_SHOOTABLE;
        npc->yvel = -0x100;
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

      if (npc->yvel > 0x100) npc->yvel = 0x100;
      if (npc->yvel < -0x100) npc->yvel = -0x100;

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

// Balrog (drop-in)
void npc_act_009(npc_t *npc) {
  static const rect_t rc_left[] = {
    {0, 0, 40, 24},
    {80, 0, 120, 24},
    {120, 0, 160, 24},
  };
  static const rect_t rc_right[] = {
    {0, 24, 40, 48},
    {80, 24, 120, 48},
    {120, 24, 160, 48}
  };

  int i;

  switch (npc->act) {
    case 0:
      npc->act = 1;
      npc->anim = 2;
      // Fallthrough
    case 1:
      npc->yvel += 0x20;

      if (npc->count1 < 40) {
        ++npc->count1;
      } else {
        npc->bits &= ~NPC_IGNORE_SOLIDITY;
        npc->bits |= NPC_SOLID_SOFT;
      }

      if (npc->flags & 8) {
        for (i = 0; i < 4; ++i)
          npc_spawn(4, npc->x + (m_rand(-12, 12) * 0x200),
                    npc->y + (m_rand(-12, 12) * 0x200), m_rand(-341, 341),
                    m_rand(-0x600, 0), 0, NULL, 0x100);

        npc->act = 2;
        npc->anim = 1;
        npc->act_wait = 0;
        snd_play_sound(-1, 26, SOUND_MODE_PLAY);
        cam_start_quake_small(30);
      }

      break;

    case 2:
      if (++npc->act_wait > 16) {
        npc->act = 3;
        npc->anim = 0;
        npc->anim_wait = 0;
      }

      break;
  }

  if (npc->yvel > 0x5FF) npc->yvel = 0x5FF;
  if (npc->yvel < -0x5FF) npc->yvel = -0x5FF;

  npc->x += npc->xvel;
  npc->y += npc->yvel;

  if (npc->dir == 0)
    npc->rect = &rc_left[npc->anim];
  else
    npc->rect = &rc_right[npc->anim];
}

// Balrog (shooting) (super-secret unused version from the prototype)
void npc_act_010(npc_t *npc) {
  static const rect_t rc_left[] = {
    {0, 0, 40, 24},
    {40, 0, 80, 24},
    {80, 0, 120, 24},
    {120, 0, 160, 24},
  };
  static const rect_t rc_right[] = {
    {0, 24, 40, 48},
    {40, 24, 80, 48},
    {80, 24, 120, 48},
    {120, 24, 160, 48}
  };

  u8 deg;

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

        npc_spawn(11, npc->x, npc->y + (4 * 0x200), m_sin(deg), m_cos(deg), 0, NULL, 0x100);

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
      if (npc->flags & 5) npc->xvel = 0;

      if (npc->y + (16 * 0x200) < player.y)
        npc->damage = 5;
      else
        npc->damage = 0;

      if (npc->flags & 8) {
        npc->act = 5;
        npc->act_wait = 0;
        npc->anim = 2;
        snd_play_sound(-1, 26, SOUND_MODE_PLAY);
        cam_start_quake_small(30);
        npc->damage = 0;
      }

      break;

    case 5:
      npc->xvel = 0;

      if (++npc->act_wait > 3) {
        npc->act = 1;
        npc->act_wait = 0;
      }

      break;
  }

  npc->yvel += 0x20;
  if (npc->yvel > 0x5FF) npc->yvel = 0x5FF;

  npc->x += npc->xvel;
  npc->y += npc->yvel;

  if (npc->x < player.x)
    npc->dir = 2;
  else
    npc->dir = 0;

  if (npc->dir == 0)
    npc->rect = &rc_left[npc->anim];
  else
    npc->rect = &rc_right[npc->anim];
}

// Proto-Balrog's projectile
void npc_act_011(npc_t *npc) {
  static const rect_t rect[] = {
    {208, 104, 224, 120},
    {224, 104, 240, 120},
    {240, 104, 256, 120}
  };

  if (npc->flags & 0xFF) {
    npc->cond = 0;
    caret_spawn(npc->x, npc->y, CARET_PROJECTILE_DISSIPATION, DIR_LEFT);
  }

  npc->y += npc->yvel;
  npc->x += npc->xvel;

  if (++npc->anim_wait > 1) {
    npc->anim_wait = 0;

    if (++npc->anim > 2) npc->anim = 0;
  }

  npc->rect = &rect[npc->anim];

  if (++npc->count1 > 150) {
    caret_spawn(npc->x, npc->y, CARET_PROJECTILE_DISSIPATION, DIR_LEFT);
    npc->cond = 0;
  }
}

// Balrog (cutscene)
void npc_act_012(npc_t *npc) {
  static const rect_t rc_left[] = {
    {0, 0, 40, 24},    {160, 0, 200, 24}, {80, 0, 120, 24},
    {120, 0, 160, 24}, {240, 0, 280, 24}, {200, 0, 240, 24},
    {280, 0, 320, 24}, {0, 0, 0, 0},      {80, 48, 120, 72},
    {0, 48, 40, 72},   {0, 0, 40, 24},    {40, 48, 80, 72},
    {0, 0, 40, 24},    {280, 0, 320, 24},
  };
  static const rect_t rc_right[] = {
    {0, 24, 40, 48},    {160, 24, 200, 48}, {80, 24, 120, 48},
    {120, 24, 160, 48}, {240, 24, 280, 48}, {200, 24, 240, 48},
    {280, 24, 320, 48}, {0, 0, 0, 0},       {80, 72, 120, 96},
    {0, 72, 40, 96},    {0, 24, 40, 48},    {40, 72, 80, 96},
    {0, 24, 40, 48},    {280, 24, 320, 48}
  };

  int i;
  int x, y;

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

    case 10:
      if (npc->dir == 4) {
        if (npc->x > player.x)
          npc->dir = 0;
        else
          npc->dir = 2;
      }

      npc->act = 11;
      npc->anim = 2;
      npc->act_wait = 0;
      npc->tgt_x = 0;
      // Fallthrough
    case 11:
      if (++npc->act_wait > 30) {
        npc->act = 12;
        npc->act_wait = 0;
        npc->anim = 3;
        npc->yvel = -0x800;
        npc->bits |= NPC_IGNORE_SOLIDITY;
      }

      break;

    case 12:
      if (npc->flags & 5) npc->xvel = 0;

      if (npc->y < 0) {
        npc->class_num = 0;
        snd_play_sound(-1, 26, SOUND_MODE_PLAY);
        cam_start_quake_small(30);
      }

      break;

    case 20:
      if (npc->dir == 4) {
        if (npc->x > player.x)
          npc->dir = 0;
        else
          npc->dir = 2;
      }

      npc->act = 21;
      npc->anim = 5;
      npc->act_wait = 0;
      npc->count1 = 0;

      for (i = 0; i < 4; ++i) {
        npc_spawn(4,
          npc->x + (m_rand(-12, 12) * 0x200),
          npc->y + (m_rand(-12, 12) * 0x200),
          m_rand(-341, 341),
          m_rand(-0x600, 0),
          0, NULL, 0x100
        );
      }

      snd_play_sound(-1, 72, SOUND_MODE_PLAY);
      // Fallthrough
    case 21:
      npc->tgt_x = 1;

      if (npc->flags & 8) ++npc->act_wait;

      if (++npc->count1 / 2 % 2)
        npc->x += 1 * 0x200;
      else
        npc->x -= 1 * 0x200;

      if (npc->act_wait > 100) {
        npc->act = 11;
        npc->act_wait = 0;
        npc->anim = 2;
      }

      npc->yvel += 0x20;
      if (npc->yvel > 0x5FF) npc->yvel = 0x5FF;

      break;

    case 30:
      npc->anim = 4;

      if (++npc->act_wait > 100) {
        npc->act = 0;
        npc->anim = 0;
      }

      break;

    case 40:
      if (npc->dir == 4) {
        if (npc->x > player.x)
          npc->dir = 0;
        else
          npc->dir = 2;
      }

      npc->act = 41;
      npc->act_wait = 0;
      npc->anim = 5;
      // Fallthrough
    case 41:
      if (++npc->anim_wait / 2 % 2)
        npc->anim = 5;
      else
        npc->anim = 6;

      break;

    case 42:
      if (npc->dir == 4) {
        if (npc->x > player.x)
          npc->dir = 0;
        else
          npc->dir = 2;
      }

      npc->act = 43;
      npc->act_wait = 0;
      npc->anim = 6;
      // Fallthrough
    case 43:
      if (++npc->anim_wait / 2 % 2)
        npc->anim = 7;
      else
        npc->anim = 6;

      break;

    case 50:
      npc->anim = 8;
      npc->xvel = 0;
      break;

    case 60:
      npc->act = 61;
      npc->anim = 9;
      npc->anim_wait = 0;
      // Fallthrough
    case 61:
      if (++npc->anim_wait > 3) {
        npc->anim_wait = 0;

        if (++npc->anim == 10 || npc->anim == 11)
          snd_play_sound(-1, 23, SOUND_MODE_PLAY);
      }

      if (npc->anim > 12) npc->anim = 9;

      if (npc->dir == 0)
        npc->xvel = -0x200;
      else
        npc->xvel = 0x200;

      break;

    case 70:
      npc->act = 71;
      npc->act_wait = 64;
      snd_play_sound(-1, 29, SOUND_MODE_PLAY);
      npc->anim = 13;
      // Fallthrough
    case 71:
      if (--npc->act_wait == 0) npc->cond = 0;

      break;

    case 80:
      npc->count1 = 0;
      npc->act = 81;
      // Fallthrough
    case 81:
      if (++npc->count1 / 2 % 2)
        npc->x += 1 * 0x200;
      else
        npc->x -= 1 * 0x200;

      npc->anim = 5;
      npc->xvel = 0;
      npc->yvel += 0x20;

      break;

    case 100:
      npc->act = 101;
      npc->act_wait = 0;
      npc->anim = 2;
      // Fallthrough
    case 101:
      if (++npc->act_wait > 20) {
        npc->act = 102;
        npc->act_wait = 0;
        npc->anim = 3;
        npc->yvel = -0x800;
        npc->bits |= NPC_IGNORE_SOLIDITY;
        npc_delete_by_class(150, FALSE);
        npc_delete_by_class(117, FALSE);
        npc_spawn(355, 0, 0, 0, 0, 0, npc, 0x100);
        npc_spawn(355, 0, 0, 0, 0, 1, npc, 0x100);
      }

      break;

    case 102: {
      x = npc->x / 0x200 / 0x10;
      y = npc->y / 0x200 / 0x10;

      if (y >= 0 && y < 35 && stage_set_tile(x, y, 0)) {
        stage_set_tile(x - 1, y, 0);
        stage_set_tile(x + 1, y, 0);
        snd_play_sound(-1, 44, SOUND_MODE_PLAY);
        cam_start_quake_big(10);
      }

      if (npc->y < -32 * 0x200) {
        npc->class_num = 0;
        cam_start_quake_small(30);
      }

      break;
    }
  }

  if (npc->tgt_x && m_rand(0, 10) == 0) {
    npc_spawn(
      4,
      npc->x + (m_rand(-12, 12) * 0x200),
      npc->y + (m_rand(-12, 12) * 0x200),
      m_rand(-341, 341),
      m_rand(-0x600, 0),
      0, NULL, 0x100
    );
  }

  if (npc->yvel > 0x5FF) npc->yvel = 0x5FF;

  npc->x += npc->xvel;
  npc->y += npc->yvel;

  if (npc->dir == 0)
    npc->rect = &rc_left[npc->anim];
  else
    npc->rect = &rc_right[npc->anim];

  if (npc->act == 71) {
    npc->rect_delta.bottom = npc->rect_delta.top + npc->act_wait / 2 - npc->rect->bottom;
    if (npc->act_wait % 2)
      npc->rect_delta.left = 1;
    npc->rect_prev = NULL; // force texrect update
  }
}

// Forcefield
void npc_act_013(npc_t *npc) {
  static const rect_t rect[] = {
    {128, 0, 144, 16},
    {144, 0, 160, 16},
    {160, 0, 176, 16},
    {176, 0, 192, 16}
  };

  if (++npc->anim_wait > 0) {
    npc->anim_wait = 0;
    ++npc->anim;
  }

  if (npc->anim > 3) npc->anim = 0;

  npc->rect = &rect[npc->anim];
}

// Santa's Key
void npc_act_014(npc_t *npc) {
  static const rect_t rect[] = {
    {192, 0, 208, 16},
    {208, 0, 224, 16},
    {224, 0, 240, 16}
  };

  int i;

  switch (npc->act) {
    case 0:
      npc->act = 1;

      if (npc->dir == 2) {
        npc->yvel = -0x200;

        for (i = 0; i < 4; ++i) {
          npc_spawn(
            4,
            npc->x + (m_rand(-12, 12) * 0x200),
            npc->y + (m_rand(-12, 12) * 0x200),
            m_rand(-341, 341),
            m_rand(-0x600, 0),
            0, NULL, 0x100
          );
        }
      }

      break;
  }

  if (++npc->anim_wait > 1) {
    npc->anim_wait = 0;
    ++npc->anim;
  }

  if (npc->anim > 2) npc->anim = 0;

  npc->yvel += 0x40;
  if (npc->yvel > 0x5FF) npc->yvel = 0x5FF;

  npc->y += npc->yvel;

  npc->rect = &rect[npc->anim];
}

// Chest (closed)
void npc_act_015(npc_t *npc) {
  static const rect_t rect[] = {
    {240, 0, 256, 16},
    {256, 0, 272, 16},
    {272, 0, 288, 16}
  };

  int i;

  switch (npc->act) {
    case 0:
      npc->act = 1;
      npc->bits |= NPC_INTERACTABLE;

      if (npc->dir == 2) {
        npc->yvel = -0x200;

        for (i = 0; i < 4; ++i)
          npc_spawn(4, npc->x + (m_rand(-12, 12) * 0x200),
                    npc->y + (m_rand(-12, 12) * 0x200), m_rand(-341, 341),
                    m_rand(-0x600, 0), 0, NULL, 0x100);
      }

      // Fallthrough
    case 1:
      npc->anim = 0;

      if (m_rand(0, 30) == 0) npc->act = 2;

      break;

    case 2:
      if (++npc->anim_wait > 1) {
        npc->anim_wait = 0;
        ++npc->anim;
      }

      if (npc->anim > 2) {
        npc->anim = 0;
        npc->act = 1;
      }

      break;
  }

  npc->yvel += 0x40;
  if (npc->yvel > 0x5FF) npc->yvel = 0x5FF;

  npc->y += npc->yvel;

  npc->rect = &rect[npc->anim];
}

// Save point
void npc_act_016(npc_t *npc) {
  static const rect_t rect[] = {
    {96, 16, 112, 32},  {112, 16, 128, 32}, {128, 16, 144, 32},
    {144, 16, 160, 32}, {160, 16, 176, 32}, {176, 16, 192, 32},
    {192, 16, 208, 32}, {208, 16, 224, 32}
  };

  int i;

  switch (npc->act) {
    case 0:
      npc->bits |= NPC_INTERACTABLE;
      npc->act = 1;

      if (npc->dir == 2) {
        npc->bits &= ~NPC_INTERACTABLE;
        npc->yvel = -0x200;

        for (i = 0; i < 4; ++i)
          npc_spawn(4, npc->x + (m_rand(-12, 12) * 0x200),
                    npc->y + (m_rand(-12, 12) * 0x200), m_rand(-341, 341),
                    m_rand(-0x600, 0), 0, NULL, 0x100);
      }

      // Fallthrough
    case 1:
      if (npc->flags & 8) npc->bits |= NPC_INTERACTABLE;

      break;
  }

  if (++npc->anim_wait > 2) {
    npc->anim_wait = 0;
    ++npc->anim;
  }

  if (npc->anim > 7) npc->anim = 0;

  npc->yvel += 0x40;
  if (npc->yvel > 0x5FF) npc->yvel = 0x5FF;

  npc->y += npc->yvel;

  npc->rect = &rect[npc->anim];
}

// Health refill
void npc_act_017(npc_t *npc) {
  static const rect_t rect[] = {
    {288, 0, 304, 16},
    {304, 0, 320, 16}
  };

  int a;

  switch (npc->act) {
    case 0:
      npc->act = 1;

      if (npc->dir == 2) {
        npc->yvel = -0x200;

        for (a = 0; a < 4; ++a)
          npc_spawn(4, npc->x + (m_rand(-12, 12) * 0x200),
                    npc->y + (m_rand(-12, 12) * 0x200), m_rand(-341, 341),
                    m_rand(-0x600, 0), 0, NULL, 0x100);
      }

      // Fallthrough
    case 1:
      a = m_rand(0, 30);

      if (a < 10)
        npc->act = 2;
      else if (a < 25)
        npc->act = 3;
      else
        npc->act = 4;

      npc->act_wait = m_rand(0x10, 0x40);
      npc->anim_wait = 0;
      break;

    case 2:
      npc->rect = &rect[0];

      if (--npc->act_wait == 0) npc->act = 1;

      break;

    case 3:
      if (++npc->anim_wait % 2)
        npc->rect = &rect[0];
      else
        npc->rect = &rect[1];

      if (--npc->act_wait == 0) npc->act = 1;

      break;

    case 4:
      npc->rect = &rect[1];

      if (--npc->act_wait == 0) npc->act = 1;

      break;
  }

  npc->yvel += 0x40;

  if (npc->yvel > 0x5FF) npc->yvel = 0x5FF;

  npc->y += npc->yvel;
}

// Door
void npc_act_018(npc_t *npc) {
  static const rect_t rect[] = {
    {224, 16, 240, 40},
    {192, 112, 208, 136}
  };

  int i;

  switch (npc->act) {
    case 0:
      if (npc->dir == 0)
        npc->rect = &rect[0];
      else
        npc->rect = &rect[1];
      break;

    case 1:
      for (i = 0; i < 4; ++i)
        npc_spawn(4, npc->x, npc->y, m_rand(-341, 341), m_rand(-0x600, 0), 0, NULL, 0x100);
      npc->act = 0;
      npc->rect = &rect[0];
      break;
  }
}

// Balrog (burst)
void npc_act_019(npc_t *npc) {
  static const rect_t rc_left[] = {
    {0, 0, 40, 24},
    {160, 0, 200, 24},
    {80, 0, 120, 24},
    {120, 0, 160, 24},
  };
  static const rect_t rc_right[] = {
    {0, 24, 40, 48},
    {160, 24, 200, 48},
    {80, 24, 120, 48},
    {120, 24, 160, 48}
  };

  int i;

  switch (npc->act) {
    case 0:
      for (i = 0; i < 0x10; ++i) {
        npc_spawn(
          4,
          npc->x + (m_rand(-12, 12) * 0x200),
          npc->y + (m_rand(-12, 12) * 0x200),
          m_rand(-341, 341),
          m_rand(-0x600, 0),
          0, NULL, 0x100
        );
      }
      npc->y += 10 * 0x200;
      npc->act = 1;
      npc->anim = 3;
      npc->yvel = -0x100;
      snd_play_sound(-1, 12, SOUND_MODE_PLAY);
      snd_play_sound(-1, 26, SOUND_MODE_PLAY);
      cam_start_quake_small(30);
      // Fallthrough
    case 1:
      npc->yvel += 0x10;

      if (npc->yvel > 0 && npc->flags & 8) {
        npc->act = 2;
        npc->anim = 2;
        npc->act_wait = 0;
        snd_play_sound(-1, 26, SOUND_MODE_PLAY);
        cam_start_quake_small(30);
      }

      break;

    case 2:
      if (++npc->act_wait > 0x10) {
        npc->act = 3;
        npc->anim = 0;
        npc->anim_wait = 0;
      }

      break;

    case 3:
      if (m_rand(0, 100) == 0) {
        npc->act = 4;
        npc->act_wait = 0;
        npc->anim = 1;
      }

      break;

    case 4:
      if (++npc->act_wait > 0x10) {
        npc->act = 3;
        npc->anim = 0;
      }

      break;
  }

  if (npc->yvel > 0x5FF) npc->yvel = 0x5FF;
  if (npc->yvel < -0x5FF) npc->yvel = -0x5FF;

  npc->x += npc->xvel;
  npc->y += npc->yvel;

  if (npc->dir == 0)
    npc->rect = &rc_left[npc->anim];
  else
    npc->rect = &rc_right[npc->anim];
}
