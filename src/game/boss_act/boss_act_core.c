#include "game/boss_act/boss_act.h"

void boss_act_core_face(npc_t *npc) {
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

      npc->x = npc_boss[0].x - (36 * 0x200);
      npc->y = npc_boss[0].y;
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

void boss_act_core_tail(npc_t *npc) {
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

      npc->x = npc_boss[0].x + (44 * 0x200);
      npc->y = npc_boss[0].y;
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

void boss_act_core_mini(npc_t *npc) {
  static const rect_t rect[3] = {
    {256, 0, 320, 40},
    {256, 40, 320, 80},
    {256, 80, 320, 120},
  };

  int xvel, yvel;
  u8 deg;

  npc->life = 1000;

  switch (npc->act) {
    case 10:
      npc->anim = 2;
      npc->bits &= ~NPC_SHOOTABLE;
      break;

    case 100:
      npc->act = 101;
      npc->anim = 2;
      npc->act_wait = 0;
      npc->tgt_x = npc_boss[0].x + (m_rand(-0x80, 0x20) * 0x200);
      npc->tgt_y = npc_boss[0].y + (m_rand(-0x40, 0x40) * 0x200);
      npc->bits |= NPC_SHOOTABLE;
      // Fallthrough
    case 101:
      npc->x += (npc->tgt_x - npc->x) / 0x10;
      npc->y += (npc->tgt_y - npc->y) / 0x10;

      ++npc->act_wait;

      if (npc->act_wait > 50) npc->anim = 0;

      break;

    case 120:
      npc->act = 121;
      npc->act_wait = 0;
      // Fallthrough
    case 121:
      ++npc->act_wait;

      if (npc->act_wait / 2 % 2)
        npc->anim = 0;
      else
        npc->anim = 1;

      if (npc->act_wait > 20) npc->act = 130;

      break;

    case 130:
      npc->act = 131;
      npc->anim = 2;
      npc->act_wait = 0;
      npc->tgt_x = npc->x + (m_rand(24, 48) * 0x200);
      npc->tgt_y = npc->y + (m_rand(-4, 4) * 0x200);
      // Fallthrough
    case 131:
      npc->x += (npc->tgt_x - npc->x) / 0x10;
      npc->y += (npc->tgt_y - npc->y) / 0x10;

      ++npc->act_wait;

      if (npc->act_wait > 50) {
        npc->act = 140;
        npc->anim = 0;
      }

      if (npc->act_wait == 1 || npc->act_wait == 3) {
        deg = m_atan2(npc->x - player.x, npc->y - player.y);
        deg += (u8)m_rand(-2, 2);
        yvel = m_sin(deg) * 2;
        xvel = m_cos(deg) * 2;
        npc_spawn(178, npc->x, npc->y, xvel, yvel, 0, NULL, 0x100);
        snd_play_sound(PRIO_NORMAL, 39, FALSE);
      }

      break;

    case 140:
      npc->x += (npc->tgt_x - npc->x) / 0x10;
      npc->y += (npc->tgt_y - npc->y) / 0x10;
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

  if (npc->shock != 0) npc->tgt_x += 2 * 0x200;

  npc->rect = &rect[npc->anim];
}

void boss_act_core_hit(npc_t *npc) {
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

void boss_act_core(npc_t *root) {
  npc_t *npc = npc_boss;

  static u8 flash;
  bool in_shock = FALSE;
  int i;
  u8 deg;
  int xvel, yvel;

  switch (npc->act) {
    case 0:
      npc->act = 10;
      npc->exp = 1;
      npc->cond = NPCCOND_ALIVE;
      npc->bits = (NPC_INVULNERABLE | NPC_IGNORE_SOLIDITY | NPC_SHOW_DAMAGE);
      npc->life = 650;
      npc->snd_hit = 114;
      npc->x = 77 * 0x10 * 0x200;
      npc->y = 14 * 0x10 * 0x200;
      npc->xvel = 0;
      npc->yvel = 0;
      npc->event_num = 1000;
      npc->bits |= NPC_EVENT_WHEN_KILLED;

      npc_boss[4].cond = 0x80;
      npc_boss[4].act = 10;

      npc_boss[5].cond = 0x80;
      npc_boss[5].act = 10;

      npc_boss[8].cond = 0x80;
      npc_boss[8].bits = (NPC_INVULNERABLE | NPC_IGNORE_SOLIDITY);
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
      npc_boss[1].act = 10;
      npc_boss[1].bits = (NPC_INVULNERABLE | NPC_IGNORE_SOLIDITY | NPC_SHOOTABLE);
      npc_boss[1].life = 1000;
      npc_boss[1].snd_hit = 54;
      npc_boss[1].hit.back = 24 * 0x200;
      npc_boss[1].hit.top = 16 * 0x200;
      npc_boss[1].hit.bottom = 16 * 0x200;
      npc_boss[1].view.front = 32 * 0x200;
      npc_boss[1].view.top = 20 * 0x200;
      npc_boss[1].x = npc->x - (8 * 0x200);
      npc_boss[1].y = npc->y - (64 * 0x200);

      npc_boss[2] = npc_boss[1];
      npc_boss[2].x = npc->x + (16 * 0x200);
      npc_boss[2].y = npc->y;

      npc_boss[3] = npc_boss[1];
      npc_boss[3].x = npc->x - (8 * 0x200);
      npc_boss[3].y = npc->y + (64 * 0x200);

      npc_boss[6] = npc_boss[1];
      npc_boss[6].x = npc->x - (48 * 0x200);
      npc_boss[6].y = npc->y - (32 * 0x200);

      npc_boss[7] = npc_boss[1];
      npc_boss[7].x = npc->x - (48 * 0x200);
      npc_boss[7].y = npc->y + (32 * 0x200);

      npc_boss_max = 11;
      for (int i = 0; i <= npc_boss_max; ++i)
        npc_boss[i].surf = SURFACE_ID_LEVEL_SPRITESET_2;

      break;

    case 200:
      npc->act = 201;
      npc->act_wait = 0;
      npc_boss[11].bits &= ~NPC_SHOOTABLE;
      npc_doctor_state.crystal_y = 0;
      snd_stop_sound(40);
      snd_stop_sound(41);
      // Fallthrough
    case 201:
      npc->tgt_x = player.x;
      npc->tgt_y = player.y;

      ++npc->act_wait;

      if (npc->act_wait > 400) {
        ++npc->count1;
        snd_play_sound(PRIO_NORMAL, 115, FALSE);

        if (npc->count1 > 3) {
          npc->count1 = 0;
          npc->act = 220;
          npc_boss[4].anim = 0;
          npc_boss[5].anim = 0;
          in_shock = TRUE;
        } else {
          npc->act = 210;
          npc_boss[4].anim = 0;
          npc_boss[5].anim = 0;
          in_shock = TRUE;
        }
      }

      break;

    case 210:
      npc->act = 211;
      npc->act_wait = 0;
      npc->count2 = npc->life;
      npc_boss[11].bits |= NPC_SHOOTABLE;
      // Fallthrough
    case 211:
      npc->tgt_x = player.x;
      npc->tgt_y = player.y;

      if (npc->shock != 0) {
        if (++flash / 2 % 2) {
          npc_boss[4].anim = 0;
          npc_boss[5].anim = 0;
        } else {
          npc_boss[4].anim = 1;
          npc_boss[5].anim = 1;
        }
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

      if (npc->act_wait < 200 && npc->act_wait % 20 == 1)
        npc_spawn(179, npc->x + (m_rand(-48, -16) * 0x200), npc->y + (m_rand(-64, 64) * 0x200), 0, 0, 0, NULL, 0x100);

      if (npc->act_wait > 400 || npc->life < npc->count2 - 200) {
        npc->act = 200;
        npc_boss[4].anim = 2;
        npc_boss[5].anim = 0;
        in_shock = TRUE;
      }

      break;

    case 220:
      npc->act = 221;
      npc->act_wait = 0;
      npc_doctor_state.crystal_y = 1;
      npc_boss[11].bits |= NPC_SHOOTABLE;
      cam_start_quake_small(100);
      snd_play_sound_freq(PRIO_HIGH, 40, 1000, TRUE);
      snd_play_sound_freq(PRIO_HIGH, 41, 1100, TRUE);
      // Fallthrough
    case 221:
      ++npc->act_wait;
      npc_spawn(199, player.x + (m_rand(-50, 150) * 0x200 * 2), player.y + (m_rand(-160, 160) * 0x200), 0, 0, 0, NULL, 0x100);
      player.xvel -= 0x20;
      player.cond |= PLRCOND_IN_WIND;

      if (npc->shock != 0) {
        if (++flash / 2 % 2) {
          npc_boss[4].anim = 0;
          npc_boss[5].anim = 0;
        } else {
          npc_boss[4].anim = 1;
          npc_boss[5].anim = 1;
        }
      } else {
        npc_boss[4].anim = 0;
        npc_boss[5].anim = 0;
      }

      if (npc->act_wait == 300 || npc->act_wait == 350 || npc->act_wait == 400) {
        deg = m_atan2(npc->x - player.x, npc->y - player.y);
        yvel = m_sin(deg) * 3;
        xvel = m_cos(deg) * 3;
        npc_spawn(218, npc->x - (40 * 0x200), npc->y, xvel, yvel, 0, NULL, 0x100);
        snd_play_sound(PRIO_NORMAL, 101, FALSE);
      }

      if (npc->act_wait > 400) {
        npc->act = 200;
        npc_boss[4].anim = 2;
        npc_boss[5].anim = 0;
        in_shock = TRUE;
      }

      break;

    case 500:
      snd_stop_sound(40);
      snd_stop_sound(41);

      npc->act = 501;
      npc->act_wait = 0;
      npc->xvel = 0;
      npc->yvel = 0;
      npc_boss[4].anim = 2;
      npc_boss[5].anim = 0;
      npc_boss[1].act = 200;
      npc_boss[2].act = 200;
      npc_boss[3].act = 200;
      npc_boss[6].act = 200;
      npc_boss[7].act = 200;

      cam_start_quake_small(20);

      for (i = 0; i < 0x20; ++i)
        npc_spawn(4, npc->x + (m_rand(-128, 128) * 0x200), npc->y + (m_rand(-64, 64) * 0x200),
          m_rand(-128, 128) * 0x200, m_rand(-128, 128) * 0x200, 0, NULL, 0x100);

      for (i = 0; i < 12; ++i) npc_boss[i].bits &= ~(NPC_INVULNERABLE | NPC_SHOOTABLE);

      // Fallthrough
    case 501:
      ++npc->act_wait;

      if (npc->act_wait % 16)
        npc_spawn(4, npc->x + (m_rand(-64, 64) * 0x200), npc->y + (m_rand(-32, 32) * 0x200), m_rand(-128, 128) * 0x200,
          m_rand(-128, 128) * 0x200, 0, NULL, 0x100);

      if (npc->act_wait / 2 % 2)
        npc->x -= 0x200;
      else
        npc->x += 0x200;

      if (npc->x < 63 * 0x10 * 0x200)
        npc->x += 0x80;
      else
        npc->x -= 0x80;

      if (npc->y < 11 * 0x10 * 0x200)
        npc->y += 0x80;
      else
        npc->y -= 0x80;

      break;

    case 600:
      npc->act = 601;
      npc_boss[4].act = 50;
      npc_boss[5].act = 50;
      npc_boss[8].bits &= ~NPC_INVULNERABLE;
      npc_boss[9].bits &= ~NPC_INVULNERABLE;
      npc_boss[10].bits &= ~NPC_INVULNERABLE;
      npc_boss[11].bits &= ~NPC_INVULNERABLE;
      // Fallthrough
    case 601:
      ++npc->act_wait;

      if (npc->act_wait / 2 % 2)
        npc->x -= 4 * 0x200;
      else
        npc->x += 4 * 0x200;

      break;
  }

  if (in_shock) {
    cam_start_quake_small(20);

    npc_boss[1].act = 100;
    npc_boss[2].act = 100;
    npc_boss[3].act = 100;
    npc_boss[6].act = 100;
    npc_boss[7].act = 100;

    snd_play_sound(PRIO_NORMAL, 26, FALSE);

    for (i = 0; i < 8; ++i)
      npc_spawn(4, npc_boss[4].x + (m_rand(-32, 16) * 0x200), npc_boss[4].y, m_rand(-0x200, 0x200),
        m_rand(-0x100, 0x100), 0, NULL, 0x100);
  }

  if (npc->act >= 200 && npc->act < 300) {
    switch (npc->act_wait) {
      case 80:
        npc_boss[1].act = 120;
        break;
      case 110:
        npc_boss[2].act = 120;
        break;
      case 140:
        npc_boss[3].act = 120;
        break;
      case 170:
        npc_boss[6].act = 120;
        break;
      case 200:
        npc_boss[7].act = 120;
        break;
    }

    if (npc->x < npc->tgt_x + (10 * 0x10 * 0x200)) npc->xvel += 4;
    if (npc->x > npc->tgt_x + (10 * 0x10 * 0x200)) npc->xvel -= 4;

    if (npc->y < npc->tgt_y) npc->yvel += 4;
    if (npc->y > npc->tgt_y) npc->yvel -= 4;
  }

  if (npc->xvel > 0x80) npc->xvel = 0x80;
  if (npc->xvel < -0x80) npc->xvel = -0x80;

  if (npc->yvel > 0x80) npc->yvel = 0x80;
  if (npc->yvel < -0x80) npc->yvel = -0x80;

  npc->x += npc->xvel;
  npc->y += npc->yvel;

  boss_act_core_face(&npc_boss[4]);

  boss_act_core_tail(&npc_boss[5]);

  boss_act_core_mini(&npc_boss[1]);
  boss_act_core_mini(&npc_boss[2]);
  boss_act_core_mini(&npc_boss[3]);
  boss_act_core_mini(&npc_boss[6]);
  boss_act_core_mini(&npc_boss[7]);

  boss_act_core_hit(&npc_boss[8]);
  boss_act_core_hit(&npc_boss[9]);
  boss_act_core_hit(&npc_boss[10]);
  boss_act_core_hit(&npc_boss[11]);
}
