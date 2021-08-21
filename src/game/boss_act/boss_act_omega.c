#include "game/boss_act/boss_act.h"

static inline void boss_act_omega_sub0102(void) {
  int i;

  static const rect_t rc_left[1] = {
    {80, 56, 104, 72}
  };

  static const rect_t rc_right[1] = {
    {104, 56, 128, 72}
  };

  for (i = 1; i < 3; ++i) {
    npc_boss[i].y = (npc_boss[0].y + npc_boss[i + 2].y - (8 * 0x200)) / 2;

    if (npc_boss[i].dir == 0) {
      npc_boss[i].x = npc_boss[0].x - (16 * 0x200);
      npc_boss[i].rect = &rc_left[npc_boss[i].anim];
    } else {
      npc_boss[i].rect = &rc_right[npc_boss[i].anim];
      npc_boss[i].x = npc_boss[0].x + (16 * 0x200);
    }
  }
}

static inline void boss_act_omega_sub0304(void) {
  int i;

  static const rect_t rc_left[2] = {
    {0, 56, 40, 88},
    {40, 56, 80, 88},
  };

  static const rect_t rc_right[2] = {
    {0, 88, 40, 120},
    {40, 88, 80, 120},
  };

  for (i = 3; i < 5; ++i) {
    switch (npc_boss[i].act) {
      case 0:
        npc_boss[i].act = 1;
        // Fallthrough
      case 1:
        npc_boss[i].y = npc_boss[0].y;

        if (i == 3) npc_boss[i].x = npc_boss[0].x - (16 * 0x200);
        if (i == 4) npc_boss[i].x = npc_boss[0].x + (16 * 0x200);

        break;

      case 3:
        npc_boss[i].tgt_y = npc_boss[0].y + (24 * 0x200);

        if (i == 3) npc_boss[i].x = npc_boss[0].x - (16 * 0x200);
        if (i == 4) npc_boss[i].x = npc_boss[0].x + (16 * 0x200);

        npc_boss[i].y += (npc_boss[i].tgt_y - npc_boss[i].y) / 2;
        break;
    }

    if (npc_boss[i].flags & 8 || npc_boss[i].y <= npc_boss[i].tgt_y)
      npc_boss[i].anim = 0;
    else
      npc_boss[i].anim = 1;

    if (npc_boss[i].dir == 0)
      npc_boss[i].rect = &rc_left[npc_boss[i].anim];
    else
      npc_boss[i].rect = &rc_right[npc_boss[i].anim];
  }
}

static inline void boss_act_omega_sub05(void) {
  switch (npc_boss[5].act) {
    case 0:
      npc_boss[5].bits |= (NPC_SOLID_SOFT | NPC_IGNORE_SOLIDITY);

      npc_boss[5].hit.front = 20 * 0x200;
      npc_boss[5].hit.top = 36 * 0x200;
      npc_boss[5].hit.back = 20 * 0x200;
      npc_boss[5].hit.bottom = 16 * 0x200;

      npc_boss[5].act = 1;
      // Fallthrough
    case 1:
      npc_boss[5].x = npc_boss[0].x;
      npc_boss[5].y = npc_boss[0].y;
      break;
  }
}

void boss_act_omega(npc_t *root) {
  switch (npc_boss[0].act) {
    case 0:
      npc_boss[0].x = 219 * 0x10 * 0x200;
      npc_boss[0].y = 16 * 0x10 * 0x200;

      npc_boss[0].view.front = 40 * 0x200;
      npc_boss[0].view.top = 40 * 0x200;
      npc_boss[0].view.back = 40 * 0x200;
      npc_boss[0].view.bottom = 16 * 0x200;

      npc_boss[0].tgt_x = npc_boss[0].x;
      npc_boss[0].tgt_y = npc_boss[0].y;

      npc_boss[0].snd_hit = 52;

      npc_boss[0].hit.front = 8 * 0x200;
      npc_boss[0].hit.top = 24 * 0x200;
      npc_boss[0].hit.back = 8 * 0x200;
      npc_boss[0].hit.bottom = 16 * 0x200;

      npc_boss[0].bits = (NPC_IGNORE_SOLIDITY | NPC_EVENT_WHEN_KILLED | NPC_SHOW_DAMAGE);
      npc_boss[0].size = 3;
      npc_boss[0].exp = 1;
      npc_boss[0].event_num = 210;
      npc_boss[0].life = 400;

      npc_boss[1].cond = 0x80;

      npc_boss[1].view.front = 12 * 0x200;
      npc_boss[1].view.top = 8 * 0x200;
      npc_boss[1].view.back = 12 * 0x200;
      npc_boss[1].view.bottom = 8 * 0x200;

      npc_boss[1].bits = NPC_IGNORE_SOLIDITY;

      npc_boss[2] = npc_boss[1];

      npc_boss[1].dir = 0;
      npc_boss[2].dir = 2;

      npc_boss[3].cond = 0x80;

      npc_boss[3].view.front = 24 * 0x200;
      npc_boss[3].view.top = 16 * 0x200;
      npc_boss[3].view.back = 16 * 0x200;
      npc_boss[3].view.bottom = 16 * 0x200;

      npc_boss[3].snd_hit = 52;

      npc_boss[3].hit.front = 8 * 0x200;
      npc_boss[3].hit.top = 8 * 0x200;
      npc_boss[3].hit.back = 8 * 0x200;
      npc_boss[3].hit.bottom = 8 * 0x200;

      npc_boss[3].bits = NPC_IGNORE_SOLIDITY;

      npc_boss[3].x = npc_boss[0].x - (16 * 0x200);
      npc_boss[3].y = npc_boss[0].y;
      npc_boss[3].dir = 0;

      npc_boss[4] = npc_boss[3];

      npc_boss[4].dir = 2;
      npc_boss[3].x = npc_boss[0].x + (16 * 0x200);
      npc_boss[5].cond = 0x80;

      npc_boss_max = 5;
      for (int i = 0; i <= npc_boss_max; ++i)
        npc_boss[i].surf = SURFACE_ID_LEVEL_SPRITESET_2;
      break;

    case 20:  // Rising out of the ground
      npc_boss[0].act = 30;
      npc_boss[0].act_wait = 0;
      npc_boss[0].anim = 0;
      // Fallthrough
    case 30:
      cam_start_quake_small(2);
      npc_boss[0].y -= 1 * 0x200;

      if (++npc_boss[0].act_wait % 4 == 0) snd_play_sound(PRIO_NORMAL, 26, FALSE);

      if (npc_boss[0].act_wait == 48) {
        npc_boss[0].act_wait = 0;
        npc_boss[0].act = 40;

        if (npc_boss[0].life > 280) break;

        npc_boss[0].act = 110;

        npc_boss[0].bits |= NPC_SHOOTABLE;
        npc_boss[0].bits &= ~NPC_IGNORE_SOLIDITY;
        npc_boss[3].bits &= ~NPC_IGNORE_SOLIDITY;
        npc_boss[4].bits &= ~NPC_IGNORE_SOLIDITY;

        npc_boss[3].act = 3;
        npc_boss[4].act = 3;
        npc_boss[5].hit.top = 16 * 0x200;
      }

      break;

    case 40:
      ++npc_boss[0].act_wait;

      if (npc_boss[0].act_wait == 48) {
        npc_boss[0].act_wait = 0;
        npc_boss[0].act = 50;
        npc_boss[0].count1 = 0;
        npc_boss[5].hit.top = 16 * 0x200;
        snd_play_sound(PRIO_NORMAL, 102, FALSE);
      }

      break;

    case 50:  // Open mouth
      ++npc_boss[0].count1;

      if (npc_boss[0].count1 > 2) {
        npc_boss[0].count1 = 0;
        ++npc_boss[0].count2;
      }

      if (npc_boss[0].count2 == 3) {
        npc_boss[0].act = 60;
        npc_boss[0].act_wait = 0;
        npc_boss[0].bits |= NPC_SHOOTABLE;
        npc_boss[0].hit.front = 16 * 0x200;
        npc_boss[0].hit.back = 16 * 0x200;
      }

      break;

    case 60:  // Shoot out of mouth
      ++npc_boss[0].act_wait;

      if (npc_boss[0].act_wait > 20 && npc_boss[0].act_wait < 80 && !(npc_boss[0].act_wait % 3)) {
        if (m_rand(0, 9) < 8)
          npc_spawn(48, npc_boss[0].x, npc_boss[0].y - (16 * 0x200), m_rand(-0x100, 0x100), -0x10 * 0x200 / 10, 0, NULL, 0x100);
        else
          npc_spawn(48, npc_boss[0].x, npc_boss[0].y - (16 * 0x200), m_rand(-0x100, 0x100), -0x10 * 0x200 / 10, 2, NULL, 0x100);

        snd_play_sound(PRIO_NORMAL, 39, FALSE);
      }

      if (npc_boss[0].act_wait == 200 || bullet_count_by_arm(6)) {
        npc_boss[0].count1 = 0;
        npc_boss[0].act = 70;
        snd_play_sound(PRIO_NORMAL, 102, FALSE);
      }

      break;

    case 70:  // Close mouth
      ++npc_boss[0].count1;

      if (npc_boss[0].count1 > 2) {
        npc_boss[0].count1 = 0;
        --npc_boss[0].count2;
      }

      if (npc_boss[0].count2 == 1) npc_boss[0].damage = 20;

      if (npc_boss[0].count2 == 0) {
        snd_stop_sound(102);
        snd_play_sound(PRIO_NORMAL, 12, FALSE);

        npc_boss[0].act = 80;
        npc_boss[0].act_wait = 0;

        npc_boss[0].bits &= ~NPC_SHOOTABLE;

        npc_boss[0].hit.front = 24 * 0x200;
        npc_boss[0].hit.back = 24 * 0x200;
        npc_boss[5].hit.top = 36 * 0x200;

        npc_boss[0].damage = 0;
      }

      break;

    case 80:
      ++npc_boss[0].act_wait;

      if (npc_boss[0].act_wait == 48) {
        npc_boss[0].act_wait = 0;
        npc_boss[0].act = 90;
      }

      break;

    case 90:  // Go back into the ground
      cam_start_quake_small(2);
      npc_boss[0].y += 1 * 0x200;

      ++npc_boss[0].act_wait;

      if (npc_boss[0].act_wait % 4 == 0)
        snd_play_sound(PRIO_NORMAL, 26, FALSE);

      if (npc_boss[0].act_wait == 48) {
        npc_boss[0].act_wait = 0;
        npc_boss[0].act = 100;
      }

      break;

    case 100:  // Move to proper position for coming out of the ground
      ++npc_boss[0].act_wait;

      if (npc_boss[0].act_wait == 120) {
        npc_boss[0].act_wait = 0;
        npc_boss[0].act = 30;
        npc_boss[0].x = npc_boss[0].tgt_x + (m_rand(-64, 64) * 0x200);
        npc_boss[0].y = npc_boss[0].tgt_y;
      }

      break;

    case 110:
      ++npc_boss[0].count1;

      if (npc_boss[0].count1 > 2) {
        npc_boss[0].count1 = 0;
        ++npc_boss[0].count2;
      }

      if (npc_boss[0].count2 == 3) {
        npc_boss[0].act = 120;
        npc_boss[0].act_wait = 0;
        npc_boss[0].hit.front = 16 * 0x200;
        npc_boss[0].hit.back = 16 * 0x200;
      }

      break;

    case 120:
      ++npc_boss[0].act_wait;

      if (npc_boss[0].act_wait == 50 || bullet_count_by_arm(6)) {
        npc_boss[0].act = 130;
        snd_play_sound(PRIO_NORMAL, 102, FALSE);
        npc_boss[0].act_wait = 0;
        npc_boss[0].count1 = 0;
      }

      if (npc_boss[0].act_wait < 30 && npc_boss[0].act_wait % 5 == 0) {
        npc_spawn(48, npc_boss[0].x, npc_boss[0].y - (16 * 0x200), m_rand(-341, 341), -0x10 * 0x200 / 10, 0, NULL, 0x100);
        snd_play_sound(PRIO_NORMAL, 39, FALSE);
      }

      break;

    case 130:
      ++npc_boss[0].count1;

      if (npc_boss[0].count1 > 2) {
        npc_boss[0].count1 = 0;
        --npc_boss[0].count2;
      }

      if (npc_boss[0].count2 == 1) npc_boss[0].damage = 20;

      if (npc_boss[0].count2 == 0) {
        npc_boss[0].act = 140;
        npc_boss[0].bits |= NPC_SHOOTABLE;

        npc_boss[0].hit.front = 16 * 0x200;
        npc_boss[0].hit.back = 16 * 0x200;

        npc_boss[0].yvel = -0x5FF;

        snd_stop_sound(102);
        snd_play_sound(PRIO_NORMAL, 12, FALSE);
        snd_play_sound(PRIO_NORMAL, 25, FALSE);

        if (npc_boss[0].x < player.x) npc_boss[0].xvel = 0x100;
        if (npc_boss[0].x > player.x) npc_boss[0].xvel = -0x100;

        npc_boss[0].damage = 0;
        npc_boss[5].hit.top = 36 * 0x200;
      }

      break;

    case 140:
      if (player.flags & 8 && npc_boss[0].yvel > 0)
        npc_boss[5].damage = 20;
      else
        npc_boss[5].damage = 0;

      npc_boss[0].yvel += 0x24;
      if (npc_boss[0].yvel > 0x5FF) npc_boss[0].yvel = 0x5FF;

      npc_boss[0].x += npc_boss[0].xvel;
      npc_boss[0].y += npc_boss[0].yvel;

      if (npc_boss[0].flags & 8) {
        npc_boss[0].act = 110;
        npc_boss[0].act_wait = 0;
        npc_boss[0].count1 = 0;

        npc_boss[5].hit.top = 16 * 0x200;
        npc_boss[5].damage = 0;

        snd_play_sound(PRIO_NORMAL, 26, FALSE);
        snd_play_sound(PRIO_NORMAL, 12, FALSE);

        cam_start_quake_small(30);
      }

      break;

    case 150:
      cam_start_quake_small(2);

      ++npc_boss[0].act_wait;

      if (npc_boss[0].act_wait % 12 == 0) snd_play_sound(PRIO_NORMAL, 52, FALSE);

      npc_spawn_death_fx(npc_boss[0].x + (m_rand(-0x30, 0x30) * 0x200), npc_boss[0].y + (m_rand(-0x30, 0x18) * 0x200), 1, 1, 0);

      if (npc_boss[0].act_wait > 100) {
        npc_boss[0].act_wait = 0;
        npc_boss[0].act = 160;
        cam_start_flash(npc_boss[0].x, npc_boss[0].y, FLASH_MODE_EXPLOSION);
        snd_play_sound(PRIO_NORMAL, 35, FALSE);
      }

      break;

    case 160:
      cam_start_quake_small(40);

      ++npc_boss[0].act_wait;

      if (npc_boss[0].act_wait > 50) {
        npc_boss[0].cond = 0;
        npc_boss[1].cond = 0;
        npc_boss[2].cond = 0;
        npc_boss[3].cond = 0;
        npc_boss[4].cond = 0;
        npc_boss[5].cond = 0;
        npc_boss_max = -1;
      }

      break;
  }

  static const rect_t rect[4] = {
    {0, 0, 80, 56},
    {80, 0, 160, 56},
    {160, 0, 240, 56},
    {80, 0, 160, 56},
  };

  npc_boss[0].rect = &rect[npc_boss[0].count2];

  npc_boss[1].shock = npc_boss[0].shock;
  npc_boss[2].shock = npc_boss[0].shock;
  npc_boss[3].shock = npc_boss[0].shock;
  npc_boss[4].shock = npc_boss[0].shock;

  boss_act_omega_sub0304();
  boss_act_omega_sub0102();
  boss_act_omega_sub05();

  if (npc_boss[0].life == 0 && npc_boss[0].act < 150) {
    npc_boss[0].act = 150;
    npc_boss[0].act_wait = 0;
    npc_boss[0].damage = 0;
    npc_boss[5].damage = 0;
    npc_delete_by_class(48, TRUE);
  }
}
