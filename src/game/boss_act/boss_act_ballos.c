#include "game/boss_act/boss_act.h"

static void boss_act_ballos_eye(npc_t *npc) {
  static const rect_t rc_left[5] = {
    {272, 0, 296, 16}, {272, 16, 296, 32}, {272, 32, 296, 48},
    {0, 0, 0, 0}, {240, 16, 264, 32},
  };

  static const rect_t rc_right[5] = {
    {296, 0, 320, 16}, {296, 16, 320, 32}, {296, 32, 320, 48},
    {0, 0, 0, 0}, {240, 32, 264, 48},
  };

  switch (npc->act) {
    case 100:
      npc->act = 101;
      npc->anim = 0;
      npc->anim_wait = 0;
      // Fallthrough
    case 101:
      ++npc->anim_wait;

      if (npc->anim_wait > 2) {
        npc->anim_wait = 0;
        ++npc->anim;
      }

      if (npc->anim > 2) npc->act = 102;

      break;

    case 102:
      npc->anim = 3;
      break;

    case 200:
      npc->act = 201;
      npc->anim = 3;
      npc->anim_wait = 0;
      // Fallthrough
    case 201:
      ++npc->anim_wait;

      if (npc->anim_wait > 2) {
        npc->anim_wait = 0;
        --npc->anim;
      }

      if (npc->anim <= 0) npc->act = 202;

      break;

    case 300:
      npc->act = 301;
      npc->anim = 4;

      if (npc->dir == 0)
        npc_spawn_death_fx(npc->x - (4 * 0x200), npc->y, 0x800, 10, 0);
      else
        npc_spawn_death_fx(npc->x + (4 * 0x200), npc->y, 0x800, 10, 0);

      break;
  }

  if (npc->dir == 0)
    npc->x = npc_boss[0].x - (24 * 0x200);
  else
    npc->x = npc_boss[0].x + (24 * 0x200);

  npc->y = npc_boss[0].y - (36 * 0x200);

  if (npc->act >= 0 && npc->act < 300) {
    if (npc->anim != 3)
      npc->bits &= ~NPC_SHOOTABLE;
    else
      npc->bits |= NPC_SHOOTABLE;
  }

  if (npc->dir == 0)
    npc->rect = &rc_left[npc->anim];
  else
    npc->rect = &rc_right[npc->anim];
}

static void boss_act_ballos_body(npc_t *npc) {
  static const rect_t rc[4] = {
    {0, 0, 120, 120},
    {120, 0, 240, 120},
    {0, 120, 120, 240},
    {120, 120, 240, 240},
  };

  npc->x = npc_boss[0].x;
  npc->y = npc_boss[0].y;

  npc->rect = &rc[npc->anim];
}

static void boss_act_ballos_hit_top(npc_t *npc) {
  npc->x = npc_boss[0].x;
  npc->y = npc_boss[0].y - (44 * 0x200);
}

static void boss_act_ballos_hit_mid(npc_t *npc) {
  npc->x = npc_boss[0].x;
  npc->y = npc_boss[0].y;
}

void boss_act_ballos(npc_t *root) {
  npc_t *npc = npc_boss;

  static u8 flash;
  int i;
  int x, y;

  switch (npc->act) {
    case 0:
      // Initialize main boss
      npc->act = 1;
      npc->cond = 0x80;
      npc->exp = 1;
      npc->dir = 0;
      npc->x = 320 * 0x200;
      npc->y = -64 * 0x200;
      npc->snd_hit = 54;
      npc->hit.front = 32 * 0x200;
      npc->hit.top = 48 * 0x200;
      npc->hit.back = 32 * 0x200;
      npc->hit.bottom = 48 * 0x200;
      npc->bits = (NPC_IGNORE_SOLIDITY | NPC_SOLID_HARD | NPC_EVENT_WHEN_KILLED | NPC_SHOW_DAMAGE);
      npc->size = 3;
      npc->damage = 0;
      npc->event_num = 1000;
      npc->life = 800;

      // Initialize eyes
      npc_boss[1].cond = 0x90;
      npc_boss[1].dir = 0;
      npc_boss[1].bits = NPC_IGNORE_SOLIDITY;
      npc_boss[1].life = 10000;
      npc_boss[1].view.front = 12 * 0x200;
      npc_boss[1].view.top = 0;
      npc_boss[1].view.back = 12 * 0x200;
      npc_boss[1].view.bottom = 16 * 0x200;
      npc_boss[1].hit.front = 12 * 0x200;
      npc_boss[1].hit.top = 0;
      npc_boss[1].hit.back = 12 * 0x200;
      npc_boss[1].hit.bottom = 16 * 0x200;

      npc_boss[2] = npc_boss[1];
      npc_boss[2].dir = 2;

      // Initialize the body
      npc_boss[3].cond = 0x90;
      npc_boss[3].bits = (NPC_SOLID_SOFT | NPC_INVULNERABLE | NPC_IGNORE_SOLIDITY);
      npc_boss[3].view.front = 60 * 0x200;
      npc_boss[3].view.top = 60 * 0x200;
      npc_boss[3].view.back = 60 * 0x200;
      npc_boss[3].view.bottom = 60 * 0x200;
      npc_boss[3].hit.front = 48 * 0x200;
      npc_boss[3].hit.top = 24 * 0x200;
      npc_boss[3].hit.back = 48 * 0x200;
      npc_boss[3].hit.bottom = 32 * 0x200;

      npc_boss[4].cond = 0x90;
      npc_boss[4].bits = (NPC_SOLID_SOFT | NPC_INVULNERABLE | NPC_IGNORE_SOLIDITY);
      npc_boss[4].hit.front = 32 * 0x200;
      npc_boss[4].hit.top = 8 * 0x200;
      npc_boss[4].hit.back = 32 * 0x200;
      npc_boss[4].hit.bottom = 8 * 0x200;

      npc_boss[5].cond = 0x90;
      npc_boss[5].bits = (NPC_INVULNERABLE | NPC_IGNORE_SOLIDITY | NPC_SOLID_HARD);
      npc_boss[5].hit.front = 32 * 0x200;
      npc_boss[5].hit.top = 0;
      npc_boss[5].hit.back = 32 * 0x200;
      npc_boss[5].hit.bottom = 48 * 0x200;

      npc_boss_max = 5;
      for (int i = 0; i <= npc_boss_max; ++i) npc_boss[i].surf = SURFACE_ID_LEVEL_SPRITESET_2;

      break;

    case 100:
      npc->act = 101;
      npc->anim = 0;
      npc->x = player.x;
      npc_spawn(333, player.x, 304 * 0x200, 0, 0, 2, NULL, 0x100);
      npc->act_wait = 0;
      // Fallthrough
    case 101:
      ++npc->act_wait;

      if (npc->act_wait > 30) npc->act = 102;

      break;

    case 102:
      npc->yvel += 0x40;
      if (npc->yvel > 0xC00) npc->yvel = 0xC00;

      npc->y += npc->yvel;

      if (npc->y > (304 * 0x200) - npc->hit.bottom) {
        npc->y = (304 * 0x200) - npc->hit.bottom;
        npc->yvel = 0;
        npc->act = 103;
        npc->act_wait = 0;
        cam_start_quake_big(30);
        snd_play_sound(PRIO_NORMAL, 44, FALSE);

        if (player.y > npc->y + (48 * 0x200) && player.x < npc->x + (24 * 0x200) && player.x > npc->x - (24 * 0x200))
          plr_damage(16);

        for (i = 0; i < 0x10; ++i) {
          x = npc->x + (m_rand(-40, 40) * 0x200);
          npc_spawn(4, x, npc->y + (40 * 0x200), 0, 0, 0, NULL, 0x100);
        }

        if (player.flags & 8) player.yvel = -0x200;
      }

      break;

    case 103:
      ++npc->act_wait;

      if (npc->act_wait == 50) {
        npc->act = 104;
        npc_boss[1].act = 100;
        npc_boss[2].act = 100;
      }

      break;

    case 200:
      npc->act = 201;
      npc->count1 = 0;
      // Fallthrough
    case 201:
      npc->act = 203;
      npc->xvel = 0;
      ++npc->count1;
      npc->hit.bottom = 48 * 0x200;
      npc->damage = 0;

      if (npc->count1 % 3 == 0)
        npc->act_wait = 150;
      else
        npc->act_wait = 50;
      // Fallthrough
    case 203:
      --npc->act_wait;

      if (npc->act_wait <= 0) {
        npc->act = 204;
        npc->yvel = -0xC00;

        if (npc->x < player.x)
          npc->xvel = 0x200;
        else
          npc->xvel = -0x200;
      }

      break;

    case 204:
      if (npc->x < 80 * 0x200) npc->xvel = 0x200;
      if (npc->x > 544 * 0x200) npc->xvel = -0x200;

      npc->yvel += 0x55;
      if (npc->yvel > 0xC00) npc->yvel = 0xC00;

      npc->x += npc->xvel;
      npc->y += npc->yvel;

      if (npc->y > (304 * 0x200) - npc->hit.bottom) {
        npc->y = (304 * 0x200) - npc->hit.bottom;
        npc->yvel = 0;
        npc->act = 201;
        npc->act_wait = 0;

        if (player.y > npc->y + (56 * 0x200)) plr_damage(16);

        if (player.flags & 8) player.yvel = -0x200;

        cam_start_quake_big(30);
        snd_play_sound(PRIO_NORMAL, 26, FALSE);
        npc_spawn(332, npc->x - (12 * 0x200), npc->y + (52 * 0x200), 0, 0, 0, NULL, 0x100);
        npc_spawn(332, npc->x + (12 * 0x200), npc->y + (52 * 0x200), 0, 0, 2, NULL, 0x100);
        snd_play_sound(PRIO_NORMAL, 44, FALSE);

        for (i = 0; i < 0x10; ++i) {
          x = npc->x + (m_rand(-40, 40) * 0x200);
          npc_spawn(4, x, npc->y + (40 * 0x200), 0, 0, 0, NULL, 0x100);
        }
      }

      break;

    case 220:
      npc->act = 221;
      npc->life = 1200;
      npc_boss[1].act = 200;
      npc_boss[2].act = 200;
      npc->xvel = 0;
      npc->anim = 0;
      npc->shock = 0;
      flash = 0;
      // Fallthrough
    case 221:
      npc->yvel += 0x40;
      if (npc->yvel > 0xC00) npc->yvel = 0xC00;

      npc->y += npc->yvel;

      if (npc->y > (304 * 0x200) - npc->hit.bottom) {
        npc->y = (304 * 0x200) - npc->hit.bottom;
        npc->yvel = 0;
        npc->act = 222;
        npc->act_wait = 0;
        cam_start_quake_big(30);
        snd_play_sound(PRIO_NORMAL, 26, FALSE);

        for (i = 0; i < 0x10; ++i) {
          x = npc->x + (m_rand(-40, 40) * 0x200);
          npc_spawn(4, x, npc->y + (40 * 0x200), 0, 0, 0, NULL, 0x100);
        }

        if (player.flags & 8) player.yvel = -0x200;
      }

      break;

    case 300:
      npc->act = 301;
      npc->act_wait = 0;

      for (i = 0; i < 0x100; i += 0x40) {
        npc_spawn(342, npc->x, npc->y, 0, 0, i, npc, 90);
        npc_spawn(342, npc->x, npc->y, 0, 0, i + 0x220, npc, 90);
      }

      npc_spawn(343, npc->x, npc->y, 0, 0, 0, npc, 0x18);
      npc_spawn(344, npc->x - (24 * 0x200), npc->y - (36 * 0x200), 0, 0, 0, npc, 0x20);
      npc_spawn(344, npc->x + (24 * 0x200), npc->y - (36 * 0x200), 0, 0, 2, npc, 0x20);
      // Fallthrough
    case 301:
      npc->y += ((225 * 0x200) - npc->y) / 8;

      ++npc->act_wait;

      if (npc->act_wait > 50) {
        npc->act = 310;
        npc->act_wait = 0;
      }

      break;

    case 311:
      npc->dir = 0;
      npc->xvel = -0x3AA;
      npc->yvel = 0;
      npc->x += npc->xvel;

      if (npc->x < 111 * 0x200) {
        npc->x = 111 * 0x200;
        npc->act = 312;
      }

      break;

    case 312:
      npc->dir = 1;
      npc->yvel = -0x3AA;
      npc->xvel = 0;
      npc->y += npc->yvel;

      if (npc->y < 111 * 0x200) {
        npc->y = 111 * 0x200;
        npc->act = 313;
      }

      break;

    case 313:
      npc->dir = 2;
      npc->xvel = 0x3AA;
      npc->yvel = 0;
      npc->x += npc->xvel;

      if (npc->x > 513 * 0x200) {
        npc->x = 513 * 0x200;
        npc->act = 314;
      }

      if (npc->count1 != 0) --npc->count1;

      if (npc->count1 == 0 && npc->x > 304 * 0x200 && npc->x < 336 * 0x200) npc->act = 400;

      break;

    case 314:
      npc->dir = 3;
      npc->yvel = 0x3AA;
      npc->xvel = 0;
      npc->y += npc->yvel;

      if (npc->y > 225 * 0x200) {
        npc->y = 225 * 0x200;
        npc->act = 311;
      }

      break;

    case 400:
      npc->act = 401;
      npc->act_wait = 0;
      npc->xvel = 0;
      npc->yvel = 0;
      npc_delete_by_class(339, FALSE);
      // Fallthrough
    case 401:
      npc->y += ((159 * 0x200) - npc->y) / 8;

      ++npc->act_wait;

      if (npc->act_wait > 50) {
        npc->act_wait = 0;
        npc->act = 410;

        for (i = 0; i < 0x100; i += 0x20)
          npc_spawn(346, npc->x, npc->y, 0, 0, i, npc, 0x50);

        npc_spawn(343, npc->x, npc->y, 0, 0, 0, npc, 0x18);
        npc_spawn(344, npc->x - (24 * 0x200), npc->y - (36 * 0x200), 0, 0, 0, npc, 0x20);
        npc_spawn(344, npc->x + (24 * 0x200), npc->y - (36 * 0x200), 0, 0, 2, npc, 0x20);
      }

      break;

    case 410:
      ++npc->act_wait;

      if (npc->act_wait > 50) {
        npc->act_wait = 0;
        npc->act = 411;
      }

      break;

    case 411:
      ++npc->act_wait;

      if (npc->act_wait % 30 == 1) {
        x = (((npc->act_wait / 30) * 2) + 2) * 0x10 * 0x200;
        npc_spawn(348, x, 336 * 0x200, 0, 0, 0, NULL, 0x180);
      }

      if (npc->act_wait / 3 % 2) snd_play_sound(PRIO_NORMAL, 26, FALSE);

      if (npc->act_wait > 540) npc->act = 420;

      break;

    case 420:
      npc->act = 421;
      npc->act_wait = 0;
      npc->anim_wait = 0;
      cam_start_quake_big(30);
      snd_play_sound(PRIO_NORMAL, 35, FALSE);
      npc_boss[1].act = 102;
      npc_boss[2].act = 102;

      for (i = 0; i < 0x100; ++i) {
        x = npc->x + (m_rand(-60, 60) * 0x200);
        y = npc->y + (m_rand(-60, 60) * 0x200);
        npc_spawn(4, x, y, 0, 0, 0, NULL, 0);
      }
      // Fallthrough
    case 421:
      ++npc->anim_wait;

      if (npc->anim_wait > 500) {
        npc->anim_wait = 0;
        npc->act = 422;
      }

      break;

    case 422:
      ++npc->anim_wait;

      if (npc->anim_wait > 200) {
        npc->anim_wait = 0;
        npc->act = 423;
      }

      break;

    case 423:
      ++npc->anim_wait;

      if (npc->anim_wait > 20) {
        npc->anim_wait = 0;
        npc->act = 424;
      }

      break;

    case 424:
      ++npc->anim_wait;

      if (npc->anim_wait > 200) {
        npc->anim_wait = 0;
        npc->act = 425;
      }

      break;

    case 425:
      ++npc->anim_wait;

      if (npc->anim_wait > 500) {
        npc->anim_wait = 0;
        npc->act = 426;
      }

      break;

    case 426:
      ++npc->anim_wait;

      if (npc->anim_wait > 200) {
        npc->anim_wait = 0;
        npc->act = 427;
      }

      break;

    case 427:
      ++npc->anim_wait;

      if (npc->anim_wait > 20) {
        npc->anim_wait = 0;
        npc->act = 428;
      }

      break;

    case 428:
      ++npc->anim_wait;

      if (npc->anim_wait > 200) {
        npc->anim_wait = 0;
        npc->act = 421;
      }

      break;

    case 1000:
      npc->act = 1001;
      npc->act_wait = 0;
      npc_boss[1].act = 300;
      npc_boss[2].act = 300;
      npc_boss[0].bits &= ~(NPC_SOLID_SOFT | NPC_SOLID_HARD);
      npc_boss[3].bits &= ~(NPC_SOLID_SOFT | NPC_SOLID_HARD);
      npc_boss[4].bits &= ~(NPC_SOLID_SOFT | NPC_SOLID_HARD);
      npc_boss[5].bits &= ~(NPC_SOLID_SOFT | NPC_SOLID_HARD);
      // Fallthrough
    case 1001:
      ++npc_boss[0].act_wait;

      if (npc_boss[0].act_wait % 12 == 0) snd_play_sound(PRIO_NORMAL, 44, FALSE);

      npc_spawn_death_fx(npc_boss[0].x + (m_rand(-60, 60) * 0x200), npc_boss[0].y + (m_rand(-60, 60) * 0x200), 1, 1, 0);

      if (npc_boss[0].act_wait > 150) {
        npc_boss[0].act_wait = 0;
        npc_boss[0].act = 1002;
        cam_start_flash(npc_boss[0].x, npc_boss[0].y, FLASH_MODE_EXPLOSION);
        snd_play_sound(PRIO_NORMAL, 35, FALSE);
      }

      break;

    case 1002:
      cam_start_quake_big(40);

      ++npc_boss[0].act_wait;

      if (npc_boss[0].act_wait == 50) {
        npc_boss[0].cond = 0;
        npc_boss[1].cond = 0;
        npc_boss[2].cond = 0;
        npc_boss[3].cond = 0;
        npc_boss[4].cond = 0;
        npc_boss[5].cond = 0;

        npc_delete_by_class(350, TRUE);
        npc_delete_by_class(348, TRUE);
      }

      break;
  }

  if (npc->act > 420 && npc->act < 500) {
    npc_boss[3].bits |= NPC_SHOOTABLE;
    npc_boss[4].bits |= NPC_SHOOTABLE;
    npc_boss[5].bits |= NPC_SHOOTABLE;

    ++npc->act_wait;

    if (npc->act_wait > 300) {
      npc->act_wait = 0;

      if (player.x > npc->x) {
        for (i = 0; i < 8; ++i) {
          x = ((156 + m_rand(-4, 4)) * 0x200 * 0x10) / 4;
          y = (m_rand(8, 68) * 0x200 * 0x10) / 4;
          npc_spawn(350, x, y, 0, 0, 0, NULL, 0x100);
        }
      } else {
        for (i = 0; i < 8; ++i) {
          x = (m_rand(-4, 4) * 0x200 * 0x10) / 4;
          y = (m_rand(8, 68) * 0x200 * 0x10) / 4;
          npc_spawn(350, x, y, 0, 0, 2, NULL, 0x100);
        }
      }
    }

    if (npc->act_wait == 270 || npc->act_wait == 280 || npc->act_wait == 290) {
      npc_spawn(353, npc->x, npc->y - (52 * 0x200), 0, 0, 1, NULL, 0x100);
      snd_play_sound(PRIO_NORMAL, 39, FALSE);

      for (i = 0; i < 4; ++i) npc_spawn(4, npc->x, npc->y - (52 * 0x200), 0, 0, 0, NULL, 0x100);
    }

    if (npc->life > 500) {
      if (m_rand(0, 10) == 2) {
        x = npc->x + (m_rand(-40, 40) * 0x200);
        y = npc->y + (m_rand(0, 40) * 0x200);
        npc_spawn(270, x, y, 0, 0, 3, NULL, 0);
      }
    } else {
      if (m_rand(0, 4) == 2) {
        x = npc->x + (m_rand(-40, 40) * 0x200);
        y = npc->y + (m_rand(0, 40) * 0x200);
        npc_spawn(270, x, y, 0, 0, 3, NULL, 0);
      }
    }
  }

  if (npc->shock != 0) {
    if (++flash / 2 % 2)
      npc_boss[3].anim = 1;
    else
      npc_boss[3].anim = 0;
  } else {
    npc_boss[3].anim = 0;
  }

  if (npc->act > 420) npc_boss[3].anim += 2;

  boss_act_ballos_eye(&npc_boss[1]);
  boss_act_ballos_eye(&npc_boss[2]);
  boss_act_ballos_body(&npc_boss[3]);
  boss_act_ballos_hit_top(&npc_boss[4]);
  boss_act_ballos_hit_mid(&npc_boss[5]);
}
