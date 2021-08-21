#include "game/boss_act/boss_act.h"

void boss_act_heavy_press(npc_t *root) {
  npc_t *npc = npc_boss;
  static u8 flash;
  int i;
  int x;

  switch (npc->act) {
    case 0:
      npc->act = 10;
      npc->cond = 0x80;
      npc->exp = 1;
      npc->dir = 2;
      npc->x = 0;
      npc->y = 0;
      npc->view.front = 40 * 0x200;
      npc->view.top = 60 * 0x200;
      npc->view.back = 40 * 0x200;
      npc->view.bottom = 60 * 0x200;
      npc->snd_hit = 54;
      npc->hit.front = 49 * 0x200;
      npc->hit.top = 60 * 0x200;
      npc->hit.back = 40 * 0x200;
      npc->hit.bottom = 48 * 0x200;
      npc->bits = (NPC_IGNORE_SOLIDITY | NPC_SOLID_HARD | NPC_EVENT_WHEN_KILLED | NPC_SHOW_DAMAGE);
      npc->size = 3;
      npc->damage = 10;
      npc->event_num = 1000;
      npc->life = 700;
      npc_boss_max = 3;
      for (int i = 0; i <= npc_boss_max; ++i)
        npc_boss[i].surf = SURFACE_ID_LEVEL_SPRITESET_2;
      break;

    case 5:
      npc->act = 6;
      npc->x = 0;
      npc->y = 0;
      npc_boss[1].cond = 0;
      npc_boss[2].cond = 0;
      break;

    case 10:
      npc->act = 11;
      npc->x = 160 * 0x200;
      npc->y = 74 * 0x200;
      break;

    case 20:
      npc->damage = 0;
      npc->act = 21;
      npc->x = 160 * 0x200;
      npc->y = 413 * 0x200;
      npc->bits &= ~NPC_SOLID_HARD;
      npc_boss[1].cond = 0;
      npc_boss[2].cond = 0;
      // Fallthrough
    case 21:
      if (++npc->act_wait % 0x10 == 0)
        npc_spawn_death_fx(npc->x + (m_rand(-40, 40) * 0x200), npc->y + (m_rand(-60, 60) * 0x200), 1, 1, 0);

      break;

    case 30:
      npc->act = 31;
      npc->anim = 2;
      npc->x = 160 * 0x200;
      npc->y = 64 * 0x200;
      // Fallthrough
    case 31:
      npc->y += 4 * 0x200;

      if (npc->y >= 413 * 0x200) {
        npc->y = 413 * 0x200;
        npc->anim = 0;
        npc->act = 20;
        snd_play_sound(PRIO_NORMAL, 44, FALSE);

        for (i = 0; i < 5; ++i) {
          x = npc->x + (m_rand(-40, 40) * 0x200);
          npc_spawn(4, x, npc->y + (60 * 0x200), 0, 0, 0, NULL, 0x100);
        }
      }

      break;

    case 100:
      npc->act = 101;
      npc->count2 = 9;
      npc->act_wait = -100;

      npc_boss[1].cond = 0x80;
      npc_boss[1].hit.front = 14 * 0x200;
      npc_boss[1].hit.back = 14 * 0x200;
      npc_boss[1].hit.top = 8 * 0x200;
      npc_boss[1].hit.bottom = 8 * 0x200;
      npc_boss[1].bits = (NPC_INVULNERABLE | NPC_IGNORE_SOLIDITY);

      npc_boss[2] = npc_boss[1];

      npc_boss[3].cond = 0x90;
      npc_boss[3].bits |= NPC_SHOOTABLE;
      npc_boss[3].hit.front = 6 * 0x200;
      npc_boss[3].hit.back = 6 * 0x200;
      npc_boss[3].hit.top = 8 * 0x200;
      npc_boss[3].hit.bottom = 8 * 0x200;

      npc_spawn(325, npc->x, npc->y + (60 * 0x200), 0, 0, 0, NULL, 0x100);
      // Fallthrough
    case 101:
      if (npc->count2 > 1 && npc->life < npc->count2 * 70) {
        --npc->count2;

        for (i = 0; i < 5; ++i) {
          stage_set_tile(i + 8, npc->count2, 0);
          npc_spawn_death_fx((i + 8) * 0x200 * 0x10, npc->count2 * 0x200 * 0x10, 0, 4, 0);
          snd_play_sound(PRIO_NORMAL, 12, FALSE);
        }
      }

      if (++npc->act_wait == 81 || npc->act_wait == 241)
        npc_spawn(323, 48 * 0x200, 240 * 0x200, 0, 0, 1, NULL, 0x100);

      if (npc->act_wait == 1 || npc->act_wait == 161)
        npc_spawn(323, 272 * 0x200, 240 * 0x200, 0, 0, 1, NULL, 0x100);

      if (npc->act_wait >= 300) {
        npc->act_wait = 0;
        npc_spawn(325, npc->x, npc->y + (60 * 0x200), 0, 0, 0, NULL, 0x100);
      }

      break;

    case 500:
      npc_boss[3].bits &= ~NPC_SHOOTABLE;

      npc->act = 501;
      npc->act_wait = 0;
      npc->count1 = 0;

      npc_delete_by_class(325, TRUE);
      npc_delete_by_class(330, TRUE);
      // Fallthrough
    case 501:
      if (++npc->act_wait % 0x10 == 0) {
        snd_play_sound(PRIO_NORMAL, 12, FALSE);
        npc_spawn_death_fx(npc->x + (m_rand(-40, 40) * 0x200), npc->y + (m_rand(-60, 60) * 0x200), 1, 1, 0);
      }

      if (npc->act_wait == 95) npc->anim = 1;
      if (npc->act_wait == 98) npc->anim = 2;

      if (npc->act_wait > 100) npc->act = 510;

      break;

    case 510:
      npc->yvel += 0x40;
      npc->damage = 0x7F;
      npc->y += npc->yvel;

      if (npc->count1 == 0 && npc->y > 160 * 0x200) {
        npc->count1 = 1;
        npc->yvel = -0x200;
        npc->damage = 0;

        for (i = 0; i < 7; ++i) {
          stage_set_tile(i + 7, 14, 0);
          npc_spawn_death_fx((i + 7) * 0x200 * 0x10, 224 * 0x200, 0, 0, 0);
          snd_play_sound(PRIO_NORMAL, 12, FALSE);
        }
      }

      if (npc->y > 480 * 0x200) npc->act = 520;

      break;
  }

  npc_boss[1].x = npc->x - (24 * 0x200);
  npc_boss[1].y = npc->y + (52 * 0x200);

  npc_boss[2].x = npc->x + (24 * 0x200);
  npc_boss[2].y = npc->y + (52 * 0x200);

  npc_boss[3].x = npc->x;
  npc_boss[3].y = npc->y + (40 * 0x200);

  static const rect_t rc[3] = {
    {0, 0, 80, 120},
    {80, 0, 160, 120},
    {160, 0, 240, 120},
  };

  static const rect_t rc_damage[3] = {
    {0, 120, 80, 240},
    {80, 120, 160, 240},
    {160, 120, 240, 240},
  };

  if (npc->shock != 0) {
    if (++flash / 2 % 2)
      npc->rect = &rc[npc->anim];
    else
      npc->rect = &rc_damage[npc->anim];
  } else {
    npc->rect = &rc[npc->anim];
  }
}
