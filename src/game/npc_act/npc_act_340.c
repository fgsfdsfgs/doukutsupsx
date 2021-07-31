#include "game/npc_act/npc_act.h"

// Ballos
void npc_act_340(npc_t *npc) {
  int i;
  int x;

  switch (npc->act) {
    case 0:
      npc->act = 1;
      npc->cond = 0x80;
      npc->exp = 1;
      npc->dir = 0;
      npc->y -= 6 * 0x200;
      npc->damage = 0;
      npc_spawn(341, npc->x, npc->y - (16 * 0x200), 0, 0, 0, npc, 0x100);
      break;

    case 10:
      npc->act = 11;
      npc->act_wait = 0;
      // Fallthrough
    case 11:
      if (++npc->act_wait > 100) npc->act = 100;

      break;

    case 100:
      npc->act = 110;
      npc->act_wait = 0;
      npc->anim = 1;
      npc->anim_wait = 0;
      npc->damage = 4;
      npc->bits |= NPC_SHOOTABLE;
      // Fallthrough
    case 110:
      npc->act = 111;
      npc->damage = 3;
      npc->tgt_x = npc->life;
      // Fallthrough
    case 111:
      if (++npc->anim_wait > 10) {
        npc->anim_wait = 0;
        ++npc->anim;
      }

      if (npc->anim > 2) npc->anim = 1;

      ++npc->act_wait;

      if (npc->life < npc->tgt_x - 50 || npc->act_wait > 150) {
        switch (npc->count2 % 5) {
          case 0:
          case 1:
          case 2:
          case 3:
            npc->act = 200;
            break;

          case 4:
            npc->act = 300;
            break;
        }

        ++npc->count2;
      }

      if (player.x < npc->x)
        npc->dir = 0;
      else
        npc->dir = 2;

      break;

    case 200:
      npc->act = 201;
      npc->count1 = 0;
      // Fallthrough
    case 201:
      if (npc->xvel == 0)
        npc->act = 202;
      else
        npc->act = 203;

      npc->act_wait = 0;
      npc->anim = 3;
      npc->damage = 3;
      ++npc->count1;
      // Fallthrough
    case 202:
      if (player.x < npc->x)
        npc->dir = 0;
      else
        npc->dir = 2;

      npc->xvel = 8 * npc->xvel / 9;
      npc->yvel = 8 * npc->yvel / 9;

      if (++npc->act_wait > 20) npc->act = 210;

      break;

    case 203:
      npc->xvel = 8 * npc->xvel / 9;
      npc->yvel = 8 * npc->yvel / 9;

      if (++npc->act_wait > 20) {
        if (player.y < npc->y + (12 * 0x200))
          npc->act = 220;
        else
          npc->act = 230;
      }

      break;

    case 210:
      npc->act = 211;
      npc->act_wait = 0;
      npc->anim = 6;
      npc->anim_wait = 0;
      npc->yvel = 0;
      npc->damage = 10;

      if (player.x < npc->x)
        npc->dir = 0;
      else
        npc->dir = 2;

      snd_play_sound(-1, 25, SOUND_MODE_PLAY);
      // Fallthrough
    case 211:
      if (npc->dir == 0)
        npc->xvel = -0x800;
      else
        npc->xvel = 0x800;

      if (++npc->act_wait / 2 % 2)
        npc->anim = 6;
      else
        npc->anim = 7;

      if (npc->dir == 0 && npc->flags & 1) {
        npc->act = 212;
        npc->act_wait = 0;
        npc->damage = 3;
        // SetQuake2(10);
        snd_play_sound(-1, 26, SOUND_MODE_PLAY);
      }

      if (npc->dir == 2 && npc->flags & 4) {
        npc->act = 212;
        npc->act_wait = 0;
        npc->damage = 3;
        // SetQuake2(10);
        snd_play_sound(-1, 26, SOUND_MODE_PLAY);
      }

      if (npc->count1 < 4 && player.x > npc->x - (16 * 0x200) && player.x < npc->x + (16 * 0x200)) npc->act = 201;

      break;

    case 212:
      ++npc->act_wait;
      npc->xvel = 0;
      npc->anim = 6;

      if (npc->act_wait > 30) {
        if (npc->count1 > 3)
          npc->act = 240;
        else
          npc->act = 201;
      }

      break;

    case 220:
      npc->act = 221;
      npc->act_wait = 0;
      npc->anim = 8;
      npc->anim_wait = 0;
      npc->xvel = 0;
      npc->damage = 10;
      npc->dir = 0;
      snd_play_sound(-1, 25, SOUND_MODE_PLAY);
      // Fallthrough
    case 221:
      npc->yvel = -0x800;

      if (++npc->act_wait / 2 % 2)
        npc->anim = 8;
      else
        npc->anim = 9;

      if (npc->y < (48 * 0x200)) {
        npc->y = (48 * 0x200);
        npc->yvel = 0;
        npc->act = 222;
        npc->act_wait = 0;
        npc->damage = 3;

        for (i = 0; i < 8; ++i) {
          x = npc->x + (m_rand(-0x10, 0x10) * 0x200);
          npc_spawn(4, x, npc->y - (10 * 0x200), 0, 0, 0, NULL, 0x100);
        }

        npc_spawn(332, npc->x - (12 * 0x200), npc->y - (12 * 0x200), 0, 0, 0, NULL, 0x100);
        npc_spawn(332, npc->x + (12 * 0x200), npc->y - (12 * 0x200), 0, 0, 2, NULL, 0x100);
        // SetQuake2(10);
        snd_play_sound(-1, 26, SOUND_MODE_PLAY);
      }

      if (npc->count1 < 4 && player.y > npc->y - (16 * 0x200) && player.y < npc->y + (16 * 0x200)) npc->act = 201;

      break;

    case 222:
      ++npc->act_wait;
      npc->xvel = 0;
      npc->anim = 8;

      if (npc->act_wait > 30) {
        if (npc->count1 > 3)
          npc->act = 240;
        else
          npc->act = 201;
      }

      break;

    case 230:
      npc->act = 231;
      npc->act_wait = 0;
      npc->anim = 8;
      npc->anim_wait = 0;
      npc->xvel = 0;
      npc->damage = 10;
      npc->dir = 2;
      snd_play_sound(-1, 25, SOUND_MODE_PLAY);
      // Fallthrough
    case 231:
      npc->yvel = 0x800;

      if (++npc->act_wait / 2 % 2)
        npc->anim = 8;
      else
        npc->anim = 9;

      if (npc->flags & 8) {
        npc->act = 232;
        npc->act_wait = 0;
        npc->damage = 3;

        if (player.x < npc->x)
          npc->dir = 0;
        else
          npc->dir = 2;

        for (i = 0; i < 8; ++i) {
          x = npc->x + (m_rand(-0x10, 0x10) * 0x200);
          npc_spawn(4, x, npc->y + (10 * 0x200), 0, 0, 0, NULL, 0x100);
        }

        npc_spawn(332, npc->x - (12 * 0x200), npc->y + (12 * 0x200), 0, 0, 0, NULL, 0x100);
        npc_spawn(332, npc->x + (12 * 0x200), npc->y + (12 * 0x200), 0, 0, 2, NULL, 0x100);
        // SetQuake2(10);
        snd_play_sound(-1, 26, SOUND_MODE_PLAY);
      }

      if (npc->count1 < 4 && player.y > npc->y - (16 * 0x200) && player.y < npc->y + (16 * 0x200)) npc->act = 201;

      break;

    case 232:
      ++npc->act_wait;
      npc->xvel = 0;
      npc->anim = 3;

      if (npc->act_wait > 30) {
        if (npc->count1 > 3)
          npc->act = 242;
        else
          npc->act = 201;
      }

      break;

    case 240:
      npc->act = 241;
      npc->dir = 0;
      // Fallthrough
    case 241:
      npc->yvel += 0x80;
      if (npc->yvel > 0x5FF) npc->yvel = 0x5FF;

      if (++npc->anim_wait / 2 % 2)
        npc->anim = 4;
      else
        npc->anim = 5;

      if (npc->flags & 8) {
        npc->act = 242;
        npc->act_wait = 0;
        npc->anim = 3;

        if (player.x < npc->x)
          npc->dir = 0;
        else
          npc->dir = 2;
      }

      break;

    case 242:
      npc->xvel = 3 * npc->xvel / 4;
      npc->anim = 3;

      if (++npc->act_wait > 10) npc->act = 110;

      break;

    case 300:
      npc->act = 310;
      npc->act_wait = 0;
      npc->yvel = -0x600;

      if (npc->x > 320 * 0x200) {
        npc->dir = 2;
        npc->tgt_x = player.x;
        npc->tgt_y = 176 * 0x200;
      } else {
        npc->dir = 0;
        npc->tgt_x = player.x;
        npc->tgt_y = 176 * 0x200;
      }

      npc->anim_wait = 0;
      // Fallthrough
    case 310:
      ++npc->anim_wait;

      if (++npc->act_wait > 200 && npc->anim_wait < 20)
        npc->dir = 2;
      else
        npc->dir = 0;

      if (npc->anim_wait / 2 % 2)
        npc->anim = 4;
      else
        npc->anim = 5;

      if (npc->x < npc->tgt_x)
        npc->xvel += 0x40;
      else
        npc->xvel -= 0x40;

      if (npc->y < npc->tgt_y)
        npc->yvel += 0x40;
      else
        npc->yvel -= 0x40;

      if (npc->xvel > 0x400) npc->xvel = 0x400;
      if (npc->xvel < -0x400) npc->xvel = -0x400;

      if (npc->yvel > 0x400) npc->yvel = 0x400;
      if (npc->yvel < -0x400) npc->yvel = -0x400;

      if (npc->act_wait > 200 && npc->act_wait % 40 == 1) {
        npc->anim_wait = 0;
        npc_spawn(333, player.x, 304 * 0x200, 0, 0, 0, NULL, 0x100);
      }

      if (npc->act_wait > 480) {
        npc->act = 320;
        npc->act_wait = 0;
      }

      break;

    case 320:
      npc->xvel = 0;
      npc->yvel = 0;
      npc->dir = 2;

      if (++npc->act_wait == 40) // SetFlash(0, 0, FLASH_MODE_FLASH);

      if (npc->act_wait > 50 && npc->act_wait % 10 == 1) {
        x = ((4 * npc->act_wait - 200) / 10 + 2) * 0x200 * 0x10;
        npc_spawn(333, x, 304 * 0x200, 0, 0, 0, NULL, 0x100);
      }

      if (npc->act_wait > 140) npc->act = 240;

      if (++npc->anim_wait / 2 % 2)
        npc->anim = 4;
      else
        npc->anim = 5;

      break;

    case 1000:
      npc->act = 1001;
      npc->act_wait = 0;
      npc->anim = 10;
      npc->tgt_x = npc->x;
      npc->xvel = 0;
      npc->bits &= ~NPC_SHOOTABLE;
      npc_spawn_death_fx(npc->x, npc->y, 0x10, 0x10, 0);
      snd_play_sound(-1, 72, SOUND_MODE_PLAY);
      // Fallthrough
    case 1001:
      npc->yvel += 0x20;
      if (npc->yvel > 0x5FF) npc->yvel = 0x5FF;

      if (++npc->act_wait / 2 % 2)
        npc->x = npc->tgt_x + 0x200;
      else
        npc->x = npc->tgt_x - 0x200;

      if (npc->flags & 8) {
        npc->act = 1002;
        npc->act_wait = 0;
      }

      break;

    case 1002:
      if (++npc->act_wait > 150) {
        npc->act_wait = 0;
        npc->act = 1003;
        npc->anim = 3;
      }

      if (npc->act_wait / 2 % 2)
        npc->x = npc->tgt_x + 0x200;
      else
        npc->x = npc->tgt_x - 0x200;

      break;

    case 1003:
      if (++npc->act_wait > 30) {
        npc->act_wait = 0;
        npc->act = 1004;
        npc->anim = 3;
        npc->yvel -= 0xA00;
        npc->dir = 0;
        npc->bits |= NPC_IGNORE_SOLIDITY;
      }

      break;

    case 1004:
      if (npc->y < 0) {
        npc->xvel = 0;
        npc->yvel = 0;
        npc->act = 1005;
        npc->act_wait = 0;
        // SetFlash(0, 0, FLASH_MODE_FLASH);
        snd_play_sound(-1, 29, SOUND_MODE_PLAY);
      }

      if (++npc->act_wait / 2 % 2)
        npc->anim = 8;
      else
        npc->anim = 9;

      break;
  }

  npc->x += npc->xvel;
  npc->y += npc->yvel;

  static const rect_t rc_left[11] = {
    {0, 0, 48, 40},     {48, 0, 96, 40},     {96, 0, 144, 40},    {144, 0, 192, 40},
    {192, 0, 240, 40},  {240, 0, 288, 40},   {0, 80, 48, 120},    {48, 80, 96, 120},
    {96, 80, 144, 120}, {144, 80, 192, 120}, {192, 80, 240, 120},
  };

  static const rect_t rc_right[11] = {
    {0, 40, 48, 80},     {48, 40, 96, 80},     {96, 40, 144, 80},    {144, 40, 192, 80},
    {192, 40, 240, 80},  {240, 40, 288, 80},   {0, 120, 48, 160},    {48, 120, 96, 160},
    {96, 120, 144, 160}, {144, 120, 192, 160}, {192, 120, 240, 160},
  };

  if (npc->dir == 0)
    npc->rect = &rc_left[npc->anim];
  else
    npc->rect = &rc_right[npc->anim];
}

// Ballos 1 head
void npc_act_341(npc_t *npc) {
  static const rect_t rc[3] = {
    {288, 32, 320, 48},
    {288, 48, 320, 64},
    {288, 64, 320, 80},
  };

  if (npc->parent->act == 11 && npc->parent->act_wait > 50) ++npc->anim_wait;

  if (npc->anim_wait > 4) {
    npc->anim_wait = 0;

    if (npc->anim < 2) ++npc->anim;
  }

  if (npc->parent->anim) npc->cond = 0;

  npc->rect = &rc[npc->anim];
}

// Ballos 3 eye
void npc_act_342(npc_t *npc) {
  static int flash;

  static const rect_t rc[3] = {
    {240, 48, 280, 88},
    {240, 88, 280, 128},
    {280, 48, 320, 88},
  };

  u8 deg;

  if (npc->act < 1000 && npc->parent->act >= 1000) npc->act = 1000;

  switch (npc->act) {
    case 0:
      npc->act = 10;
      npc->count1 = (npc->dir & 0xFF) * 2;
      npc->dir >>= 8;
      npc->count2 = 0xC0;
      npc->damage = 14;
      // Fallthrough
    case 10:
      if (npc->count2 < 320)
        npc->count2 += 8;
      else
        npc->act = 11;

      break;

    case 11:
      if (npc->count2 > 304)
        npc->count2 -= 4;
      else
        npc->act = 12;

      break;

    case 12:
      if (npc->parent->act == 311) npc->act = 20;

      break;

    case 20:
      npc->act = 21;
      npc->bits |= NPC_SHOOTABLE;
      npc->life = 1000;
      // Fallthrough
    case 21:
      npc->count1 -= 2;

      if (npc->count1 < 0) npc->count1 += 0x200;

      if (npc->shock) {
        if (++flash / 2 % 2)
          npc->anim = 1;
        else
          npc->anim = 0;
      } else {
        npc->anim = 0;
      }

      if (npc->life < 900) {
        npc->act = 22;
        npc->bits &= ~NPC_SHOOTABLE;
        npc_spawn_death_fx(npc->x, npc->y, 0x2000, 0x20, 0);
        snd_play_sound(-1, 71, SOUND_MODE_PLAY);
      }

      npc->parent->count1 = 4;

      if (npc->parent->act == 401) npc->act = 23;

      break;

    case 22:
      npc->anim = 2;
      npc->count1 -= 2;

      if (npc->count1 < 0) npc->count1 += 0x200;

      if (npc->parent->act == 401) npc->act = 23;

      break;

    case 23:
      npc->anim = 2;
      npc->count1 -= 4;

      if (npc->count1 < 0) npc->count1 += 0x200;

      if (npc->parent->act == 420) npc->act = 30;

      break;

    case 30:
      npc->act = 31;
      npc->life = 1000;
      npc->damage = 10;

      if (npc->dir == 0) npc->bits |= NPC_SHOOTABLE;

      npc->yvel = 0;
      // Fallthrough
    case 31:
      ++npc->count1;
      npc->count1 %= 0x200;

      if (npc->count2 > 0x100) --npc->count2;

      if (npc->bits & NPC_SHOOTABLE) {
        if (npc->shock) {
          if (++flash / 2 % 2)
            npc->anim = 1;
          else
            npc->anim = 0;
        } else {
          npc->anim = 0;
        }
      } else {
        npc->anim = 2;
      }

      if (npc->life < 900) npc->act = 40;

      break;

    case 40:
      npc->act = 41;
      npc->xvel = 0;
      npc->yvel = 0;
      npc->anim = 2;
      npc->damage = 5;
      npc->bits &= ~(NPC_IGNORE_SOLIDITY | NPC_SHOOTABLE);
      npc_spawn_death_fx(npc->x, npc->y, 0x2000, 0x20, 0);
      snd_play_sound(-1, 71, SOUND_MODE_PLAY);
      // Fallthrough
    case 41:
      if (npc->flags & 1) npc->xvel = 0x100;

      if (npc->flags & 4) npc->xvel = -0x100;

      if (npc->flags & 8) {
        if (npc->xvel == 0) {
          if (player.x < npc->x)
            npc->xvel = 0x100;
          else
            npc->xvel = -0x100;
        }

        npc->yvel = -0x800;
        snd_play_sound(-1, 26, SOUND_MODE_PLAY);
      }

      npc->yvel += 0x20;
      if (npc->yvel > 0x5FF) npc->yvel = 0x5FF;

      break;

    case 1000:
      npc->act = 1001;
      npc->xvel = 0;
      npc->yvel = 0;
      npc->anim = 2;
      npc->bits &= ~(NPC_IGNORE_SOLIDITY | NPC_SHOOTABLE);
      npc->damage = 0;
      npc->count1 /= 4;
      npc->exp = 0;
      // Fallthrough
    case 1001:
      if (npc->count1 > 0) {
        if (--npc->count1 / 2 % 2)
          npc->anim = 1;
        else
          npc->anim = 0;
      } else {
        npc_spawn_death_fx(npc->x, npc->y, 0x2000, 0x20, 0);
        snd_play_sound(-1, 71, SOUND_MODE_PLAY);
        npc_show_death_damage(npc);
        return;
      }

      break;
  }

  if (npc->act == 21 || npc->act == 22) {
    switch (npc->parent->dir) {
      case 0:
        if (npc->count1 == 140) {
          npc_spawn(4, npc->x + (8 * 0x200), npc->y + 0x1800, 0, 0, 0, NULL, 0x100);
          npc_spawn(4, npc->x - (8 * 0x200), npc->y + 0x1800, 0, 0, 0, NULL, 0x100);
          snd_play_sound(-1, 26, SOUND_MODE_PLAY);
        }
        break;

      case 1:
        if (npc->count1 == 268) {
          npc_spawn(4, npc->x - (12 * 0x200), npc->y + (8 * 0x200), 0, 0, 0, NULL, 0x100);
          npc_spawn(4, npc->x - (12 * 0x200), npc->y - (8 * 0x200), 0, 0, 0, NULL, 0x100);
          snd_play_sound(-1, 26, SOUND_MODE_PLAY);
        }
        break;

      case 2:
        if (npc->count1 == 396) {
          npc_spawn(4, npc->x + (8 * 0x200), npc->y - (12 * 0x200), 0, 0, 0, NULL, 0x100);
          npc_spawn(4, npc->x - (8 * 0x200), npc->y - (12 * 0x200), 0, 0, 0, NULL, 0x100);
          npc_spawn(345, npc->x - (8 * 0x200), npc->y - (12 * 0x200), 0, 0, 0, NULL, 0x100);
          snd_play_sound(-1, 26, SOUND_MODE_PLAY);
        }
        break;

      case 3:
        if (npc->count1 == 12) {
          npc_spawn(4, npc->x + (12 * 0x200), npc->y + (8 * 0x200), 0, 0, 0, NULL, 0x100);
          npc_spawn(4, npc->x + (12 * 0x200), npc->y - (8 * 0x200), 0, 0, 0, NULL, 0x100);
          snd_play_sound(-1, 26, SOUND_MODE_PLAY);
        }
        break;
    }
  }

  if (npc->act < 40) {
    deg = npc->count1 / 2;

    npc->tgt_x = npc->parent->x + npc->count2 * m_cos(deg) / 4;
    npc->tgt_y = npc->parent->y + npc->count2 * m_sin(deg) / 4;

    npc->xvel = npc->tgt_x - npc->x;
    npc->yvel = npc->tgt_y - npc->y;
  }

  npc->x += npc->xvel;
  npc->y += npc->yvel;

  npc->rect = &rc[npc->anim];
}

// Ballos 2 cutscene
void npc_act_343(npc_t *npc) {
  static const rect_t rc = {0, 0, 120, 120};

  npc->rect = &rc;

  if (++npc->act_wait > 100) npc->cond = 0;

  npc->x = npc->parent->x;
  npc->y = npc->parent->y;
}

// Ballos 2 eyes
void npc_act_344(npc_t *npc) {
  static const rect_t rc[2] = {
    {272, 0, 296, 16},
    {296, 0, 320, 16},
  };

  if (npc->dir == 0) {
    npc->rect = &rc[0];
    npc->x = npc->parent->x - (24 * 0x200);
  } else {
    npc->rect = &rc[1];
    npc->x = npc->parent->x + (24 * 0x200);
  }

  if (++npc->act_wait > 100) npc->cond = 0;

  npc->y = npc->parent->y - (36 * 0x200);
}

// Ballos skull projectile
void npc_act_345(npc_t *npc) {
  static const rect_t rc[4] = {
    {128, 176, 144, 192},
    {144, 176, 160, 192},
    {160, 176, 176, 192},
    {176, 176, 192, 192},
  };

  int i;

  switch (npc->act) {
    case 0:
      npc->act = 100;
      npc->anim = m_rand(0, 16) % 4;
      // Fallthrough
    case 100:
      npc->yvel += 0x40;
      if (npc->yvel > 0x700) npc->yvel = 0x700;

      if (npc->y > 128 * 0x200) npc->bits &= ~NPC_IGNORE_SOLIDITY;

      if (npc->act_wait++ / 2 % 2) npc_spawn(4, npc->x, npc->y, 0, 0, 0, NULL, 0x100);

      if (npc->flags & 8) {
        npc->yvel = -0x200;
        npc->act = 110;
        npc->bits |= NPC_IGNORE_SOLIDITY;
        snd_play_sound(-1, 12, SOUND_MODE_PLAY);
        // SetQuake(10);

        for (i = 0; i < 4; ++i)
          npc_spawn(4, npc->x + (m_rand(-12, 12) * 0x200), npc->y + (16 * 0x200), m_rand(-341, 341),
                    m_rand(-0x600, 0), 0, NULL, 0x100);
      }

      break;

    case 110:
      npc->yvel += 0x40;

      if (npc->y > (stage_data->height * 0x200 * 0x10) + (2 * 0x200 * 0x10)) {
        npc->cond = 0;
        return;
      }

      break;
  }

  if (++npc->anim_wait > 8) {
    npc->anim_wait = 0;
    ++npc->anim;
  }

  if (npc->anim > 3) npc->anim = 0;

  npc->y += npc->yvel;

  npc->rect = &rc[npc->anim];
}

// Ballos 4 orbiting platform
void npc_act_346(npc_t *npc) {
  static const rect_t rc = {240, 0, 272, 16};
  u8 deg;

  if (npc->act < 1000 && npc->parent->act >= 1000) npc->act = 1000;

  switch (npc->act) {
    case 0:
      npc->act = 10;
      npc->count1 = npc->dir * 4;
      npc->count2 = 192;
      npc->anim = 0;
      // Fallthrough
    case 10:
      if (npc->count2 < 448)
        npc->count2 += 8;
      else
        npc->act = 11;

      break;

    case 11:
      if (npc->parent->act == 411) npc->act = 20;

      break;

    case 20:
      if (--npc->count1 < 0) npc->count1 += 0x400;

      if (npc->parent->act == 421) npc->act = 40;
      if (npc->parent->act == 423) npc->act = 100;

      break;

    case 30:
      ++npc->count1;
      npc->count1 %= 0x400;

      if (npc->parent->act == 425) npc->act = 50;
      if (npc->parent->act == 427) npc->act = 100;

      break;

    case 40:
      npc->count1 -= 2;

      if (npc->count1 < 0) npc->count1 += 0x400;

      if (npc->parent->act == 422) npc->act = 20;

      break;

    case 50:
      npc->count1 += 2;
      npc->count1 %= 0x400;

      if (npc->parent->act == 426) npc->act = 30;

      break;

    case 100:
      npc->anim = 0;

      if (npc->parent->act == 424) npc->act = 30;
      if (npc->parent->act == 428) npc->act = 20;

      break;

    case 1000:
      npc->act = 1001;
      npc->xvel = 0;
      npc->yvel = 0;
      npc->bits &= ~NPC_SOLID_HARD;
      // Fallthrough
    case 1001:
      npc->yvel += 0x40;

      if (npc->y > stage_data->height * 0x10 * 0x200)
        npc->cond = 0;

      break;
  }

  if (npc->act < 1000) {
    if (player.y > npc->y - 0x1000 && player.yvel < 0)
      npc->bits &= ~NPC_SOLID_HARD;
    else
      npc->bits |= NPC_SOLID_HARD;

    deg = npc->count1 / 4;
    npc->tgt_x = npc->parent->x + npc->count2 * m_cos(deg) / 4;
    npc->tgt_y = npc->parent->y + (16 * 0x200) + npc->count2 * m_sin(deg) / 4;

    npc->xvel = npc->tgt_x - npc->x;

    if (npc->act == 20 || npc->act == 30) {
      if (npc->count1 % 4 == 0) npc->anim = (npc->tgt_y - npc->y) / 4;
    } else if (npc->act == 40 || npc->act == 50) {
      if ((npc->count1 / 2 % 2) == 0) npc->anim = (npc->tgt_y - npc->y) / 2;
    } else {
      npc->anim = npc->tgt_y - npc->y;
    }

    npc->yvel = npc->anim;
  }

  npc->x += npc->xvel;
  npc->y += npc->yvel;

  npc->rect = &rc;
}

// Hoppy
void npc_act_347(npc_t *npc) {
  switch (npc->act) {
    case 0:
      npc->act = 1;
      // Fallthrough
    case 1:
      npc->anim = 0;

      if (player.y < npc->y + (128 * 0x200) && player.y > npc->y - (128 * 0x200)) {
        npc->act = 10;
        npc->act_wait = 0;
        npc->anim = 1;
      }

      break;

    case 10:
      if (++npc->act_wait == 4) npc->anim = 2;

      if (npc->act_wait > 12) {
        npc->act = 12;
        npc->xvel = 0x700;
        snd_play_sound(-1, 6, SOUND_MODE_PLAY);
        npc->anim = 3;
      }

      break;

    case 12:
      if (player.y < npc->y)
        npc->yvel = -0xAA;
      else
        npc->yvel = 0xAA;

      if (npc->flags & 1) {
        npc->act = 13;
        npc->act_wait = 0;
        npc->anim = 2;
        npc->xvel = 0;
        npc->yvel = 0;
        break;
      }

      npc->xvel -= 42;

      if (npc->xvel < -0x5FF) npc->xvel = -0x5FF;

      npc->x += npc->xvel;
      npc->y += npc->yvel;
      break;

    case 13:
      ++npc->act_wait;

      if (npc->act_wait == 2) npc->anim = 1;

      if (npc->act_wait == 6) npc->anim = 0;

      if (npc->act_wait > 16) npc->act = 1;

      break;
  }

  static const rect_t rc[4] = {
    {256, 48, 272, 64},
    {272, 48, 288, 64},
    {288, 48, 304, 64},
    {304, 48, 320, 64},
  };

  npc->rect = &rc[npc->anim];
}

// Ballos 4 spikes
void npc_act_348(npc_t *npc) {
  static const rect_t rc[2] = {
    {128, 152, 160, 176},
    {160, 152, 192, 176},
  };

  switch (npc->act) {
    case 0:
      npc->act = 1;
      // Fallthrough
    case 1:
      if (++npc->act_wait < 0x80) {
        npc->y -= 0x80;

        if (npc->act_wait / 2 % 2)
          npc->anim = 1;
        else
          npc->anim = 0;
      } else {
        npc->act = 10;
        npc->anim = 0;
        npc->damage = 2;
      }

      break;
  }

  npc->rect = &rc[npc->anim];
}

// Statue
void npc_act_349(npc_t *npc) {
  static const rect_t rect = {0, 0, 16, 16};

  if (npc->act == 0) {
    npc->act = 1;

    if (npc->dir == 0) npc->x += 8 * 0x200;
    if (npc->dir == 2) npc->y += 16 * 0x200;
  }

  npc->rect = &rect;
}

// Flying Bute archer
void npc_act_350(npc_t *npc) {
  static const rect_t rc_left[7] = {
    {0, 160, 24, 184},   {24, 160, 48, 184},   {48, 160, 72, 184},   {72, 160, 96, 184},
    {96, 160, 120, 184}, {120, 160, 144, 184}, {144, 160, 168, 184},
  };

  static const rect_t rc_right[7] = {
    {0, 184, 24, 208},   {24, 184, 48, 208},   {48, 184, 72, 208},   {72, 184, 96, 208},
    {96, 184, 120, 208}, {120, 184, 144, 208}, {144, 184, 168, 208},
  };

  switch (npc->act) {
    case 0:
      npc->act = 1;

      if (npc->dir == 0)
        npc->tgt_x = npc->x - (128 * 0x200);
      else
        npc->tgt_x = npc->x + (128 * 0x200);

      npc->tgt_y = npc->y;
      npc->yvel = m_rand(-0x200, 0x200) * 2;
      npc->xvel = m_rand(-0x200, 0x200) * 2;
      // Fallthrough
    case 1:
      if (++npc->anim_wait > 1) {
        npc->anim_wait = 0;

        if (++npc->anim > 1) npc->anim = 0;
      }

      if (npc->dir == 0) {
        if (npc->x < npc->tgt_x) npc->act = 20;
      } else {
        if (npc->x > npc->tgt_x) npc->act = 20;
      }

      break;

    case 20:
      npc->act = 21;
      npc->act_wait = m_rand(0, 150);
      npc->anim = 2;
      npc->anim_wait = 0;
      // Fallthrough
    case 21:
      if (++npc->anim_wait > 2) {
        npc->anim_wait = 0;

        if (++npc->anim > 3) npc->anim = 2;
      }

      if (++npc->act_wait > 300) npc->act = 30;

      if (player.x < npc->x + (112 * 0x200) && player.x > npc->x - (112 * 0x200) && player.y < npc->y + (16 * 0x200) &&
          player.y > npc->y - (16 * 0x200))
        npc->act = 30;

      break;

    case 30:
      npc->act = 31;
      npc->act_wait = 0;
      npc->anim_wait = 0;
      // Fallthrough
    case 31:
      if (++npc->anim_wait / 2 % 2)
        npc->anim = 3;
      else
        npc->anim = 4;

      if (++npc->act_wait > 30) {
        npc->act = 40;
        npc->anim = 5;

        if (npc->dir == 0)
          npc_spawn(312, npc->x, npc->y, -0x800, 0, 0, NULL, 0x199);
        else
          npc_spawn(312, npc->x, npc->y, 0x800, 0, 2, NULL, 0x199);
      }

      break;

    case 40:
      npc->act = 41;
      npc->act_wait = 0;
      npc->anim_wait = 0;
      // Fallthrough
    case 41:
      if (++npc->anim_wait > 2) {
        npc->anim_wait = 0;

        if (++npc->anim > 6) npc->anim = 5;
      }

      if (++npc->act_wait > 40) {
        npc->act = 50;
        npc->anim = 0;
        npc->xvel = 0;
        npc->yvel = 0;
      }

      break;

    case 50:
      if (++npc->anim_wait > 1) {
        npc->anim_wait = 0;

        if (++npc->anim > 1) npc->anim = 0;
      }

      if (npc->dir == 0)
        npc->xvel -= 0x20;
      else
        npc->xvel += 0x20;

      if (npc->x < 0 || npc->x > stage_data->width * 0x200 * 0x10) {
        npc_show_death_damage(npc);
        return;
      }

      break;
  }

  if (npc->act < 50) {
    if (npc->x < npc->tgt_x) npc->xvel += 0x2A;
    if (npc->x > npc->tgt_x) npc->xvel -= 0x2A;

    if (npc->y < npc->tgt_y) npc->yvel += 0x2A;
    if (npc->y > npc->tgt_y) npc->yvel -= 0x2A;

    if (npc->xvel > 0x400) npc->xvel = 0x400;
    if (npc->xvel < -0x400) npc->xvel = -0x400;

    if (npc->yvel > 0x400) npc->yvel = 0x400;
    if (npc->yvel < -0x400) npc->yvel = -0x400;
  }

  npc->x += npc->xvel;
  npc->y += npc->yvel;

  if (npc->dir == 0)
    npc->rect = &rc_left[npc->anim];
  else
    npc->rect = &rc_right[npc->anim];
}

// Statue (shootable)
void npc_act_351(npc_t *npc) {
  static const rect_t rc[9] = {
    {0, 96, 32, 136},  {32, 96, 64, 136},  {64, 96, 96, 136},  {96, 96, 128, 136},  {128, 96, 160, 136},
    {0, 176, 32, 216}, {32, 176, 64, 216}, {64, 176, 96, 216}, {96, 176, 128, 216},
  };

  switch (npc->act) {
    case 0:
      npc->act = 1;
      npc->anim = npc->dir / 10;
      npc->x += 8 * 0x200;
      npc->y += 12 * 0x200;
      break;

    case 10:
      if (npc_get_flag(npc->event_flag)) {
        npc->act = 20;
      } else {
        npc->act = 11;
        npc->bits |= NPC_SHOOTABLE;
      }
      // Fallthrough
    case 11:
      if (npc->life <= 900) {
        npc_spawn(351, npc->x - (8 * 0x200), npc->y - (12 * 0x200), 0, 0, (npc->anim + 4) * 10, NULL, 0);
        npc->cond |= 8;
      }

      break;

    case 20:
      npc->anim += 4;
      npc->act = 1;
      break;
  }

  npc->rect = &rc[npc->anim];
}

// Ending characters
void npc_act_352(npc_t *npc) {
  switch (npc->act) {
    case 0:
      // Set state
      npc->act = 1;
      npc->anim = 0;
      npc->count1 = npc->dir / 100;
      npc->dir %= 100;

      // Set surfaces / offset
      switch (npc->count1) {
        case 7:
        case 8:
        case 9:
        case 12:
        case 13:
          npc->surf = SURFACE_ID_LEVEL_SPRITESET_1;
          break;
      }

      switch (npc->count1) {
        case 2:
        case 4:
        case 9:
        case 12:
          npc->view.top = 16 * 0x200;
          break;
      }

      // Balrog
      if (npc->count1 == 9) {
        npc->view.back = 20 * 0x200;
        npc->view.front = 20 * 0x200;
        npc->x -= 1 * 0x200;
      }

      // Spawn King's sword
      if (npc->count1 == 0) npc_spawn(145, 0, 0, 0, 0, 2, npc, 0x100);
      // Fallthrough
    case 1:
      npc->yvel += 0x40;
      if (npc->yvel > 0x5FF) npc->yvel = 0x5FF;

      if (npc->flags & 8) {
        npc->yvel = 0;
        npc->act = 2;
        npc->anim = 1;
      }

      npc->y += npc->yvel;

      break;
  }

  static const rect_t rc[28] = {
    {304, 48, 320, 64},   {224, 48, 240, 64},   {32, 80, 48, 96},     {0, 80, 16, 96},
    {224, 216, 240, 240}, {192, 216, 208, 240}, {48, 16, 64, 32},     {0, 16, 16, 32},
    {112, 192, 128, 216}, {80, 192, 96, 216},   {304, 0, 320, 16},    {224, 0, 240, 16},
    {176, 32, 192, 48},   {176, 32, 192, 48},   {240, 16, 256, 32},   {224, 16, 240, 32},
    {208, 16, 224, 32},   {192, 16, 208, 32},   {280, 128, 320, 152}, {280, 152, 320, 176},
    {32, 112, 48, 128},   {0, 112, 16, 128},    {80, 0, 96, 16},      {112, 0, 128, 16},
    {16, 152, 32, 176},   {0, 152, 16, 176},    {48, 16, 64, 32},     {48, 0, 64, 16}
  };

  npc->rect = &rc[npc->anim + (npc->count1 * 2)];
}

// Bute with sword (flying)
void npc_act_353(npc_t *npc) {
  static const rect_t rc[4] = {
    {168, 160, 184, 184},
    {184, 160, 200, 184},
    {168, 184, 184, 208},
    {184, 184, 200, 208},
  };

  static const rect_t rc_left[2] = {
    {200, 160, 216, 176},
    {216, 160, 232, 176},
  };

  static const rect_t rc_right[2] = {
    {200, 176, 216, 192},
    {216, 176, 232, 192},
  };

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
      ++npc->act_wait;

      if (npc->act_wait == 8) npc->bits &= ~NPC_IGNORE_SOLIDITY;

      npc->x += npc->xvel;
      npc->y += npc->yvel;

      if (npc->act_wait == 0x10) npc->act = 10;

      if (++npc->anim_wait > 2) {
        npc->anim_wait = 0;

        if (++npc->anim > 3) npc->anim = 0;
      }

      npc->rect = &rc[npc->anim];
      break;

    case 10:
      npc->act = 11;
      npc->anim = 0;
      npc->bits |= NPC_SHOOTABLE;
      npc->bits &= ~NPC_IGNORE_SOLIDITY;
      npc->damage = 5;
      npc->view.top = 8 * 0x200;
      // Fallthrough
    case 11:
      if (npc->x > player.x)
        npc->dir = 0;
      else
        npc->dir = 2;

      if (player.y - (24 * 0x200) > npc->y) {
        if (npc->dir == 0)
          npc->xvel2 += 0x10;
        else
          npc->xvel2 -= 0x10;
      } else {
        if (npc->dir == 0)
          npc->xvel2 -= 0x10;
        else
          npc->xvel2 += 0x10;
      }

      if (npc->y > player.y)
        npc->yvel2 -= 0x10;
      else
        npc->yvel2 += 0x10;

      if (npc->xvel2 < 0 && npc->flags & 1) npc->xvel2 *= -1;
      if (npc->xvel2 > 0 && npc->flags & 4) npc->xvel2 *= -1;

      if (npc->yvel2 < 0 && npc->flags & 2) npc->yvel2 *= -1;
      if (npc->yvel2 > 0 && npc->flags & 8) npc->yvel2 *= -1;

      if (npc->xvel2 < -0x5FF) npc->xvel2 = -0x5FF;
      if (npc->xvel2 > 0x5FF) npc->xvel2 = 0x5FF;

      if (npc->yvel2 < -0x5FF) npc->yvel2 = -0x5FF;
      if (npc->yvel2 > 0x5FF) npc->yvel2 = 0x5FF;

      npc->x += npc->xvel2;
      npc->y += npc->yvel2;

      if (++npc->anim_wait > 1) {
        npc->anim_wait = 0;

        if (++npc->anim > 1) npc->anim = 0;
      }

      if (npc->dir == 0)
        npc->rect = &rc_left[npc->anim];
      else
        npc->rect = &rc_right[npc->anim];

      break;
  }
}

// Invisible deathtrap wall
void npc_act_354(npc_t *npc) {
  int i, x, y;

  switch (npc->act) {
    case 0:
      npc->hit.bottom = 280 * 0x200;
      break;

    case 10:
      npc->act = 11;
      npc->act_wait = 0;

      if (npc->dir == 0)
        npc->x += 16 * 0x200;
      else
        npc->x -= 16 * 0x200;
      // Fallthrough
    case 11:
      if (++npc->act_wait > 100) {
        npc->act_wait = 0;
        // SetQuake(20);
        snd_play_sound(-1, 26, SOUND_MODE_PLAY);
        snd_play_sound(-1, 12, SOUND_MODE_PLAY);

        if (npc->dir == 0)
          npc->x -= 16 * 0x200;
        else
          npc->x += 16 * 0x200;

        for (i = 0; i < 20; ++i) {
          x = (npc->x / 0x200 / 0x10);
          y = (npc->y / 0x200 / 0x10) + i;
          stage_set_tile(x, y, 109);
        }
      }

      break;
  }
}

// Quote and Curly on Balrog's back
void npc_act_355(npc_t *npc) {
  static const rect_t rc[4] = {
    {80, 16, 96, 32},
    {80, 96, 96, 112},
    {128, 16, 144, 32},
    {208, 96, 224, 112},
  };

  switch (npc->act) {
    case 0:
      switch (npc->dir) {
        case 0:
          npc->surf = SURFACE_ID_MY_CHAR;
          npc->anim = 0;
          npc->x = npc->parent->x - (14 * 0x200);
          npc->y = npc->parent->y + (10 * 0x200);
          break;

        case 1:
          npc->surf = SURFACE_ID_NPC_REGU;
          npc->anim = 1;
          npc->x = npc->parent->x + (14 * 0x200);
          npc->y = npc->parent->y + (10 * 0x200);
          break;

        case 2:
          npc->surf = SURFACE_ID_MY_CHAR;
          npc->anim = 2;
          npc->x = npc->parent->x - (7 * 0x200);
          npc->y = npc->parent->y - (19 * 0x200);
          break;

        case 3:
          npc->surf = SURFACE_ID_NPC_REGU;
          npc->anim = 3;
          npc->x = npc->parent->x + (4 * 0x200);
          npc->y = npc->parent->y - (19 * 0x200);
          break;
      }

      break;
  }

  npc->rect = &rc[npc->anim];
}

// Balrog rescue
void npc_act_356(npc_t *npc) {
  static const rect_t rc_right[2] = {
    {240, 128, 280, 152},
    {240, 152, 280, 176},
  };

  switch (npc->act) {
    case 0:
      npc->act = 11;
      npc->anim_wait = 0;
      npc->tgt_y = npc->y - (16 * 0x200);
      npc->tgt_x = npc->x - (6 * 0x200);
      npc->yvel = 0;
      npc_spawn(355, 0, 0, 0, 0, 3, npc, 0xAA);
      npc_spawn(355, 0, 0, 0, 0, 2, npc, 0xAA);
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

      break;

    case 20:
      npc->act = 21;
      npc->xvel = -0x400;
      npc->yvel = 0x200;
      // Fallthrough
    case 21:
      ++npc->anim_wait;
      npc->xvel += 0x10;
      npc->yvel -= 8;
      npc->x += npc->xvel;
      npc->y += npc->yvel;

      if (npc->x > 60 * 0x10 * 0x200) npc->act = 22;

      break;

    case 22:
      npc->xvel = 0;
      npc->yvel = 0;
      break;
  }

  if (++npc->anim_wait > 4) {
    npc->anim_wait = 0;
    ++npc->anim;
  }

  if (npc->anim > 1) npc->anim = 0;

  npc->rect = &rc_right[npc->anim];
}

// Puppy ghost
void npc_act_357(npc_t *npc) {
  static const rect_t rc = {224, 136, 240, 152};

  switch (npc->act) {
    case 0:
      npc->rect = &rc;
      ++npc->act_wait;
      break;

    case 10:
      npc->act_wait = 0;
      npc->act = 11;
      snd_play_sound(-1, 29, SOUND_MODE_PLAY);
      // Fallthrough
    case 11:
      ++npc->act_wait;

      if (npc->act_wait / 2 % 2)
        npc->rect = NULL;
      else
        npc->rect = &rc;

      if (npc->act_wait > 50) npc->cond = 0;

      break;
  }

  if (npc->act_wait % 8 == 1) {
    // SetCaret(npc->x + (m_rand(-8, 8) * 0x200), npc->y + 0x1000, CARET_TINY_PARTICLES, DIR_UP);
  }
}

// Misery (stood in the wind during the credits)
void npc_act_358(npc_t *npc) {
  static const rect_t rc[5] = {
    {208, 8, 224, 32}, {224, 8, 240, 32}, {240, 8, 256, 32},
    {256, 8, 272, 32}, {272, 8, 288, 32},
  };

  switch (npc->act) {
    case 0:
      if (++npc->anim_wait > 6) {
        npc->anim_wait = 0;
        ++npc->anim;
      }

      if (npc->anim > 1) npc->anim = 0;

      break;

    case 10:
      if (++npc->anim_wait > 6) {
        npc->anim_wait = 0;
        ++npc->anim;
      }

      if (npc->anim > 4) npc->anim = 3;

      break;
  }

  npc->rect = &rc[npc->anim];
}

// Water droplet generator
void npc_act_359(npc_t *npc) {
  int x;

  if (player.x < npc->x + (((VID_WIDTH / 2) + 160) * 0x200) && player.x > npc->x - (((VID_WIDTH / 2) + 160) * 0x200) &&
      player.y < npc->y + (((VID_HEIGHT / 2) + 200) * 0x200) && player.y > npc->y - (((VID_HEIGHT / 2) + 40) * 0x200) &&
      m_rand(0, 100) == 2) {
    x = npc->x + (m_rand(-6, 6) * 0x200);
    npc_spawn(73, x, npc->y - (7 * 0x200), 0, 0, 0, 0, 0);
  }
}

// "Thank you" message at the end of the credits
void npc_act_360(npc_t *npc) {
  static const rect_t rc = {0, 176, 48, 184};

  if (npc->act == 0) {
    ++npc->act;
    npc->x -= 8 * 0x200;
    npc->y -= 8 * 0x200;
  }

  npc->rect = &rc;
}
