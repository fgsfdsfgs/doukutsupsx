#include "game/boss_act/boss_act.h"

void boss_act_ironhead(npc_t *root) {
  int i;
  npc_t *npc = npc_boss;
  static u8 flash;

  switch (npc->act) {
    case 0:
      npc->cond = 0x80;
      npc->exp = 1;
      npc->dir = 2;
      npc->act = 100;
      npc->x = 160 * 0x200;
      npc->y = 128 * 0x200;
      npc->view.front = 40 * 0x200;
      npc->view.top = 12 * 0x200;
      npc->view.back = 24 * 0x200;
      npc->view.bottom = 12 * 0x200;
      npc->snd_hit = 54;
      npc->hit.front = 16 * 0x200;
      npc->hit.top = 10 * 0x200;
      npc->hit.back = 16 * 0x200;
      npc->hit.bottom = 10 * 0x200;
      npc->bits = (NPC_IGNORE_SOLIDITY | NPC_SHOOTABLE | NPC_EVENT_WHEN_KILLED | NPC_SHOW_DAMAGE);
      npc->size = 3;
      npc->damage = 10;
      npc->event_num = 1000;
      npc->life = 400;
      npc->surf = SURFACE_ID_LEVEL_SPRITESET_2;
      npc_boss_max = 0;
      break;

    case 100:
      npc->act = 101;
      npc->bits &= ~NPC_SHOOTABLE;
      npc->act_wait = 0;
      // Fallthrough
    case 101:
      ++npc->act_wait;

      if (npc->act_wait > 50) {
        npc->act = 250;
        npc->act_wait = 0;
      }

      if (npc->act_wait % 4 == 0)
        npc_spawn(197, m_rand(15, 18) * (16 * 0x200), m_rand(2, 13) * (16 * 0x200), 0, 0, 0, NULL, 0x100);

      break;

    case 250:
      npc->act = 251;

      if (npc->dir == 2) {
        npc->x = 240 * 0x200;
        npc->y = player.y;
      } else {
        npc->x = 720 * 0x200;
        npc->y = m_rand(2, 13) * (16 * 0x200);
      }

      npc->tgt_x = npc->x;
      npc->tgt_y = npc->y;

      npc->yvel = m_rand(-0x200, 0x200);
      npc->xvel = m_rand(-0x200, 0x200);

      npc->bits |= NPC_SHOOTABLE;
      // Fallthrough
    case 251:
      if (npc->dir == 2) {
        npc->tgt_x += 2 * 0x200;
      } else {
        npc->tgt_x -= 1 * 0x200;

        if (npc->tgt_y < player.y)
          npc->tgt_y += 1 * 0x200;
        else
          npc->tgt_y -= 1 * 0x200;
      }

      if (npc->x < npc->tgt_x)
        npc->xvel += 8;
      else
        npc->xvel -= 8;

      if (npc->y < npc->tgt_y)
        npc->yvel += 8;
      else
        npc->yvel -= 8;

      if (npc->yvel > 0x200) npc->yvel = 0x200;
      if (npc->yvel < -0x200) npc->yvel = -0x200;

      npc->x += npc->xvel;
      npc->y += npc->yvel;

      if (npc->dir == 2) {
        if (npc->x > 720 * 0x200) {
          npc->dir = 0;
          npc->act = 100;
        }
      } else {
        if (npc->x < 272 * 0x200) {
          npc->dir = 2;
          npc->act = 100;
        }
      }

      if (npc->dir == 0) {
        ++npc->act_wait;

        if (npc->act_wait == 300 || npc->act_wait == 310 || npc->act_wait == 320) {
          snd_play_sound(PRIO_NORMAL, 39, FALSE);
          npc_spawn(198, npc->x + (10 * 0x200), npc->y + (1 * 0x200), m_rand(-3, 0) * 0x200, m_rand(-3, 3) * 0x200, 2, NULL, 0x100);
        }
      }

      ++npc->anim_wait;

      if (npc->anim_wait > 2) {
        npc->anim_wait = 0;
        ++npc->anim;
      }

      if (npc->anim > 7) npc->anim = 0;

      break;

    case 1000:
      npc->bits &= ~NPC_SHOOTABLE;
      npc->anim = 8;
      npc->damage = 0;
      npc->act = 1001;
      npc->tgt_x = npc->x;
      npc->tgt_y = npc->y;
      cam_start_quake_small(20);

      for (i = 0; i < 0x20; ++i)
        npc_spawn(4, npc->x + (m_rand(-128, 128) * 0x200), npc->y + (m_rand(-64, 64) * 0x200),
          m_rand(-128, 128) * 0x200, m_rand(-128, 128) * 0x200, 0, NULL, 0x100);

      npc_delete_by_class(197, TRUE);
      npc_delete_by_class(271, TRUE);
      npc_delete_by_class(272, TRUE);
      // Fallthrough
    case 1001:
      npc->tgt_x -= 1 * 0x200;

      npc->x = npc->tgt_x + (m_rand(-1, 1) * 0x200);
      npc->y = npc->tgt_y + (m_rand(-1, 1) * 0x200);

      if (++npc->act_wait % 4 == 0)
        npc_spawn(4, npc->x + (m_rand(-128, 128) * 0x200), npc->y + (m_rand(-64, 64) * 0x200),
          m_rand(-128, 128) * 0x200, m_rand(-128, 128) * 0x200, 0, NULL, 0x100);

      break;
  }

  static const rect_t rc[9] = {
    {0, 0, 64, 24},    {64, 0, 128, 24},  {128, 0, 192, 24}, {64, 0, 128, 24},   {0, 0, 64, 24},
    {192, 0, 256, 24}, {256, 0, 320, 24}, {192, 0, 256, 24}, {256, 48, 320, 72},
  };

  static const rect_t rc_damage[9] = {
    {0, 24, 64, 48},    {64, 24, 128, 48},  {128, 24, 192, 48}, {64, 24, 128, 48},  {0, 24, 64, 48},
    {192, 24, 256, 48}, {256, 24, 320, 48}, {192, 24, 256, 48}, {256, 48, 320, 72},
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
