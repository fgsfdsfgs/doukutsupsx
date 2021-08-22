#include "game/boss_act/boss_act.h"

static void boss_act_monster_x_type1(npc_t *npc) {
  static const rect_t rc_up[6] = {
    {0, 0, 72, 32}, {0, 32, 72, 64}, {72, 0, 144, 32},
    {144, 0, 216, 32}, {72, 32, 144, 64}, {144, 32, 216, 64},
  };

  static const rect_t rc_down[6] = {
    {0, 64, 72, 96}, {0, 96, 72, 128}, {72, 64, 144, 96},
    {144, 64, 216, 96}, {72, 96, 144, 128}, {144, 96, 216, 128},
  };

  switch (npc->act) {
    case 10:
      npc->anim = 0;
      npc->bits &= ~NPC_BOUNCY;
      break;

    case 100:
      npc->bits |= NPC_BOUNCY;
      npc->act = 101;
      npc->act_wait = 0;
      npc->anim = 2;
      npc->anim_wait = 0;
      // Fallthrough
    case 101:
      if (++npc->act_wait > 30) npc->act = 102;

      if (++npc->anim_wait > 0) {
        npc->anim_wait = 0;
        ++npc->anim;
      }

      if (npc->anim > 3) npc->anim = 2;

      npc->xvel -= 0x20;
      break;

    case 102:
      npc->bits &= ~NPC_BOUNCY;
      npc->act = 103;
      npc->anim = 0;
      npc->anim_wait = 0;
      // Fallthrough
    case 103:
      ++npc->act_wait;

      if (++npc->anim_wait > 1) {
        npc->anim_wait = 0;
        ++npc->anim;
      }

      if (npc->anim > 1) npc->anim = 0;

      npc->xvel -= 0x20;
      break;

    case 200:
      npc->bits |= NPC_BOUNCY;
      npc->bits |= NPC_REAR_AND_TOP_DONT_HURT;
      npc->act = 201;
      npc->act_wait = 0;
      npc->anim = 4;
      npc->anim_wait = 0;
      // Fallthrough
    case 201:
      if (++npc->act_wait > 30) npc->act = 202;

      if (++npc->anim_wait > 0) {
        npc->anim_wait = 0;
        ++npc->anim;
      }

      if (npc->anim > 5) npc->anim = 4;

      npc->xvel += 0x20;
      break;

    case 202:
      npc->bits &= ~NPC_BOUNCY;
      npc->act = 203;
      npc->anim = 0;
      npc->anim_wait = 0;
      // Fallthrough
    case 203:
      ++npc->act_wait;

      if (++npc->anim_wait > 1) {
        npc->anim_wait = 0;
        ++npc->anim;
      }

      if (npc->anim > 1) npc->anim = 0;

      npc->xvel += 0x20;
      break;

    case 300:
      npc->act = 301;
      npc->anim = 4;
      npc->anim_wait = 0;
      npc->bits |= NPC_BOUNCY;
      // Fallthrough
    case 301:
      if (++npc->anim_wait > 0) {
        npc->anim_wait = 0;
        ++npc->anim;
      }

      if (npc->anim > 5) npc->anim = 4;

      npc->xvel += 0x20;

      if (npc->xvel > 0) {
        npc->xvel = 0;
        npc->act = 10;
      }

      break;

    case 400:
      npc->act = 401;
      npc->anim = 2;
      npc->anim_wait = 0;
      npc->bits |= NPC_BOUNCY;
      // Fallthrough
    case 401:
      if (++npc->anim_wait > 0) {
        npc->anim_wait = 0;
        ++npc->anim;
      }

      if (npc->anim > 3) npc->anim = 2;

      npc->xvel -= 0x20;

      if (npc->xvel < 0) {
        npc->xvel = 0;
        npc->act = 10;
      }

      break;
  }

  if ((npc->act == 101 || npc->act == 201 || npc->act == 301 || npc->act == 401) && npc->act_wait % 2 == 1)
    snd_play_sound(PRIO_NORMAL, 112, FALSE);

  if ((npc->act == 103 || npc->act == 203) && npc->act_wait % 4 == 1)
    snd_play_sound(PRIO_NORMAL, 111, FALSE);

  if (npc->act >= 100 && player.y < npc->y + (4 * 0x200) && player.y > npc->y - (4 * 0x200)) {
    npc->damage = 10;
    npc->bits |= NPC_REAR_AND_TOP_DONT_HURT;
  } else {
    npc->damage = 0;
    npc->bits &= ~NPC_REAR_AND_TOP_DONT_HURT;
  }

  if (npc->xvel > 0x400) npc->xvel = 0x400;
  if (npc->xvel < -0x400) npc->xvel = -0x400;

  npc->x += npc->xvel;

  if (npc->dir == 1)
    npc->rect = &rc_up[npc->anim];
  else
    npc->rect = &rc_down[npc->anim];
}

static void boss_act_monster_x_type2(npc_t *npc) {
  static const rect_t rect[4] = {
    {0, 128, 72, 160},
    {72, 128, 144, 160},
    {0, 160, 72, 192},
    {72, 160, 144, 192},
  };

  int dir;
  int x, y;

  switch (npc->act) {
    case 10:
      npc->act = 11;
      npc->act_wait = (npc->anim * 30) + 30;
      // Fallthrough
    case 11:
      if (npc->act_wait != 0) {
        --npc->act_wait;
        break;
      }

      switch (npc->anim) {
        case 0:
          dir = 3;
          x = -30 * 0x200;
          y = 6 * 0x200;
          break;
        case 1:
          dir = 2;
          x = 30 * 0x200;
          y = 6 * 0x200;
          break;
        case 2:
          dir = 0;
          x = -30 * 0x200;
          y = -6 * 0x200;
          break;
        default:
          dir = 1;
          x = 30 * 0x200;
          y = -6 * 0x200;
          break;
      }

      npc_spawn(158, npc->x + x, npc->y + y, 0, 0, dir, NULL, 0x100);
      snd_play_sound(PRIO_NORMAL, 39, FALSE);
      npc->act_wait = 120;

      break;
  }

  npc->x = (npc_boss[0].x + npc_boss[npc->count1].x) / 2;
  npc->y = (npc_boss[0].y + npc_boss[npc->count1].y) / 2;

  npc->rect = &rect[npc->anim];
}

static void boss_act_monster_x_type3(npc_t *npc) {
  switch (npc->act) {
    case 10:
      npc->tgt_x += 1 * 0x200;

      if (npc->tgt_x > 32 * 0x200) {
        npc->tgt_x = 32 * 0x200;
        npc->act = 0;
        npc_boss[3].act = 10;
        npc_boss[4].act = 10;
        npc_boss[5].act = 10;
        npc_boss[6].act = 10;
      }

      break;

    case 20:
      npc->tgt_x -= 1 * 0x200;

      if (npc->tgt_x < 0) {
        npc->tgt_x = 0;
        npc->act = 0;
        npc_boss[3].act = 0;
        npc_boss[4].act = 0;
        npc_boss[5].act = 0;
        npc_boss[6].act = 0;
      }

      break;

    case 30:
      npc->tgt_x += 1 * 0x200;

      if (npc->tgt_x > 20 * 0x200) {
        npc->tgt_x = 20 * 0x200;
        npc->act = 0;
        npc_boss[7].act = 10;
        npc_boss[13].act = 10;
        npc_boss[14].act = 10;
        npc_boss[15].act = 10;
        npc_boss[16].act = 10;
      }

      break;

    case 40:
      npc->tgt_x -= 1 * 0x200;

      if (npc->tgt_x < 0) {
        npc->tgt_x = 0;
        npc->act = 0;
        npc_boss[7].act = 0;
        npc_boss[13].act = 0;
        npc_boss[14].act = 0;
        npc_boss[15].act = 0;
        npc_boss[16].act = 0;
      }

      break;
  }

  static const rect_t rc_left = {216, 96, 264, 144};
  static const rect_t rc_right = {264, 96, 312, 144};

  if (npc->dir == 0) {
    npc->rect = &rc_left;
    npc->x = npc_boss[0].x - (24 * 0x200) - npc->tgt_x;
    npc->y = npc_boss[0].y;
  } else {
    npc->rect = &rc_right;
    npc->x = npc_boss[0].x + (24 * 0x200) + npc->tgt_x;
    npc->y = npc_boss[0].y;
  }
}

static void boss_act_monster_x_type4(npc_t *npc) {
  int xvel, yvel;
  u8 deg;

  static const rect_t rect[8] = {
    {0, 192, 16, 208}, {16, 192, 32, 208}, {32, 192, 48, 208}, {48, 192, 64, 208},
    {0, 208, 16, 224}, {16, 208, 32, 224}, {32, 208, 48, 224}, {48, 208, 64, 224},
  };

  switch (npc->act) {
    case 0:
      npc->bits &= ~NPC_SHOOTABLE;
      npc->anim = 0;
      break;

    case 10:
      npc->act = 11;
      npc->act_wait = (npc->tgt_x * 10) + 40;
      npc->bits |= NPC_SHOOTABLE;
      // Fallthrough
    case 11:
      if (npc->act_wait < 16 && npc->act_wait / 2 % 2)
        npc->anim = 1;
      else
        npc->anim = 0;

      if (npc->act_wait != 0) {
        --npc->act_wait;
        break;
      }

      deg = m_atan2(npc->x - player.x, npc->y - player.y);
      deg += (u8)m_rand(-2, 2);
      yvel = m_sin(deg) * 3;
      xvel = m_cos(deg) * 3;
      npc_spawn(156, npc->x, npc->y, xvel, yvel, 0, NULL, 0x100);

      snd_play_sound(PRIO_NORMAL, 39, FALSE);
      npc->act_wait = 40;

      break;
  }

  switch (npc->tgt_x) {
    case 0:
      npc->x = npc_boss[0].x - (22 * 0x200);
      npc->y = npc_boss[0].y - (16 * 0x200);
      break;
    case 1:
      npc->x = npc_boss[0].x + (28 * 0x200);
      npc->y = npc_boss[0].y - (16 * 0x200);
      break;
    case 2:
      npc->x = npc_boss[0].x - (15 * 0x200);
      npc->y = npc_boss[0].y + (14 * 0x200);
      break;
    case 3:
      npc->x = npc_boss[0].x + (17 * 0x200);
      npc->y = npc_boss[0].y + (14 * 0x200);
      break;
  }

  npc->rect = &rect[npc->tgt_x + 4 * npc->anim];
}

static void boss_act_monster_x_face(npc_t *npc) {
  static const rect_t rect[3] = {
      {216, 0, 320, 48},
      {216, 48, 320, 96},
      {216, 144, 320, 192},
  };

  static u8 flash;

  switch (npc->act) {
    case 0:
      npc_boss[0].bits &= ~NPC_SHOOTABLE;
      npc->anim = 0;
      break;

    case 10:
      npc->act = 11;
      npc->act_wait = (npc->tgt_x * 10) + 40;
      npc_boss[0].bits |= NPC_SHOOTABLE;
      // Fallthrough
    case 11:
      if (npc_boss[0].shock) {
        if (flash++ / 2 % 2)
          npc->anim = 1;
        else
          npc->anim = 0;
      } else {
        npc->anim = 0;
      }

      break;
  }

  npc_boss[7].x = npc_boss[0].x;
  npc_boss[7].y = npc_boss[0].y;

  if (npc_boss[0].act <= 10) npc->anim = 2;

  npc->rect = &rect[npc->anim];
}

void boss_act_monster_x(npc_t *root) {
  int i;
  npc_t *npc = npc_boss;

  switch (npc->act) {
    case 0:
      npc->life = 1;
      npc->x = -320 * 0x200;
      break;

    case 1:
      npc->life = 700;
      npc->exp = 1;
      npc->act = 1;
      npc->x = 2048 * 0x200;
      npc->y = 200 * 0x200;
      npc->snd_hit = 54;
      npc->hit.front = 24 * 0x200;
      npc->hit.top = 24 * 0x200;
      npc->hit.back = 24 * 0x200;
      npc->hit.bottom = 24 * 0x200;
      npc->bits = (NPC_IGNORE_SOLIDITY | NPC_EVENT_WHEN_KILLED | NPC_SHOW_DAMAGE);
      npc->size = 3;
      npc->event_num = 1000;
      npc->anim = 0;

      npc_boss[1].cond = 0x80;
      npc_boss[1].size = 3;
      npc_boss[1].dir = 0;
      npc_boss[1].view.front = 24 * 0x200;
      npc_boss[1].view.top = 24 * 0x200;
      npc_boss[1].view.back = 24 * 0x200;
      npc_boss[1].view.bottom = 24 * 0x200;
      npc_boss[1].bits = NPC_IGNORE_SOLIDITY;

      npc_boss[2] = npc_boss[1];
      npc_boss[2].dir = 2;

      npc_boss[3].cond = 0x80;
      npc_boss[3].life = 60;
      npc_boss[3].snd_hit = 54;
      npc_boss[3].snd_die = 71;
      npc_boss[3].size = 2;
      npc_boss[3].view.front = 8 * 0x200;
      npc_boss[3].view.top = 8 * 0x200;
      npc_boss[3].view.back = 8 * 0x200;
      npc_boss[3].view.bottom = 8 * 0x200;
      npc_boss[3].hit.front = 5 * 0x200;
      npc_boss[3].hit.back = 5 * 0x200;
      npc_boss[3].hit.top = 5 * 0x200;
      npc_boss[3].hit.bottom = 5 * 0x200;
      npc_boss[3].bits = NPC_IGNORE_SOLIDITY;
      npc_boss[3].tgt_x = 0;

      npc_boss[4] = npc_boss[3];
      npc_boss[4].tgt_x = 1;

      npc_boss[5] = npc_boss[3];
      npc_boss[5].tgt_x = 2;
      npc_boss[5].life = 100;

      npc_boss[6] = npc_boss[3];
      npc_boss[6].tgt_x = 3;
      npc_boss[6].life = 100;

      npc_boss[7].cond = 0x80;
      npc_boss[7].x = 2048 * 0x200;
      npc_boss[7].y = 200 * 0x200;
      npc_boss[7].view.front = 52 * 0x200;
      npc_boss[7].view.top = 24 * 0x200;
      npc_boss[7].view.back = 52 * 0x200;
      npc_boss[7].view.bottom = 24 * 0x200;
      npc_boss[7].snd_hit = 52;
      npc_boss[7].hit.front = 8 * 0x200;
      npc_boss[7].hit.top = 24 * 0x200;
      npc_boss[7].hit.back = 8 * 0x200;
      npc_boss[7].hit.bottom = 16 * 0x200;
      npc_boss[7].bits = NPC_IGNORE_SOLIDITY;
      npc_boss[7].size = 3;
      npc_boss[7].anim = 0;

      npc_boss[9].cond = 0x80;
      npc_boss[9].act = 0;
      npc_boss[9].dir = 1;
      npc_boss[9].x = 1984 * 0x200;
      npc_boss[9].y = 144 * 0x200;
      npc_boss[9].view.front = 36 * 0x200;
      npc_boss[9].view.top = 8 * 0x200;
      npc_boss[9].view.back = 36 * 0x200;
      npc_boss[9].view.bottom = 24 * 0x200;
      npc_boss[9].snd_hit = 52;
      npc_boss[9].hit.front = 28 * 0x200;
      npc_boss[9].hit.top = 8 * 0x200;
      npc_boss[9].hit.back = 28 * 0x200;
      npc_boss[9].hit.bottom = 16 * 0x200;
      npc_boss[9].bits = (NPC_SOLID_SOFT | NPC_INVULNERABLE | NPC_IGNORE_SOLIDITY | NPC_REAR_AND_TOP_DONT_HURT);
      npc_boss[9].size = 3;

      npc_boss[10] = npc_boss[9];
      npc_boss[10].x = 2112 * 0x200;

      npc_boss[11] = npc_boss[9];
      npc_boss[11].dir = 3;
      npc_boss[11].x = 1984 * 0x200;
      npc_boss[11].y = 256 * 0x200;
      npc_boss[11].view.top = 24 * 0x200;
      npc_boss[11].view.bottom = 8 * 0x200;
      npc_boss[11].hit.top = 16 * 0x200;
      npc_boss[11].hit.bottom = 8 * 0x200;

      npc_boss[12] = npc_boss[11];
      npc_boss[12].x = 2112 * 0x200;

      npc_boss[13] = npc_boss[9];
      npc_boss[13].cond = 0x80;
      npc_boss[13].view.top = 16 * 0x200;
      npc_boss[13].view.bottom = 16 * 0x200;
      npc_boss[13].view.front = 30 * 0x200;
      npc_boss[13].view.back = 42 * 0x200;
      npc_boss[13].count1 = 9;
      npc_boss[13].anim = 0;
      npc_boss[13].bits = NPC_IGNORE_SOLIDITY;

      npc_boss[14] = npc_boss[13];
      npc_boss[14].view.front = 42 * 0x200;
      npc_boss[14].view.back = 30 * 0x200;
      npc_boss[14].count1 = 10;
      npc_boss[14].anim = 1;
      npc_boss[14].bits = NPC_IGNORE_SOLIDITY;

      npc_boss[15] = npc_boss[13];
      npc_boss[15].view.top = 16 * 0x200;
      npc_boss[15].view.bottom = 16 * 0x200;
      npc_boss[15].count1 = 11;
      npc_boss[15].anim = 2;
      npc_boss[15].bits = NPC_IGNORE_SOLIDITY;

      npc_boss[16] = npc_boss[15];
      npc_boss[16].view.front = 42 * 0x200;
      npc_boss[16].view.back = 30 * 0x200;
      npc_boss[16].count1 = 12;
      npc_boss[16].anim = 3;
      npc_boss[16].bits = NPC_IGNORE_SOLIDITY;

      npc_boss_max = 16;
      for (int i = 0; i <= npc_boss_max; ++i)
        npc_boss[i].surf = SURFACE_ID_LEVEL_SPRITESET_2;

      npc->act = 2;
      break;

    case 10:
      npc->act = 11;
      npc->act_wait = 0;
      npc->count1 = 0;
      // Fallthrough
    case 11:
      ++npc->act_wait;

      if (npc->act_wait > 100) {
        npc->act_wait = 0;

        if (npc->x > player.x)
          npc->act = 100;
        else
          npc->act = 200;
      }

      break;

    case 100:
      npc->act_wait = 0;
      npc->act = 101;
      ++npc->count1;
      // Fallthrough
    case 101:
      ++npc->act_wait;

      if (npc->act_wait == 4) npc_boss[9].act = 100;
      if (npc->act_wait == 8) npc_boss[10].act = 100;
      if (npc->act_wait == 10) npc_boss[11].act = 100;
      if (npc->act_wait == 12) npc_boss[12].act = 100;

      if (npc->act_wait > 120 && npc->count1 > 2) npc->act = 300;
      if (npc->act_wait > 121 && player.x > npc->x) npc->act = 200;

      break;

    case 200:
      npc->act_wait = 0;
      npc->act = 201;
      ++npc->count1;
      // Fallthrough
    case 201:
      ++npc->act_wait;

      if (npc->act_wait == 4) npc_boss[9].act = 200;
      if (npc->act_wait == 8) npc_boss[10].act = 200;
      if (npc->act_wait == 10) npc_boss[11].act = 200;
      if (npc->act_wait == 12) npc_boss[12].act = 200;

      if (npc->act_wait > 120 && npc->count1 > 2) npc->act = 400;
      if (npc->act_wait > 121 && player.x < npc->x) npc->act = 100;

      break;

    case 300:
      npc->act_wait = 0;
      npc->act = 301;
      // Fallthrough
    case 301:
      ++npc->act_wait;

      if (npc->act_wait == 4) npc_boss[9].act = 300;
      if (npc->act_wait == 8) npc_boss[10].act = 300;
      if (npc->act_wait == 10) npc_boss[11].act = 300;
      if (npc->act_wait == 12) npc_boss[12].act = 300;

      if (npc->act_wait > 50) {
        if (npc_boss[3].cond == 0 && npc_boss[4].cond == 0 && npc_boss[5].cond == 0 && npc_boss[6].cond == 0)
          npc->act = 600;
        else
          npc->act = 500;
      }

      break;

    case 400:
      npc->act_wait = 0;
      npc->act = 401;
      // Fallthrough
    case 401:
      ++npc->act_wait;

      if (npc->act_wait == 4) npc_boss[9].act = 400;
      if (npc->act_wait == 8) npc_boss[10].act = 400;
      if (npc->act_wait == 10) npc_boss[11].act = 400;
      if (npc->act_wait == 12) npc_boss[12].act = 400;

      if (npc->act_wait > 50) {
        if (npc_boss[3].cond == 0 && npc_boss[4].cond == 0 && npc_boss[5].cond == 0 && npc_boss[6].cond == 0)
          npc->act = 600;
        else
          npc->act = 500;
      }

      break;

    case 500:
      npc->act = 501;
      npc->act_wait = 0;
      npc_boss[1].act = 10;
      npc_boss[2].act = 10;
      // Fallthrough
    case 501:
      ++npc->act_wait;

      if (npc->act_wait > 300) {
        npc->act = 502;
        npc->act_wait = 0;
      }

      if (npc_boss[3].cond == 0 && npc_boss[4].cond == 0 && npc_boss[5].cond == 0 && npc_boss[6].cond == 0) {
        npc->act = 502;
        npc->act_wait = 0;
      }

      break;

    case 502:
      npc->act = 503;
      npc->act_wait = 0;
      npc->count1 = 0;
      npc_boss[1].act = 20;
      npc_boss[2].act = 20;
      // Fallthrough
    case 503:
    case 603:
      ++npc->act_wait;

      if (npc->act_wait > 50) {
        if (npc->x > player.x)
          npc->act = 100;
        else
          npc->act = 200;
      }

      break;

    case 600:
      npc->act = 601;
      npc->act_wait = 0;
      npc->count2 = npc->life;
      npc_boss[1].act = 30;
      npc_boss[2].act = 30;
      // Fallthrough
    case 601:
      ++npc->act_wait;

      if (npc->life < npc->count2 - 200 || npc->act_wait > 300) {
        npc->act = 602;
        npc->act_wait = 0;
      }

      break;

    case 602:
      npc->act = 603;
      npc->act_wait = 0;
      npc->count1 = 0;
      npc_boss[1].act = 40;
      npc_boss[2].act = 40;
      // Fallthrough

    case 1000:
      cam_start_quake_small(2);

      if (++npc->act_wait % 8 == 0) snd_play_sound(PRIO_NORMAL, 52, FALSE);

      npc_spawn_death_fx(npc->x + (m_rand(-72, 72) * 0x200), npc->y + (m_rand(-64, 64) * 0x200), 1, 1, 0);

      if (npc->act_wait > 100) {
        npc->act_wait = 0;
        npc->act = 1001;
        cam_start_flash(npc->x, npc->y, FLASH_MODE_EXPLOSION);
        snd_play_sound(PRIO_NORMAL, 35, FALSE);
      }

      break;

    case 1001:
      cam_start_quake_small(40);

      ++npc->act_wait;

      if (npc->act_wait > 50) {
        for (i = 0; i < 20; ++i) npc_boss[i].cond = 0;
        npc_boss_max = -1;
        npc_delete_by_class(158, TRUE);
        npc_spawn(159, npc->x, npc->y - (24 * 0x200), 0, 0, 0, NULL, 0);
      }

      break;
  }

  boss_act_monster_x_type1(&npc_boss[9]);
  boss_act_monster_x_type1(&npc_boss[10]);
  boss_act_monster_x_type1(&npc_boss[11]);
  boss_act_monster_x_type1(&npc_boss[12]);

  npc->x += (((npc_boss[11].x + npc_boss[10].x + npc_boss[9].x + npc_boss[12].x) / 4) - npc->x) / 0x10;

  boss_act_monster_x_face(&npc_boss[7]);

  boss_act_monster_x_type2(&npc_boss[13]);
  boss_act_monster_x_type2(&npc_boss[14]);
  boss_act_monster_x_type2(&npc_boss[15]);
  boss_act_monster_x_type2(&npc_boss[16]);

  boss_act_monster_x_type3(&npc_boss[1]);
  boss_act_monster_x_type3(&npc_boss[2]);

  if (npc_boss[3].cond) boss_act_monster_x_type4(&npc_boss[3]);
  if (npc_boss[4].cond) boss_act_monster_x_type4(&npc_boss[4]);
  if (npc_boss[5].cond) boss_act_monster_x_type4(&npc_boss[5]);
  if (npc_boss[6].cond) boss_act_monster_x_type4(&npc_boss[6]);

  if (npc->life == 0 && npc->act < 1000) {
    npc->act = 1000;
    npc->act_wait = 0;
    npc->shock = 150;
    npc_boss[9].act = 300;
    npc_boss[10].act = 300;
    npc_boss[11].act = 300;
    npc_boss[12].act = 300;
  }
}
