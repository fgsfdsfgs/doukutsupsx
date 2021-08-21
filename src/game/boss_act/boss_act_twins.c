#include "game/boss_act/boss_act.h"

static void boss_act_twins_body(npc_t *npc) {
  u8 deg;

  static const rect_t rc_left[3] = {
    {0, 0, 40, 40},
    {40, 0, 80, 40},
    {80, 0, 120, 40},
  };

  static const rect_t rc_right[3] = {
    {0, 40, 40, 80},
    {40, 40, 80, 80},
    {80, 40, 120, 80},
  };

  switch (npc->act) {
    case 0:
      deg = ((npc->parent->count1 / 4) + npc->count1) % 0x100;
      npc->act = 10;
      npc->x += npc->parent->x + (m_cos(deg) * npc->parent->tgt_x);
      npc->y += npc->parent->y + (m_sin(deg) * npc->parent->tgt_y);
      // Fallthrough
    case 10:
      if (npc->x > player.x)
        npc->dir = 0;
      else
        npc->dir = 2;

      break;

    case 100:
      deg = ((npc->parent->count1 / 4) + npc->count1) % 0x100;
      npc->tgt_x = npc->parent->x + (m_cos(deg) * npc->parent->tgt_x);
      npc->tgt_y = npc->parent->y + (m_sin(deg) * npc->parent->tgt_y);

      npc->x += (npc->tgt_x - npc->x) / 8;
      npc->y += (npc->tgt_y - npc->y) / 8;

      if (npc->x > player.x)
        npc->dir = 0;
      else
        npc->dir = 2;

      break;

    case 1000:
      npc->act = 1001;
      npc->bits &= ~NPC_SHOOTABLE;
      // Fallthrough
    case 1001:
      deg = ((npc->parent->count1 / 4) + npc->count1) % 0x100;
      npc->tgt_x = npc->parent->x + (m_cos(deg) * npc->parent->tgt_x);
      npc->tgt_y = npc->parent->y + (m_sin(deg) * npc->parent->tgt_y);

      npc->x += (npc->tgt_x - npc->x) / 8;
      npc->y += (npc->tgt_y - npc->y) / 8;

      if (npc->x > npc->parent->x)
        npc->dir = 0;
      else
        npc->dir = 2;

      break;
  }

  if (++npc->anim_wait > 2) {
    npc->anim_wait = 0;
    ++npc->anim;
  }

  if (npc->anim > 2) npc->anim = 0;

  if (npc->dir == 0)
    npc->rect = &rc_left[npc->anim];
  else
    npc->rect = &rc_right[npc->anim];
}

static void boss_act_twins_head(npc_t *npc) {
  u8 deg;
  int xvel, yvel;

  static const rect_t rc_left[4] = {
    {0, 80, 40, 112},
    {40, 80, 80, 112},
    {80, 80, 120, 112},
    {120, 80, 160, 112},
  };

  static const rect_t rc_right[4] = {
    {0, 112, 40, 144},
    {40, 112, 80, 144},
    {80, 112, 120, 144},
    {120, 112, 160, 144},
  };

  switch (npc->act) {
    case 0:
      npc->act = 1;
      break;

    case 100:
      npc->act = 200;
      // Fallthrough
    case 200:
      npc->bits &= ~NPC_SHOOTABLE;
      npc->anim = 0;
      npc->hit.front = 16 * 0x200;
      npc->act = 201;
      npc->count1 = m_rand(100, 200);
      // Fallthrough
    case 201:
      if (npc->count1 != 0) {
        --npc->count1;
      } else {
        npc->act = 210;
        npc->act_wait = 0;
        npc->count2 = 0;
      }

      break;

    case 210:
      ++npc->act_wait;

      if (npc->act_wait == 3) npc->anim = 1;

      if (npc->act_wait == 6) {
        npc->anim = 2;
        npc->hit.front = 8 * 0x200;
        npc->bits |= NPC_SHOOTABLE;
        npc->count2 = 0;
      }

      if (npc->act_wait > 150) {
        npc->act = 220;
        npc->act_wait = 0;
      }

      if (npc->shock != 0) ++npc->count2;

      if (npc->count2 > 10) {
        snd_play_sound(PRIO_NORMAL, 51, FALSE);
        npc_spawn_death_fx(npc->x, npc->y, npc->view.back, 4, 0);
        npc->act = 300;
        npc->act_wait = 0;
        npc->anim = 3;
        npc->hit.front = 16 * 0x200;
      }

      break;

    case 220:
      ++npc->act_wait;

      if (npc->act_wait % 8 == 1) {
        deg = m_atan2(npc->x - player.x, npc->y - player.y);
        deg += (u8)m_rand(-6, 6);
        yvel = m_sin(deg);
        xvel = m_cos(deg);

        if (npc->dir == 0)
          npc_spawn(202, npc->x - (8 * 0x200), npc->y, xvel, yvel, 0, NULL, 0x100);
        else
          npc_spawn(202, npc->x + (8 * 0x200), npc->y, xvel, yvel, 0, NULL, 0x100);

        snd_play_sound(PRIO_NORMAL, 33, FALSE);
      }

      if (npc->act_wait > 50) npc->act = 200;

      break;

    case 300:
      ++npc->act_wait;

      if (npc->act_wait > 100) npc->act = 200;

      break;

    case 400:
      npc->act = 401;
      npc->act_wait = 0;
      npc->anim = 0;
      npc->hit.front = 16 * 0x200;
      npc->bits &= ~NPC_SHOOTABLE;
      // Fallthrough
    case 401:
      ++npc->act_wait;

      if (npc->act_wait == 3) npc->anim = 1;

      if (npc->act_wait == 6) {
        npc->anim = 2;
        npc->hit.front = 8 * 0x200;
        npc->bits |= NPC_SHOOTABLE;
        npc->count2 = 0;
      }

      if (npc->act_wait > 20 && npc->act_wait % 32 == 1) {
        deg = m_atan2(npc->x - player.x, npc->y - player.y);
        deg += (u8)m_rand(-6, 6);
        yvel = m_sin(deg);
        xvel = m_cos(deg);

        if (npc->dir == 0)
          npc_spawn(202, npc->x - (8 * 0x200), npc->y, xvel, yvel, 0, NULL, 0x100);
        else
          npc_spawn(202, npc->x + (8 * 0x200), npc->y, xvel, yvel, 0, NULL, 0x100);

        snd_play_sound(PRIO_NORMAL, 33, FALSE);
      }

      break;

    case 1000:
      npc->bits &= ~NPC_SHOOTABLE;
      npc->anim = 3;
      break;
  }

  npc->dir = npc->parent->dir;

  if (npc->dir == 0)
    npc->x = npc->parent->x - (4 * 0x200);
  else
    npc->x = npc->parent->x + (4 * 0x200);

  npc->y = npc->parent->y - (8 * 0x200);

  if (npc->dir == 0)
    npc->rect = &rc_left[npc->anim];
  else
    npc->rect = &rc_right[npc->anim];
}

void boss_act_twins(npc_t *root) {
  npc_t *npc = npc_boss;

  switch (npc->act) {
    case 0:
      npc->cond = 0x80;
      npc->dir = 0;
      npc->act = 10;
      npc->exp = 0;
      npc->x = 160 * 0x200;
      npc->y = 128 * 0x200;
      npc->view.front = 8 * 0x200;
      npc->view.top = 8 * 0x200;
      npc->view.back = 128 * 0x200;
      npc->view.bottom = 8 * 0x200;
      npc->snd_hit = 54;
      npc->hit.front = 8 * 0x200;
      npc->hit.top = 8 * 0x200;
      npc->hit.back = 8 * 0x200;
      npc->hit.bottom = 8 * 0x200;
      npc->bits = NPC_IGNORE_SOLIDITY;
      npc->bits |= NPC_EVENT_WHEN_KILLED;
      npc->size = 3;
      npc->damage = 0;
      npc->event_num = 1000;
      npc->life = 500;
      npc->count2 = m_rand(700, 1200);
      npc->tgt_x = 180;
      npc->tgt_y = 61;

      npc_boss[2].cond = 0x80;
      npc_boss[2].view.back = 20 * 0x200;
      npc_boss[2].view.front = 20 * 0x200;
      npc_boss[2].view.top = 16 * 0x200;
      npc_boss[2].view.bottom = 16 * 0x200;
      npc_boss[2].hit.back = 12 * 0x200;
      npc_boss[2].hit.front = 12 * 0x200;
      npc_boss[2].hit.top = 10 * 0x200;
      npc_boss[2].hit.bottom = 10 * 0x200;
      npc_boss[2].bits = (NPC_INVULNERABLE | NPC_IGNORE_SOLIDITY);
      npc_boss[2].parent = &npc_boss[3];
      npc_boss[2].cond |= 0x10;
      npc_boss[2].damage = 10;

      npc_boss[3].cond = 0x80;
      npc_boss[3].view.back = 20 * 0x200;
      npc_boss[3].view.front = 20 * 0x200;
      npc_boss[3].view.top = 20 * 0x200;
      npc_boss[3].view.bottom = 20 * 0x200;
      npc_boss[3].hit.back = 12 * 0x200;
      npc_boss[3].hit.front = 12 * 0x200;
      npc_boss[3].hit.top = 2 * 0x200;
      npc_boss[3].hit.bottom = 16 * 0x200;
      npc_boss[3].bits = NPC_IGNORE_SOLIDITY;
      npc_boss[3].parent = npc;
      npc_boss[3].damage = 10;

      npc_boss[4] = npc_boss[2];
      npc_boss[4].parent = &npc_boss[5];

      npc_boss[5] = npc_boss[3];
      npc_boss[5].count1 = 128;

      npc_boss_max = 5;
      for (int i = 0; i <= npc_boss_max; ++i) npc_boss[i].surf = SURFACE_ID_LEVEL_SPRITESET_2;

      break;

    case 20:
      if (--npc->tgt_x <= 0x70) {
        npc->act = 100;
        npc->act_wait = 0;
        npc_boss[2].act = 100;
        npc_boss[4].act = 100;
        npc_boss[3].act = 100;
        npc_boss[5].act = 100;
      }

      break;

    case 100:
      ++npc->act_wait;

      if (npc->act_wait < 100) {
        npc->count1 += 1;
      } else if (npc->act_wait < 120) {
        npc->count1 += 2;
      } else if (npc->act_wait < npc->count2) {
        npc->count1 += 4;
      } else if (npc->act_wait < npc->count2 + 40) {
        npc->count1 += 2;
      } else if (npc->act_wait < npc->count2 + 60) {
        npc->count1 += 1;
      } else {
        npc->act_wait = 0;
        npc->act = 110;
        npc->count2 = m_rand(400, 700);
        break;
      }

      if (npc->count1 > 0x3FF) npc->count1 -= 0x400;

      break;

    case 110:
      ++npc->act_wait;

      if (npc->act_wait < 20) {
        npc->count1 -= 1;
      } else if (npc->act_wait < 60) {
        npc->count1 -= 2;
      } else if (npc->act_wait < npc->count2) {
        npc->count1 -= 4;
      } else if (npc->act_wait < npc->count2 + 40) {
        npc->count1 -= 2;
      } else if (npc->act_wait < npc->count2 + 60) {
        npc->count1 -= 1;
      } else {
        if (npc->life < 300) {
          npc->act_wait = 0;
          npc->act = 400;
          npc_boss[2].act = 400;
          npc_boss[4].act = 400;
        } else {
          npc->act_wait = 0;
          npc->act = 100;
          npc->count2 = m_rand(400, 700);
        }

        break;
      }

      if (npc->count1 <= 0) npc->count1 += 0x400;

      break;

    case 400:
      ++npc->act_wait;

      if (npc->act_wait > 100) {
        npc->act_wait = 0;
        npc->act = 401;
      }

      break;

    case 401:
      ++npc->act_wait;

      if (npc->act_wait < 100) {
        npc->count1 += 1;
      } else if (npc->act_wait < 120) {
        npc->count1 += 2;
      } else if (npc->act_wait < 500) {
        npc->count1 += 4;
      } else if (npc->act_wait < 540) {
        npc->count1 += 2;
      } else if (npc->act_wait < 560) {
        npc->count1 += 1;
      } else {
        npc->act = 100;
        npc->act_wait = 0;
        npc_boss[2].act = 100;
        npc_boss[4].act = 100;
        break;
      }

      if (npc->count1 > 0x3FF) npc->count1 -= 0x400;

      break;

    case 1000:
      npc->act = 1001;
      npc->act_wait = 0;
      npc_boss[2].act = 1000;
      npc_boss[3].act = 1000;
      npc_boss[4].act = 1000;
      npc_boss[5].act = 1000;
      npc_spawn_death_fx(npc->x, npc->y, npc->view.back, 40, 0);
      // Fallthrough

    case 1001:
      ++npc->act_wait;

      if (npc->act_wait > 100) npc->act = 1010;

      npc_spawn(4, npc->x + (m_rand(-0x80, 0x80) * 0x200), npc->y + (m_rand(-70, 70) * 0x200), 0, 0, 0, NULL, 0x100);
      break;

    case 1010:
      npc->count1 += 4;

      if (npc->count1 > 0x3FF) npc->count1 -= 0x400;

      if (npc->tgt_x > 8) --npc->tgt_x;
      if (npc->tgt_y > 0) --npc->tgt_y;

      if (npc->tgt_x < -8) ++npc->tgt_x;
      if (npc->tgt_y < 0) ++npc->tgt_y;

      if (npc->tgt_y == 0) {
        npc->act = 1020;
        npc->act_wait = 0;
        cam_start_flash(npc_boss[0].x, npc_boss[0].y, FLASH_MODE_EXPLOSION);
        snd_play_sound(PRIO_NORMAL, 35, FALSE);
      }

      break;

    case 1020:
      if (++npc_boss[0].act_wait > 50) {
        npc_delete_by_class(211, TRUE);
        npc_boss[0].cond = 0;
        npc_boss[1].cond = 0;
        npc_boss[2].cond = 0;
        npc_boss[3].cond = 0;
        npc_boss[4].cond = 0;
        npc_boss[5].cond = 0;
        npc_boss[0].act = 0;
        npc_boss_max = -1;
      }

      break;
  }

  boss_act_twins_head(&npc_boss[2]);
  boss_act_twins_body(&npc_boss[3]);
  boss_act_twins_head(&npc_boss[4]);
  boss_act_twins_body(&npc_boss[5]);

  static const rect_t rc = {0, 0, 0, 0};
  npc->rect = &rc;
}
