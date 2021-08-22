#include "game/boss_act/boss_act.h"

static void boss_act_undead_core_head(npc_t *npc) {
  static const rect_t rect[4] = {
    {0, 0, 72, 112},
    {0, 112, 72, 224},
    {160, 0, 232, 112},
    {0, 0, 0, 0},
  };

  switch (npc->act) {
    case 10:
      npc->act = 11;
      npc->anim = 2;
      npc->bits = NPC_IGNORE_SOLIDITY;
      npc->view.front = 36 * 0x200;
      npc->view.top = 56 * 0x200;
      // Fallthrough
    case 11:
      npc->x = npc_boss[0].x - (36 * 0x200);
      npc->y = npc_boss[0].y;
      break;

    case 50:
      npc->act = 51;
      npc->act_wait = 112;
      // Fallthrough
    case 51:
      --npc->act_wait;

      if (npc->act_wait == 0) {
        npc->act = 100;
        npc->anim = 3;
      }

      break;

    case 100:
      npc->anim = 3;
      break;
  }

  npc->rect = &rect[npc->anim];

  if (npc->act == 51) {
    npc->rect_delta.bottom = npc->rect->top + npc->act_wait - npc->rect->bottom;
    npc->rect_prev = NULL;
  }
}

static void boss_act_undead_core_tail(npc_t *npc) {
  static const rect_t rect[3] = {
    {72, 0, 160, 112},
    {72, 112, 160, 224},
    {0, 0, 0, 0},
  };

  switch (npc->act) {
    case 10:
      npc->act = 11;
      npc->anim = 0;
      npc->bits = NPC_IGNORE_SOLIDITY;
      npc->view.front = 44 * 0x200;
      npc->view.top = 56 * 0x200;
      // Fallthrough
    case 11:
      npc->x = npc_boss[0].x + (44 * 0x200);
      npc->y = npc_boss[0].y;
      break;

    case 50:
      npc->act = 51;
      npc->act_wait = 112;
      // Fallthrough
    case 51:
      --npc->act_wait;

      if (npc->act_wait == 0) {
        npc->act = 100;
        npc->anim = 2;
      }

      break;

    case 100:
      npc->anim = 2;
      break;
  }

  npc->rect = &rect[npc->anim];

  if (npc->act == 51) {
    npc->rect_delta.bottom = npc->rect->top + npc->act_wait - npc->rect->bottom;
    npc->rect_prev = NULL;
  }
}

static void boss_act_undead_core_face(npc_t *npc) {
  static const rect_t rect[5] = {
    {0, 0, 0, 0}, {160, 112, 232, 152}, {160, 152, 232, 192},
    {160, 192, 232, 232}, {248, 160, 320, 200},
  };

  switch (npc->act) {
    case 0:
      npc->anim = 0;
      break;

    case 10:
      npc->anim = 1;
      break;

    case 20:
      npc->anim = 2;
      break;

    case 30:
      npc->act = 31;
      npc->anim = 3;
      npc->act_wait = 100;
      // Fallthrough
    case 31:
      ++npc->act_wait;

      if (npc->act_wait > 300) npc->act_wait = 0;

      if (npc->act_wait > 250 && npc->act_wait % 0x10 == 1)
        snd_play_sound(PRIO_NORMAL, 26, FALSE);

      if (npc->act_wait > 250 && npc->act_wait % 0x10 == 7) {
        npc_spawn(293, npc->x, npc->y, 0, 0, 0, NULL, 0x80);
        snd_play_sound(PRIO_NORMAL, 101, FALSE);
      }

      if (npc->act_wait == 200) snd_play_sound(PRIO_NORMAL, 116, FALSE);

      if (npc->act_wait > 200 && npc->act_wait % 2 != 0)
        npc->anim = 4;
      else
        npc->anim = 3;

      break;
  }

  npc->view.back = 36 * 0x200;
  npc->view.front = 36 * 0x200;
  npc->view.top = 20 * 0x200;

  npc->x = npc_boss[0].x - (36 * 0x200);
  npc->y = npc_boss[0].y + (4 * 0x200);

  npc->bits = NPC_IGNORE_SOLIDITY;

  npc->rect = &rect[npc->anim];
}

static void boss_act_undead_core_mini(npc_t *npc) {
  static const rect_t rect[3] = {
    {256, 0, 320, 40},
    {256, 40, 320, 80},
    {256, 80, 320, 120},
  };

  int deg;

  if (npc->cond == 0) return;

  npc->life = 1000;

  switch (npc->act) {
    case 0:
      npc->bits &= ~NPC_SHOOTABLE;
      break;

    case 5:
      npc->anim = 0;
      npc->bits &= ~NPC_SHOOTABLE;
      ++npc->count2;
      npc->count2 %= 0x100;
      break;

    case 10:
      npc->anim = 0;
      npc->bits &= ~NPC_SHOOTABLE;
      npc->count2 += 2;
      npc->count2 %= 0x100;
      break;

    case 20:
      npc->anim = 1;
      npc->bits &= ~NPC_SHOOTABLE;
      npc->count2 += 2;
      npc->count2 %= 0x100;
      break;

    case 30:
      npc->anim = 0;
      npc->bits &= ~NPC_SHOOTABLE;
      npc->count2 += 4;
      npc->count2 %= 0x100;
      break;

    case 200:
      npc->act = 201;
      npc->anim = 2;
      npc->xvel = 0;
      npc->yvel = 0;
      // Fallthrough
    case 201:
      npc->xvel += 0x20;

      npc->x += npc->xvel;

      if (npc->x > (stage_data->width * 0x200 * 0x10) + (2 * 0x200 * 0x10))
        npc->cond = 0;

      break;
  }

  if (npc->act < 50) {
    if (npc->count1 != 0)
      deg = npc->count2 + 0x80;
    else
      deg = npc->count2 + 0x180;

    npc->x = npc->parent->x - (8 * 0x200) + (m_cos(deg / 2) * 0x30);
    npc->y = npc->parent->y + (m_sin(deg / 2) * 0x50);
  }

  npc->rect = &rect[npc->anim];
}

static void boss_act_undead_core_hit(npc_t *npc) {
  switch (npc->count1) {
    case 0:
      npc->x = npc_boss[0].x;
      npc->y = npc_boss[0].y - (32 * 0x200);
      break;

    case 1:
      npc->x = npc_boss[0].x + (28 * 0x200);
      npc->y = npc_boss[0].y;
      break;

    case 2:
      npc->x = npc_boss[0].x + (4 * 0x200);
      npc->y = npc_boss[0].y + (32 * 0x200);
      break;

    case 3:
      npc->x = npc_boss[0].x - (28 * 0x200);
      npc->y = npc_boss[0].y + (4 * 0x200);
      break;
  }
}

void boss_act_undead_core(npc_t *root) {
  npc_t *npc = npc_boss;

  static u8 flash;
  bool in_shock = FALSE;
  int x, y;
  int i;

  static int life;

  switch (npc->act) {
    case 1:
      npc->act = 10;
      npc->exp = 1;
      npc->cond = NPCCOND_ALIVE;
      npc->bits = (NPC_INVULNERABLE | NPC_IGNORE_SOLIDITY | NPC_SHOW_DAMAGE);
      npc->life = 700;
      npc->snd_hit = 114;
      npc->x = 592 * 0x200;
      npc->y = 120 * 0x200;
      npc->xvel = 0;
      npc->yvel = 0;
      npc->event_num = 1000;
      npc->bits |= NPC_EVENT_WHEN_KILLED;

      npc_boss[3].cond = 0x80;
      npc_boss[3].act = 0;

      npc_boss[4].cond = 0x80;
      npc_boss[4].act = 10;

      npc_boss[5].cond = 0x80;
      npc_boss[5].act = 10;

      npc_boss[8].cond = 0x80;
      npc_boss[8].bits = NPC_IGNORE_SOLIDITY;
      npc_boss[8].view.front = 0;
      npc_boss[8].view.top = 0;
      npc_boss[8].hit.back = 40 * 0x200;
      npc_boss[8].hit.top = 16 * 0x200;
      npc_boss[8].hit.bottom = 16 * 0x200;
      npc_boss[8].count1 = 0;

      npc_boss[9] = npc_boss[8];
      npc_boss[9].hit.back = 36 * 0x200;
      npc_boss[9].hit.top = 24 * 0x200;
      npc_boss[9].hit.bottom = 24 * 0x200;
      npc_boss[9].count1 = 1;

      npc_boss[10] = npc_boss[8];
      npc_boss[10].hit.back = 44 * 0x200;
      npc_boss[10].hit.top = 8 * 0x200;
      npc_boss[10].hit.bottom = 8 * 0x200;
      npc_boss[10].count1 = 2;

      npc_boss[11] = npc_boss[8];
      npc_boss[11].cond |= 0x10;
      npc_boss[11].hit.back = 20 * 0x200;
      npc_boss[11].hit.top = 20 * 0x200;
      npc_boss[11].hit.bottom = 20 * 0x200;
      npc_boss[11].count1 = 3;

      npc_boss[1].cond = 0x80;
      npc_boss[1].act = 0;
      npc_boss[1].bits = (NPC_IGNORE_SOLIDITY | NPC_SHOOTABLE);
      npc_boss[1].life = 1000;
      npc_boss[1].snd_hit = 54;
      npc_boss[1].hit.back = 24 * 0x200;
      npc_boss[1].hit.top = 16 * 0x200;
      npc_boss[1].hit.bottom = 16 * 0x200;
      npc_boss[1].view.front = 32 * 0x200;
      npc_boss[1].view.top = 20 * 0x200;
      npc_boss[1].parent = npc;

      npc_boss[2] = npc_boss[1];
      npc_boss[2].count2 = 0x80;

      npc_boss[6] = npc_boss[1];
      npc_boss[6].count1 = 1;

      npc_boss[7] = npc_boss[1];
      npc_boss[7].count1 = 1;
      npc_boss[7].count2 = 0x80;

      life = npc->life;

      npc_boss_max = 11;
      for (int i = 0; i <= npc_boss_max; ++i)
        npc_boss[i].surf = SURFACE_ID_LEVEL_SPRITESET_2;

      break;

    case 15:
      npc->act = 16;
      in_shock = TRUE;
      npc->dir = 0;
      npc_boss[3].act = 10;
      npc_boss[4].anim = 0;
      break;

    case 20:
      npc->act = 210;
      in_shock = TRUE;
      npc->dir = 0;
      npc_boss[1].act = 5;
      npc_boss[2].act = 5;
      npc_boss[6].act = 5;
      npc_boss[7].act = 5;
      break;

    case 200:
      npc->act = 201;
      npc->act_wait = 0;
      npc_boss[3].act = 0;
      npc_boss[4].anim = 2;
      npc_boss[5].anim = 0;
      npc_boss[8].bits &= ~NPC_INVULNERABLE;
      npc_boss[9].bits &= ~NPC_INVULNERABLE;
      npc_boss[10].bits &= ~NPC_INVULNERABLE;
      npc_boss[11].bits &= ~NPC_SHOOTABLE;
      npc_doctor_state.crystal_y = 0;
      snd_stop_sound(40);
      snd_stop_sound(41);
      in_shock = TRUE;
      // Fallthrough
    case 201:
      ++npc->act_wait;

      if (npc->dir == 2 || npc->anim > 0 || npc->life < 200) {
        if (npc->act_wait > 200) {
          ++npc->count1;
          snd_play_sound(PRIO_NORMAL, 115, FALSE);

          if (npc->life < 200) {
            npc->act = 230;
          } else {
            if (npc->count1 > 2)
              npc->act = 220;
            else
              npc->act = 210;
          }
        }
      }

      break;

    case 210:
      npc->act = 211;
      npc->act_wait = 0;
      npc_boss[3].act = 10;
      npc_boss[8].bits |= NPC_INVULNERABLE;
      npc_boss[9].bits |= NPC_INVULNERABLE;
      npc_boss[10].bits |= NPC_INVULNERABLE;
      npc_boss[11].bits |= NPC_SHOOTABLE;
      life = npc->life;
      in_shock = TRUE;
      // Fallthrough
    case 211:
      ++flash;

      if (npc->shock != 0 && flash / 2 % 2) {
        npc_boss[4].anim = 1;
        npc_boss[5].anim = 1;
      } else {
        npc_boss[4].anim = 0;
        npc_boss[5].anim = 0;
      }

      ++npc->act_wait;

      if (npc->act_wait % 100 == 1) {
        npc_curly_state.shoot_wait = m_rand(80, 100);
        npc_curly_state.shoot_x = npc_boss[11].x;
        npc_curly_state.shoot_y = npc_boss[11].y;
      }

      if (npc->act_wait < 300) {
        if (npc->act_wait % 120 == 1) npc_spawn(288, npc->x - (32 * 0x200), npc->y - (16 * 0x200), 0, 0, 1, NULL, 0x20);

        if (npc->act_wait % 120 == 61)
          npc_spawn(288, npc->x - (32 * 0x200), npc->y + (16 * 0x200), 0, 0, 3, NULL, 0x20);
      }

      if (npc->life < life - 50 || npc->act_wait > 400) npc->act = 200;

      break;

    case 220:
      npc->act = 221;
      npc->act_wait = 0;
      npc->count1 = 0;
      npc_doctor_state.crystal_y = 1;
      npc_boss[3].act = 20;
      npc_boss[8].bits |= NPC_INVULNERABLE;
      npc_boss[9].bits |= NPC_INVULNERABLE;
      npc_boss[10].bits |= NPC_INVULNERABLE;
      npc_boss[11].bits |= NPC_SHOOTABLE;
      cam_start_quake_small(100);
      life = npc->life;
      in_shock = TRUE;
      // Fallthrough
    case 221:
      ++npc->act_wait;

      if (npc->act_wait % 40 == 1) {
        switch (m_rand(0, 3)) {
          case 0:
            x = npc_boss[1].x;
            y = npc_boss[1].y;
            break;
          case 1:
            x = npc_boss[2].x;
            y = npc_boss[2].y;
            break;
          case 2:
            x = npc_boss[6].x;
            y = npc_boss[6].y;
            break;
          default:
            x = npc_boss[7].x;
            y = npc_boss[7].y;
            break;
        }

        snd_play_sound(PRIO_NORMAL, 25, FALSE);
        npc_spawn(285, x - (16 * 0x200), y, 0, 0, 0, NULL, 0x100);
        npc_spawn(285, x - (16 * 0x200), y, 0, 0, 0x400, NULL, 0x100);
      }

      ++flash;

      if (npc->shock != 0 && flash / 2 % 2) {
        npc_boss[4].anim = 1;
        npc_boss[5].anim = 1;
      } else {
        npc_boss[4].anim = 0;
        npc_boss[5].anim = 0;
      }

      if (npc->life < life - 150 || npc->act_wait > 400 || npc->life < 200) npc->act = 200;

      break;

    case 230:
      npc->act = 231;
      npc->act_wait = 0;

      npc_boss[3].act = 30;

      npc_boss[8].bits |= NPC_INVULNERABLE;
      npc_boss[9].bits |= NPC_INVULNERABLE;
      npc_boss[10].bits |= NPC_INVULNERABLE;
      npc_boss[11].bits |= NPC_SHOOTABLE;

      snd_play_sound(PRIO_NORMAL, 25, FALSE);

      npc_spawn(285, npc_boss[3].x - (16 * 0x200), npc_boss[3].y, 0, 0, 0, NULL, 0x100);
      npc_spawn(285, npc_boss[3].x - (16 * 0x200), npc_boss[3].y, 0, 0, 0x400, NULL, 0x100);
      npc_spawn(285, npc_boss[3].x, npc_boss[3].y - (16 * 0x200), 0, 0, 0, NULL, 0x100);
      npc_spawn(285, npc_boss[3].x, npc_boss[3].y - (16 * 0x200), 0, 0, 0x400, NULL, 0x100);
      npc_spawn(285, npc_boss[3].x, npc_boss[3].y + (16 * 0x200), 0, 0, 0, NULL, 0x100);
      npc_spawn(285, npc_boss[3].x, npc_boss[3].y + (16 * 0x200), 0, 0, 0x400, NULL, 0x100);

      life = npc->life;
      in_shock = TRUE;
      // Fallthrough
    case 231:
      ++flash;

      if (npc->shock != 0 && flash / 2 % 2) {
        npc_boss[4].anim = 1;
        npc_boss[5].anim = 1;
      } else {
        npc_boss[4].anim = 0;
        npc_boss[5].anim = 0;
      }

      if (++npc->act_wait % 100 == 1) {
        npc_curly_state.shoot_wait = m_rand(80, 100);
        npc_curly_state.shoot_x = npc_boss[11].x;
        npc_curly_state.shoot_y = npc_boss[11].y;
      }

      const int w = npc->act_wait % 120;
      if (w == 1)
        npc_spawn(288, npc->x - (32 * 0x200), npc->y - (16 * 0x200), 0, 0, 1, NULL, 0x20);
      else if (w == 61)
        npc_spawn(288, npc->x - (32 * 0x200), npc->y + (16 * 0x200), 0, 0, 3, NULL, 0x20);

      break;

    case 500:
      snd_stop_sound(40);
      snd_stop_sound(41);
      npc->act = 501;
      npc->act_wait = 0;
      npc->xvel = 0;
      npc->yvel = 0;
      npc_boss[3].act = 0;
      npc_boss[4].anim = 2;
      npc_boss[5].anim = 0;
      npc_boss[1].act = 5;
      npc_boss[2].act = 5;
      npc_boss[6].act = 5;
      npc_boss[7].act = 5;
      cam_start_quake_small(20);

      for (i = 0; i < 100; ++i)
        npc_spawn(4, npc->x + (m_rand(-128, 128) * 0x200), npc->y + (m_rand(-64, 64) * 0x200),
          m_rand(-128, 128) * 0x200, m_rand(-128, 128) * 0x200, 0, NULL, 0);

      npc_delete_by_class(282, TRUE);
      npc_boss[11].bits &= ~NPC_SHOOTABLE;

      for (i = 0; i < 12; ++i) npc_boss[i].bits &= ~NPC_INVULNERABLE;
      // Fallthrough
    case 501:
      ++npc->act_wait;

      if (npc->act_wait % 0x10 != 0)
        npc_spawn(4, npc->x + (m_rand(-64, 64) * 0x200), npc->y + (m_rand(-32, 32) * 0x200), m_rand(-128, 128) * 0x200,
          m_rand(-128, 128) * 0x200, 0, NULL, 0x100);

      npc->x += 0x40;
      npc->y += 0x80;

      if (npc->act_wait > 200) {
        npc->act_wait = 0;
        npc->act = 1000;
      }

      break;

    case 1000:
      cam_start_quake_small(100);

      ++npc_boss[0].act_wait;

      if (npc_boss[0].act_wait % 8 == 0) snd_play_sound(PRIO_NORMAL, 44, FALSE);

      npc_spawn_death_fx(npc_boss[0].x + (m_rand(-72, 72) * 0x200), npc_boss[0].y + (m_rand(-64, 64) * 0x200), 1, 1, 0);

      if (npc_boss[0].act_wait > 100) {
        npc_boss[0].act_wait = 0;
        npc_boss[0].act = 1001;
        cam_start_flash(npc_boss[0].x, npc_boss[0].y, FLASH_MODE_EXPLOSION);
        snd_play_sound(PRIO_NORMAL, 35, FALSE);
      }

      break;

    case 1001:
      cam_start_quake_small(40);

      ++npc_boss[0].act_wait;

      if (npc_boss[0].act_wait > 50) {
        for (i = 0; i < 20; ++i)
          npc_boss[i].cond = 0;
        npc_delete_by_class(158, TRUE);
        npc_delete_by_class(301, TRUE);
        npc_boss_max = -1;
      }

      break;
  }

  if (in_shock) {
    cam_start_quake_small(20);

    if (npc->act == 201) npc_boss[1].act = npc_boss[2].act = npc_boss[6].act = npc_boss[7].act = 10;
    if (npc->act == 221) npc_boss[1].act = npc_boss[2].act = npc_boss[6].act = npc_boss[7].act = 20;
    if (npc->act == 231) npc_boss[1].act = npc_boss[2].act = npc_boss[6].act = npc_boss[7].act = 30;

    snd_play_sound(PRIO_NORMAL, 26, FALSE);

    for (i = 0; i < 8; ++i)
      npc_spawn(4, npc_boss[4].x + (m_rand(-32, 16) * 0x200), npc_boss[4].y, m_rand(-0x200, 0x200),
        m_rand(-0x100, 0x100), 0, NULL, 0x100);
  }

  if (npc->act >= 200 && npc->act < 300) {
    if (npc->x < 192 * 0x200) npc->dir = 2;
    if (npc->x > (stage_data->width - 4) * (0x200 * 0x10)) npc->dir = 0;

    if (npc->dir == 0)
      npc->xvel -= 4;
    else
      npc->xvel += 4;
  }

  switch (npc->act) {
    case 201:
    case 211:
    case 221:
    case 231:
      ++npc->count2;

      if (npc->count2 == 150) {
        npc->count2 = 0;
        npc_spawn(282, (stage_data->width * 0x200 * 0x10) + 0x40, (m_rand(-1, 3) + 10) * (0x200 * 0x10), 0, 0, 0, NULL, 0x30);
      } else if (npc->count2 == 75) {
        npc_spawn(282, (stage_data->width * 0x200 * 0x10) + 0x40, (m_rand(-3, 0) + 3) * (0x200 * 0x10), 0, 0, 0, NULL, 0x30);
      }

      break;
  }

  if (npc->xvel > 0x80) npc->xvel = 0x80;
  if (npc->xvel < -0x80) npc->xvel = -0x80;

  if (npc->yvel > 0x80) npc->yvel = 0x80;
  if (npc->yvel < -0x80) npc->yvel = -0x80;

  npc->x += npc->xvel;
  npc->y += npc->yvel;

  boss_act_undead_core_face(&npc_boss[3]);

  boss_act_undead_core_head(&npc_boss[4]);

  boss_act_undead_core_tail(&npc_boss[5]);

  boss_act_undead_core_mini(&npc_boss[1]);
  boss_act_undead_core_mini(&npc_boss[2]);
  boss_act_undead_core_mini(&npc_boss[6]);
  boss_act_undead_core_mini(&npc_boss[7]);

  boss_act_undead_core_hit(&npc_boss[8]);
  boss_act_undead_core_hit(&npc_boss[9]);
  boss_act_undead_core_hit(&npc_boss[10]);
  boss_act_undead_core_hit(&npc_boss[11]);
}
