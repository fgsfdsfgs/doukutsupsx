#include "game/npc_act/npc_act.h"

// Sue (being teleported by Misery)
void npc_act_280(npc_t *npc) {
  static const rect_t rc_left[2] = {
    {112, 32, 128, 48},
    {144, 32, 160, 48},
  };

  static const rect_t rc_right[2] = {
    {112, 48, 128, 64},
    {144, 48, 160, 64},
  };

  switch (npc->act) {
    case 0:
      npc->act = 1;
      npc->anim = 0;
      npc->anim_wait = 0;
      npc->x += 6 * 0x200;
      npc->tgt_x = npc->x;
      snd_play_sound(PRIO_NORMAL, 29, FALSE);
      // Fallthrough
    case 1:
      if (++npc->act_wait == 64) {
        npc->act = 2;
        npc->act_wait = 0;
      }

      break;

    case 2:
      npc->anim = 0;

      if (npc->flags & 8) {
        npc->act = 4;
        npc->act_wait = 0;
        npc->anim = 1;
        snd_play_sound(PRIO_NORMAL, 23, FALSE);
      }

      break;
  }

  if (npc->act > 1) {
    npc->yvel += 0x20;
    if (npc->yvel > 0x5FF) npc->yvel = 0x5FF;

    npc->y += npc->yvel;
  }

  if (npc->dir == 0)
    npc->rect = &rc_left[npc->anim];
  else
    npc->rect = &rc_right[npc->anim];

  if (npc->act == 1) {
    npc->rect_delta.bottom = npc->rect->top + npc->act_wait / 4 - npc->rect->bottom;
    npc->rect_prev = NULL; // force texrect update
    if (npc->act_wait / 2 % 2)
      npc->x = npc->tgt_x;
    else
      npc->x = npc->tgt_x + (1 * 0x200);
  }
}

// Doctor (red energy form)
void npc_act_281(npc_t *npc) {
  static const rect_t rc = {0, 0, 0, 0};

  switch (npc->act) {
    case 0:
      npc->act = 1;
      break;

    case 10:
      npc->act = 11;
      npc->act_wait = 0;
      // Fallthrough
    case 11:
      ++npc->act_wait;

      npc_spawn(270, npc->x, npc->y + (128 * 0x200), 0, 0, 2, npc, 0x100);

      if (npc->act_wait > 150) npc->act = 12;

      break;

    case 20:
      npc->act = 21;
      npc->act_wait = 0;
      // Fallthrough
    case 21:
      if (++npc->act_wait > 250) {
        npc_delete_by_class(270, FALSE);
        npc->act = 22;
      }

      break;
  }

  npc->rect = &rc;
}

// Mini Undead Core (active)
void npc_act_282(npc_t *npc) {
  // Yes, Pixel spelt this wrong (should be 'rc')
  static const rect_t tc[3] = {
    {256, 80, 320, 120},
    {256, 0, 320, 40},
    {256, 120, 320, 160},
  };

  switch (npc->act) {
    case 0:
      npc->act = 20;
      npc->tgt_y = npc->y;

      if (m_rand(0, 100) % 2)
        npc->yvel = -0x100;
      else
        npc->yvel = 0x100;
      // Fallthrough
    case 20:
      npc->xvel = -0x200;

      if (npc->x < -64 * 0x200) npc->cond = 0;

      if (npc->tgt_y < npc->y) npc->yvel -= 0x10;
      if (npc->tgt_y > npc->y) npc->yvel += 0x10;

      if (npc->yvel > 0x100) npc->yvel = 0x100;
      if (npc->yvel < -0x100) npc->yvel = -0x100;

      if (player.flags & 8 && player.y < npc->y - (4 * 0x200) && player.x > npc->x - (24 * 0x200) &&
          player.x < npc->x + (24 * 0x200)) {
        npc->tgt_y = 144 * 0x200;
        npc->anim = 2;
      } else if (npc->anim != 1) {
        npc->anim = 0;
      }

      if (player.flags & 1 && player.x < npc->x - npc->hit.back && player.x > npc->x - npc->hit.back - (8 * 0x200) &&
          player.y + player.hit.bottom > npc->y - npc->hit.top &&
          player.y - player.hit.top < npc->y + npc->hit.bottom) {
        npc->bits &= ~NPC_SOLID_HARD;
        npc->anim = 1;
      } else if (player.flags & 4 && player.x > npc->x + npc->hit.back &&
                 player.x < npc->x + npc->hit.back + (8 * 0x200) &&
                 player.y + player.hit.bottom > npc->y - npc->hit.top &&
                 player.y - player.hit.top < npc->y + npc->hit.bottom) {
        npc->bits &= ~NPC_SOLID_HARD;
        npc->anim = 1;
      } else if (player.flags & 2 && player.y < npc->y - npc->hit.top &&
                 player.y > npc->y - npc->hit.top - (8 * 0x200) &&
                 player.x + player.hit.front > npc->x - npc->hit.back &&
                 player.x - player.hit.back < npc->x + npc->hit.front) {
        npc->bits &= ~NPC_SOLID_HARD;
        npc->anim = 1;
      } else if (player.flags & 8 && player.y > npc->y + npc->hit.bottom - (4 * 0x200) &&
                 player.y < npc->y + npc->hit.bottom + (12 * 0x200) &&
                 player.x + player.hit.front > npc->x - npc->hit.back - (4 * 0x200) &&
                 player.x - player.hit.back < npc->x + npc->hit.front + (4 * 0x200)) {
        npc->bits &= ~NPC_SOLID_HARD;
        npc->anim = 1;
      }
  }

  npc->x += npc->xvel;
  npc->y += npc->yvel;

  npc->rect = &tc[npc->anim];
}

// Misery (transformed)
void npc_act_283(npc_t *npc) {
  int x, y, dir;

  static const rect_t rc_left[11] = {
    {0, 64, 32, 96},    {32, 64, 64, 96},   {64, 64, 96, 96},   {96, 64, 128, 96},
    {128, 64, 160, 96}, {160, 64, 192, 96}, {192, 64, 224, 96}, {224, 64, 256, 96},
    {0, 0, 0, 0},       {256, 64, 288, 96}, {288, 64, 320, 96},
  };

  static const rect_t rc_right[11] = {
    {0, 96, 32, 128},    {32, 96, 64, 128},   {64, 96, 96, 128},   {96, 96, 128, 128},
    {128, 96, 160, 128}, {160, 96, 192, 128}, {192, 96, 224, 128}, {224, 96, 256, 128},
    {0, 0, 0, 0},        {256, 96, 288, 128}, {288, 96, 320, 128},
  };

  if (npc->act < 100 && (npc_boss[0].cond == 0 || npc->life < 400)) npc->act = 100;

  switch (npc->act) {
    case 0:
      npc->act = 1;
      npc->y -= 8 * 0x200;
      snd_play_sound(PRIO_NORMAL, 29, FALSE);
      // Fallthrough
    case 1:
      if (++npc->act_wait / 2 % 2)
        npc->anim = 9;
      else
        npc->anim = 0;

      break;

    case 10:
      npc->act = 11;
      npc->anim = 9;
      break;

    case 20:
      npc_doctor_state.crystal_x = 0;
      npc->act = 21;
      npc->act_wait = 0;
      npc->anim = 0;
      npc->anim_wait = 0;
      // Fallthrough
    case 21:
      npc->xvel = 7 * npc->xvel / 8;
      npc->yvel = 7 * npc->yvel / 8;

      if (++npc->anim_wait > 20) {
        npc->anim_wait = 0;
        ++npc->anim;
      }

      if (npc->anim > 1) npc->anim = 0;

      if (++npc->act_wait > 100) npc->act = 30;

      if (npc->x < player.x)
        npc->dir = 2;
      else
        npc->dir = 0;

      break;

    case 30:
      npc->act = 31;
      npc->act_wait = 0;
      npc->anim = 2;
      npc->count2 = npc->life;
      // Fallthrough
    case 31:
      if (++npc->anim_wait > 1) {
        npc->anim_wait = 0;
        ++npc->anim;
      }

      if (npc->anim > 3) npc->anim = 2;

      if (npc->flags & 8) npc->yvel = -0x200;

      if (npc->x > npc_boss[0].x)
        npc->xvel -= 0x20;
      else
        npc->xvel += 0x20;

      if (npc->y > player.y)
        npc->yvel -= 0x10;
      else
        npc->yvel += 0x10;

      if (npc->xvel > 0x200) npc->xvel = 0x200;
      if (npc->xvel < -0x200) npc->xvel = -0x200;

      if (npc->yvel > 0x200) npc->yvel = 0x200;
      if (npc->yvel < -0x200) npc->yvel = -0x200;

      if (npc->x > player.x)
        npc->dir = 0;
      else
        npc->dir = 2;

      if (++npc->act_wait > 150 && (npc->life < npc->count2 - 20 || npc_doctor_state.crystal_x)) {
        npc_doctor_state.crystal_x = 0;
        npc->act = 40;
      }

      if (npc_boss[0].anim && npc->act_wait > 250) npc->act = 50;

      break;

    case 40:
      npc->act = 41;
      npc->act_wait = 0;
      npc->xvel = 0;
      npc->yvel = 0;

      if (npc->x > player.x)
        npc->dir = 0;
      else
        npc->dir = 2;

      snd_play_sound(PRIO_NORMAL, 103, FALSE);

      if (player.y < 160 * 0x200)
        npc->count2 = 290;
      else
        npc->count2 = 289;
      // Fallthrough
    case 41:
      if (++npc->act_wait / 2 % 2)
        npc->anim = 4;
      else
        npc->anim = 5;

      if (npc->act_wait % 6 == 1) {
        if (npc->count2 == 289) {
          x = npc->x + (m_rand(-0x40, 0x40) * 0x200);
          y = npc->y + (m_rand(-0x20, 0x20) * 0x200);
        } else {
          x = npc->x + (m_rand(-0x20, 0x20) * 0x200);
          y = npc->y + (m_rand(-0x40, 0x40) * 0x200);
        }

        if (x < 32 * 0x200) x = 32 * 0x200;
        if (x > (stage_data->width - 2) * 0x200 * 0x10) x = (stage_data->width - 2) * 0x200 * 0x10;

        if (y < 32 * 0x200) y = 32 * 0x200;
        if (y > (stage_data->height - 2) * 0x200 * 0x10) y = (stage_data->height - 2) * 0x200 * 0x10;

        snd_play_sound(PRIO_NORMAL, 39, FALSE);
        npc_spawn(npc->count2, x, y, 0, 0, 0, NULL, 0x100);
      }

      if (npc->act_wait > 50) {
        npc->act = 42;
        npc->act_wait = 0;

        if (npc->x > player.x)
          npc->dir = 0;
        else
          npc->dir = 2;
      }

      break;

    case 42:
      ++npc->act_wait;
      npc->anim = 6;

      if (npc->act_wait > 50) {
        npc->yvel = -0x200;

        if (npc->dir == 0)
          npc->xvel = 0x200;
        else
          npc->xvel = -0x200;

        npc->act = 30;
      }

      break;

    case 50:
      npc->act = 51;
      npc->act_wait = 0;
      npc->xvel = 0;
      npc->yvel = 0;

      if (npc->x > player.x)
        npc->dir = 0;
      else
        npc->dir = 2;

      snd_play_sound(PRIO_NORMAL, 103, FALSE);
      // Fallthrough
    case 51:
      if (++npc->act_wait / 2 % 2)
        npc->anim = 4;
      else
        npc->anim = 5;

      if (player.equip & EQUIP_BOOSTER_2_0) {
        if (npc->act_wait % 10 == 1) {
          if (npc->dir == 0) {
            x = npc->x + (10 * 0x200);
            y = npc->y;

            switch (npc->act_wait / 6 % 4) {
              case 0:
                dir = 0xD8;
                break;

              case 1:
                dir = 0xEC;
                break;

              case 2:
                dir = 0x14;
                break;

              default:
                dir = 0x28;
                break;
            }
          } else {
            x = npc->x - (10 * 0x200);
            y = npc->y;

            switch (npc->act_wait / 6 % 4) {
              case 0:
                dir = 0x58;
                break;

              case 1:
                dir = 0x6C;
                break;

              case 2:
                dir = 0x94;
                break;

              default:
                dir = 0xA8;
                break;
            }
          }

          snd_play_sound(PRIO_NORMAL, 39, FALSE);
          npc_spawn(301, x, y, 0, 0, dir, NULL, 0x100);
        }
      } else if (npc->act_wait % 24 == 1) {
        if (npc->dir == 0) {
          x = npc->x + (10 * 0x200);
          y = npc->y;

          switch (npc->act_wait / 6 % 4) {
            case 0:
              dir = 0xD8;
              break;

            case 1:
              dir = 0xEC;
              break;

            case 2:
              dir = 0x14;
              break;

            default:
              dir = 0x28;
              break;
          }
        } else {
          x = npc->x - (10 * 0x200);
          y = npc->y;

          switch (npc->act_wait / 6 % 4) {
            case 0:
              dir = 0x58;
              break;

            case 1:
              dir = 0x6C;
              break;

            case 2:
              dir = 0x94;
              break;

            default:
              dir = 0xA8;
              break;
          }
        }

        snd_play_sound(PRIO_NORMAL, 39, FALSE);
        npc_spawn(301, x, y, 0, 0, dir, NULL, 0x100);
      }

      if (npc->act_wait > 50) {
        npc->act = 42;
        npc->act_wait = 0;

        if (npc->x > player.x)
          npc->dir = 0;
        else
          npc->dir = 2;
      }

      break;

    case 99:
      npc->xvel = 0;
      npc->yvel = 0;
      npc->anim = 9;
      npc->bits &= ~NPC_SHOOTABLE;
      break;

    case 100:
      npc->act = 101;
      npc->anim = 9;
      npc->damage = 0;
      npc->bits &= ~NPC_SHOOTABLE;
      npc->bits |= NPC_IGNORE_SOLIDITY;
      npc->yvel = -0x200;
      npc->shock += 50;
      npc->hit.bottom = 12 * 0x200;
      ++npc_boss[0].anim;
      // Fallthrough
    case 101:
      npc->yvel += 0x20;

      if (npc->y > (216 * 0x200) - npc->hit.bottom) {
        npc->y = (216 * 0x200) - npc->hit.bottom;
        npc->act = 102;
        npc->anim = 10;
        npc->xvel = 0;
        npc->yvel = 0;
      }

      break;
  }

  npc->y += npc->yvel;

  if (npc->shock)
    npc->x += npc->xvel / 2;
  else
    npc->x += npc->xvel;

  if (npc->dir == 0)
    npc->rect = &rc_left[npc->anim];
  else
    npc->rect = &rc_right[npc->anim];
}

// Sue (transformed)
void npc_act_284(npc_t *npc) {
  u8 deg;

  static const rect_t rc_left[13] = {
    {0, 128, 32, 160},    {32, 128, 64, 160},   {64, 128, 96, 160},   {96, 128, 128, 160}, {128, 128, 160, 160},
    {160, 128, 192, 160}, {192, 128, 224, 160}, {224, 128, 256, 160}, {0, 0, 0, 0},        {256, 128, 288, 160},
    {288, 128, 320, 160}, {224, 64, 256, 96},   {208, 32, 224, 48},
  };

  static const rect_t rc_right[13] = {
    {0, 160, 32, 192},    {32, 160, 64, 192},   {64, 160, 96, 192},   {96, 160, 128, 192}, {128, 160, 160, 192},
    {160, 160, 192, 192}, {192, 160, 224, 192}, {224, 160, 256, 192}, {0, 0, 0, 0},        {256, 160, 288, 192},
    {288, 160, 320, 192}, {224, 96, 256, 128},  {208, 48, 224, 64},
  };

  if (npc->act < 100 && (npc_boss[0].cond == 0 || npc->life < 500)) npc->act = 100;

  switch (npc->act) {
    case 0:
      npc->act = 1;
      npc->y -= 4 * 0x200;
      snd_play_sound(PRIO_NORMAL, 29, FALSE);
      npc->count2 = npc->life;
      // Fallthrough
    case 1:
      if (++npc->act_wait / 2 % 2) {
        npc->view.top = 16 * 0x200;
        npc->view.back = 16 * 0x200;
        npc->view.front = 16 * 0x200;
        npc->anim = 11;
      } else {
        npc->view.top = 3 * 0x200;
        npc->view.back = 8 * 0x200;
        npc->view.front = 8 * 0x200;
        npc->anim = 12;
      }

      if (npc->act_wait > 50) npc->act = 10;

      break;

    case 10:
      npc->act = 11;
      npc->anim = 11;
      npc->view.top = 16 * 0x200;
      npc->view.back = 16 * 0x200;
      npc->view.front = 16 * 0x200;
      npc_delete_by_class(257, TRUE);
      break;

    case 20:
      npc->act = 21;
      npc->act_wait = 0;
      npc->anim = 0;
      npc->anim_wait = 0;
      npc->damage = 0;
      npc->bits |= NPC_SHOOTABLE;
      npc->bits &= ~NPC_IGNORE_SOLIDITY;
      // Fallthrough
    case 21:
      npc->xvel = (npc->xvel * 7) / 8;
      npc->yvel = (npc->yvel * 7) / 8;

      if (++npc->anim_wait > 20) {
        npc->anim_wait = 0;
        ++npc->anim;
      }

      if (npc->anim > 1) npc->anim = 0;

      if (++npc->act_wait > 80) npc->act = 30;

      if (npc->x < player.x)
        npc->dir = 2;
      else
        npc->dir = 0;

      if (npc->life < npc->count2 - 50) {
        npc->count2 = npc->life;
        npc_doctor_state.crystal_x = 10;
      }

      break;

    case 30:
      npc->act = 31;
      npc->act_wait = 0;
      npc->anim = 2;
      npc->xvel = 0;
      npc->yvel = 0;
      // Fallthrough
    case 31:
      if (++npc->act_wait > 16) {
        ++npc->count1;
        npc->count1 %= 4;

        switch (npc->count1) {
          case 1:
          case 3:
            npc->act = 34;
            break;

          case 0:  // Identical to case 2
            npc->act = 32;
            break;

          case 2:  // Identical to case 0
            npc->act = 32;
            break;
        }
      }

      break;

    case 32:
      npc->act = 33;
      npc->act_wait = 0;
      npc->bits &= ~NPC_SHOOTABLE;

      if (player.x < npc->x)
        npc->tgt_x = player.x - (160 * 0x200);
      else
        npc->tgt_x = player.x + (160 * 0x200);

      npc->tgt_y = player.y;

      deg = m_atan2(npc->x - npc->tgt_x, npc->y - npc->tgt_y);
      npc->xvel = 3 * m_cos(deg);
      npc->yvel = 3 * m_sin(deg);
      npc->bits &= ~NPC_IGNORE_SOLIDITY;

      if (npc->x < (stage_data->width * 0x200 * 0x10) / 2 && npc->xvel > 0) {
        if (npc->y < (stage_data->height * 0x200 * 0x10) / 2 && npc->yvel > 0) npc->bits |= NPC_IGNORE_SOLIDITY;
        if (npc->y > (stage_data->height * 0x200 * 0x10) / 2 && npc->yvel < 0) npc->bits |= NPC_IGNORE_SOLIDITY;
      }
      if (npc->x > (stage_data->width * 0x200 * 0x10) / 2 && npc->xvel < 0) {
        if (npc->y < (stage_data->height * 0x200 * 0x10) / 2 && npc->yvel > 0) npc->bits |= NPC_IGNORE_SOLIDITY;
        if (npc->y > (stage_data->height * 0x200 * 0x10) / 2 && npc->yvel < 0) npc->bits |= NPC_IGNORE_SOLIDITY;
      }

      if (npc->xvel > 0)
        npc->dir = 2;
      else
        npc->dir = 0;
      // Fallthrough
    case 33:
      if (++npc->act_wait / 2 % 2)
        npc->anim = 3;
      else
        npc->anim = 8;

      if (npc->act_wait > 50 || npc->flags & 5) npc->act = 20;

      break;

    case 34:
      npc->act = 35;
      npc->act_wait = 0;
      npc->damage = 4;

      npc->tgt_x = player.x;
      npc->tgt_y = player.y;

      deg = m_atan2(npc->x - npc->tgt_x, npc->y - npc->tgt_y);
      npc->yvel = 3 * m_sin(deg);
      npc->xvel = 3 * m_cos(deg);
      npc->bits &= ~NPC_IGNORE_SOLIDITY;

      if (npc->x < (stage_data->width * 0x200 * 0x10) / 2 && npc->xvel > 0) {
        if (npc->y < (stage_data->height * 0x200 * 0x10) / 2 && npc->yvel > 0) npc->bits |= NPC_IGNORE_SOLIDITY;
        if (npc->y > (stage_data->height * 0x200 * 0x10) / 2 && npc->yvel < 0) npc->bits |= NPC_IGNORE_SOLIDITY;
      }

      if (npc->x > (stage_data->width * 0x200 * 0x10) / 2 && npc->xvel < 0) {
        if (npc->y < (stage_data->height * 0x200 * 0x10) / 2 && npc->yvel > 0) npc->bits |= NPC_IGNORE_SOLIDITY;
        if (npc->y > (stage_data->height * 0x200 * 0x10) / 2 && npc->yvel < 0) npc->bits |= NPC_IGNORE_SOLIDITY;
      }

      if (npc->xvel > 0)
        npc->dir = 2;
      else
        npc->dir = 0;
      // Fallthrough
    case 35:
      if (++npc->act_wait > 20 && npc->shock)
        npc->act = 40;
      else if (npc->act_wait > 50 || npc->flags & 5)
        npc->act = 20;

      if (++npc->anim_wait > 1) {
        npc->anim_wait = 0;
        ++npc->anim;
      }

      if (npc->anim > 7) npc->anim = 4;

      if (npc->act_wait % 5 == 1) snd_play_sound(PRIO_NORMAL, 109, FALSE);

      break;

    case 40:
      npc->act = 41;
      npc->act_wait = 0;
      npc->anim = 2;
      npc->damage = 0;
      npc->bits &= ~NPC_IGNORE_SOLIDITY;
      // Fallthrough
    case 41:
      npc->xvel = (npc->xvel * 7) / 8;
      npc->yvel = (npc->yvel * 7) / 8;

      if (++npc->act_wait > 6) {
        npc->act = 42;
        npc->act_wait = 0;
        npc->yvel = -0x200;

        if (npc->dir == 0)
          npc->xvel = 0x200;
        else
          npc->xvel = -0x200;
      }

      break;

    case 42:
      npc->anim = 9;

      if (npc->flags & 8) {
        npc->act = 43;
        npc->act_wait = 0;
        npc->anim = 2;

        if (npc->x < player.x)
          npc->dir = 2;
        else
          npc->dir = 0;
      }

      npc->yvel += 0x20;
      if (npc->yvel > 0x5FF) npc->yvel = 0x5FF;

      break;

    case 43:
      if (++npc->act_wait > 16) npc->act = 20;

      break;

    case 99:
      npc->xvel = 0;
      npc->yvel = 0;
      npc->anim = 9;
      npc->bits &= ~NPC_SHOOTABLE;
      break;

    case 100:
      npc->act = 101;
      npc->anim = 9;
      npc->damage = 0;
      npc->bits &= ~NPC_SHOOTABLE;
      npc->bits |= NPC_IGNORE_SOLIDITY;
      npc->yvel = -0x200;
      npc->shock += 50;
      ++npc_boss[0].anim;
      // Fallthrough
    case 101:
      npc->yvel += 0x20;

      if (npc->y > (216 * 0x200) - npc->hit.bottom) {
        npc->y = (216 * 0x200) - npc->hit.bottom;
        npc->act = 102;
        npc->anim = 10;
        npc->xvel = 0;
        npc->yvel = 0;
      }

      break;
  }

  npc->y += npc->yvel;

  if (npc->shock)
    npc->x += npc->xvel / 2;
  else
    npc->x += npc->xvel;

  if (npc->dir == 0)
    npc->rect = &rc_left[npc->anim];
  else
    npc->rect = &rc_right[npc->anim];
}

// Undead Core spiral projectile
void npc_act_285(npc_t *npc) {
  static const rect_t rc = {232, 104, 248, 120};
  u8 deg;

  if (npc->x < 0 || npc->x > stage_data->width * 0x10 * 0x200) {
    npc_show_death_damage(npc);
    return;
  }

  switch (npc->act) {
    case 0:
      npc->act = 1;
      npc->tgt_x = npc->x;
      npc->tgt_y = npc->y;
      npc->count1 = npc->dir / 8;
      npc->dir %= 8;
      // Fallthrough
    case 1:
      npc->count1 += 24;
      npc->count1 %= 0x100;

      deg = npc->count1;

      if (npc->act_wait < 128) ++npc->act_wait;

      if (npc->dir == 0)
        npc->xvel -= 21;
      else
        npc->xvel += 21;

      npc->tgt_x += npc->xvel;

      npc->x = npc->tgt_x + (m_cos(deg) * 4);
      npc->y = npc->tgt_y + (m_sin(deg) * 6);

      npc_spawn(286, npc->x, npc->y, 0, 0, 0, NULL, 0x100);

      break;
  }

  npc->rect = &rc;
}

// Undead Core spiral shot trail
void npc_act_286(npc_t *npc) {
  static const rect_t rc[3] = {
    {232, 120, 248, 136},
    {232, 136, 248, 152},
    {232, 152, 248, 168},
  };

  if (++npc->anim_wait > 0) {
    npc->anim_wait = 0;
    ++npc->anim;
  }

  if (npc->anim > 2)
    npc->cond = 0;
  else
    npc->rect = &rc[npc->anim];
}

// Orange smoke
void npc_act_287(npc_t *npc) {
  static const rect_t rc_left[7] = {
    {0, 224, 16, 240},  {16, 224, 32, 240}, {32, 224, 48, 240},  {48, 224, 64, 240},
    {64, 224, 80, 240}, {80, 224, 96, 240}, {96, 224, 112, 240},
  };

  if (npc->act == 0) {
    npc->xvel = m_rand(-4, 4) * 0x200;
    npc->act = 1;
  } else {
    npc->xvel = (npc->xvel * 20) / 21;
    npc->yvel = (npc->yvel * 20) / 21;

    npc->x += npc->xvel;
    npc->y += npc->yvel;
  }

  if (++npc->anim_wait > 1) {
    npc->anim_wait = 0;
    ++npc->anim;
  }

  if (npc->anim > 6)
    npc->cond = 0;
  else
    npc->rect = &rc_left[npc->anim];
}

// Undead Core exploding rock
void npc_act_288(npc_t *npc) {
  static const rect_t rc[5] = {
    {232, 72, 248, 88}, {232, 88, 248, 104}, {232, 0, 256, 24},
    {232, 24, 256, 48}, {232, 48, 256, 72},
  };

  switch (npc->act) {
    case 0:
      npc->act = 1;
      npc->xvel = -0x200;
      // Fallthrough
    case 1:
      if (npc->dir == 1) {
        npc->yvel -= 0x20;
        if (npc->yvel < -0x5FF) npc->yvel = -0x5FF;

        if (npc->flags & 2) npc->act = 2;
      } else if (npc->dir == 3) {
        npc->yvel += 0x20;
        if (npc->yvel > 0x5FF) npc->yvel = 0x5FF;

        if (npc->flags & 8) npc->act = 2;
      }

      if (++npc->anim_wait > 3) {
        npc->anim_wait = 0;
        ++npc->anim;
      }

      if (npc->anim > 1) npc->anim = 0;

      break;

    case 2:
      snd_play_sound(PRIO_NORMAL, 44, FALSE);
      npc->act = 3;
      npc->act_wait = 0;
      npc->bits |= NPC_IGNORE_SOLIDITY;
      npc->yvel = 0;

      if (npc->x > player.x)
        npc->xvel = -0x400;
      else
        npc->xvel = 0x400;

      npc->view.back = 12 * 0x200;
      npc->view.front = 12 * 0x200;
      npc->view.top = 12 * 0x200;
      npc->view.bottom = 12 * 0x200;
      // Fallthrough
    case 3:
      if (++npc->anim > 4) npc->anim = 2;

      if (++npc->act_wait % 4 == 1) {
        if (npc->dir == 1)
          npc_spawn(287, npc->x, npc->y, 0, 0x400, 0, NULL, 0x100);
        else
          npc_spawn(287, npc->x, npc->y, 0, -0x400, 0, NULL, 0x100);
      }

      if (npc->x < 1 * 0x200 * 0x10 || npc->x > (stage_data->width * 0x200 * 0x10) - (1 * 0x200 * 0x10)) npc->cond = 0;

      break;
  }

  npc->x += npc->xvel;
  npc->y += npc->yvel;

  npc->rect = &rc[npc->anim];
}

// Critter (orange, Misery)
void npc_act_289(npc_t *npc) {
  static const rect_t rc_left[3] = {
    {160, 32, 176, 48},
    {176, 32, 192, 48},
    {192, 32, 208, 48},
  };

  static const rect_t rc_right[3] = {
    {160, 48, 176, 64},
    {176, 48, 192, 64},
    {192, 48, 208, 64},
  };

  switch (npc->act) {
    case 0:
      npc->act = 1;
      npc->anim = 2;

      if (npc->x > player.x)
        npc->dir = 0;
      else
        npc->dir = 2;
      // Fallthrough
    case 1:
      if (++npc->act_wait > 16) {
        npc->act = 10;
        npc->view.top = 8 * 0x200;
        npc->view.bottom = 8 * 0x200;
        npc->damage = 2;
        npc->bits |= NPC_SHOOTABLE;
      }

      break;

    case 10:
      if (npc->flags & 8) {
        npc->act = 11;
        npc->anim = 0;
        npc->act_wait = 0;
        npc->xvel = 0;

        if (npc->x > player.x)
          npc->dir = 0;
        else
          npc->dir = 2;
      }

      break;

    case 11:
      if (++npc->act_wait > 10) {
        if (++npc->count1 > 4)
          npc->act = 12;
        else
          npc->act = 10;

        snd_play_sound(PRIO_NORMAL, 30, FALSE);
        npc->yvel = -0x600;

        if (npc->dir == 0)
          npc->xvel = -0x200;
        else
          npc->xvel = 0x200;

        npc->anim = 2;
      }

      break;

    case 12:
      npc->bits |= NPC_IGNORE_SOLIDITY;

      if (npc->y > stage_data->height * 0x200 * 0x10) {
        npc_show_death_damage(npc);
        return;
      }

      break;
  }

  if (npc->act >= 10) npc->yvel += 0x40;

  if (npc->yvel > 0x5FF) npc->yvel = 0x5FF;

  npc->x += npc->xvel;
  npc->y += npc->yvel;

  if (npc->dir == 0)
    npc->rect = &rc_left[npc->anim];
  else
    npc->rect = &rc_right[npc->anim];

  if (npc->act == 1) {
    npc->rect_delta.top = 8 - (npc->act_wait / 2);
    npc->rect_delta.bottom = -8 - (npc->act_wait / 2);
    npc->rect_prev = NULL; // force texrect update
    npc->view.top = (npc->act_wait * 0x200) / 2;
    npc->view.bottom = (npc->act_wait * 0x200) / 2;
  }
}

// Bat (Misery)
void npc_act_290(npc_t *npc) {
  static const rect_t rc_left[3] = {
    {112, 32, 128, 48},
    {128, 32, 144, 48},
    {144, 32, 160, 48},
  };

  static const rect_t rc_right[3] = {
    {112, 48, 128, 64},
    {128, 48, 144, 64},
    {144, 48, 160, 64},
  };

  switch (npc->act) {
    case 0:
      npc->act = 1;
      npc->anim = 2;

      if (npc->x > player.x)
        npc->dir = 0;
      else
        npc->dir = 2;
      // Fallthrough
    case 1:
      if (++npc->act_wait > 16) {
        npc->act = 10;
        npc->view.top = 8 * 0x200;
        npc->view.bottom = 8 * 0x200;
        npc->damage = 2;
        npc->bits |= NPC_SHOOTABLE;
        npc->tgt_y = npc->y;
        npc->yvel = 0x400;
      }

      break;

    case 10:
      if (++npc->anim_wait > 2) {
        npc->anim_wait = 0;
        ++npc->anim;
      }

      if (npc->anim > 2) npc->anim = 0;

      if (npc->y < npc->tgt_y)
        npc->yvel += 0x40;
      else
        npc->yvel -= 0x40;

      if (npc->dir == 0)
        npc->xvel -= 0x10;
      else
        npc->xvel += 0x10;

      if (npc->x < 0 || npc->y < 0 || npc->x > stage_data->width * 0x200 * 0x10 ||
          npc->y > stage_data->height * 0x200 * 0x10) {
        npc_show_death_damage(npc);
        return;
      }

      break;
  }

  npc->x += npc->xvel;
  npc->y += npc->yvel;

  if (npc->dir == 0)
    npc->rect = &rc_left[npc->anim];
  else
    npc->rect = &rc_right[npc->anim];

  if (npc->act == 1) {
    npc->rect_delta.top = 8 - (npc->act_wait / 2);
    npc->rect_delta.bottom = -8 - (npc->act_wait / 2);
    npc->rect_prev = NULL; // force texrect update
    npc->view.top = (npc->act_wait * 0x200) / 2;
    npc->view.bottom = (npc->act_wait * 0x200) / 2;
  }
}

// Mini Undead Core (inactive)
void npc_act_291(npc_t *npc) {
  static const rect_t tc[2] = {
    {256, 80, 320, 120},
    {256, 0, 320, 40},
  };

  switch (npc->act) {
    case 0:
      npc->act = 20;

      if (npc->dir == 2) {
        npc->bits &= ~NPC_SOLID_HARD;
        npc->anim = 1;
      }

      break;
  }

  npc->rect = &tc[npc->anim];
}

// Quake
void npc_act_292(npc_t *npc) {
  (void)npc;

  cam_start_quake_small(10);
}

// Undead Core giant energy shot
void npc_act_293(npc_t *npc) {
  static const rect_t rect[2] = {
    {240, 200, 280, 240},
    {280, 200, 320, 240},
  };

  switch (npc->act) {
    case 0:
      npc->act = 1;
      // Fallthrough
    case 1:
      if (++npc->anim > 1) npc->anim = 0;

      npc_spawn(4, npc->x + (m_rand(0, 0x10) * 0x200), npc->y + (m_rand(-0x10, 0x10) * 0x200), 0, 0, 0, NULL, 0x100);

      npc->x -= 8 * 0x200;

      if (npc->x < -32 * 0x200) npc->cond = 0;

      break;
  }

  npc->rect = &rect[npc->anim];
}

// Quake + falling block generator
void npc_act_294(npc_t *npc) {
  int x, y, dir;

  switch (npc->act) {
    case 0:
      if (player.x < (stage_data->width - 6) * 0x200 * 0x10) {
        npc->act = 1;
        npc->act_wait = 0;
      }

      break;

    case 1:
      ++npc->act_wait;

      if (player.equip & EQUIP_BOOSTER_2_0) {
        npc->x = player.x + (64 * 0x200);
        if (npc->x < 416 * 0x200) npc->x = 416 * 0x200;
      } else {
        npc->x = player.x + (96 * 0x200);
        if (npc->x < 368 * 0x200) npc->x = 368 * 0x200;
      }

      if (npc->x > (stage_data->width - 10) * 0x200 * 0x10)
        npc->x = (stage_data->width - 10) * 0x200 * 0x10;

      if (npc->act_wait > 24) {
        if (player.equip & EQUIP_BOOSTER_2_0)
          x = npc->x + (m_rand(-14, 14) * 0x200 * 0x10);
        else
          x = npc->x + (m_rand(-11, 11) * 0x200 * 0x10);

        y = player.y - (224 * 0x200);

        if (m_rand(0, 10) % 2)  // Because just doing 'm_rand(0, 1)' is too hard
          dir = 0;
        else
          dir = 2;

        npc_spawn(279, x, y, 0, 0, dir, NULL, 0x100);

        npc->act_wait = m_rand(0, 15);
      }

      break;
  }
}

// Cloud
void npc_act_295(npc_t *npc) {
  static const rect_t rc[4] = {
    {0, 0, 208, 64},
    {32, 64, 144, 96},
    {32, 96, 104, 0x80},
    {104, 96, 144, 0x80},
  };

  switch (npc->act) {
    case 0:
      npc->act = 1;
      npc->anim = npc->dir % 4;
      switch (npc->dir) {
        case 0:
          npc->yvel = -1000;
          npc->view.back = 104 * 0x200;
          npc->view.front = 104 * 0x200;
          break;

        case 1:
          npc->yvel = -0x800;
          npc->view.back = 56 * 0x200;
          npc->view.front = 56 * 0x200;
          break;

        case 2:
          npc->yvel = -0x400;
          npc->view.back = 32 * 0x200;
          npc->view.front = 32 * 0x200;
          break;

        case 3:
          npc->yvel = -0x200;
          npc->view.back = 20 * 0x200;
          npc->view.front = 20 * 0x200;
          break;

        case 4:
          npc->xvel = -0x400;
          npc->view.back = 104 * 0x200;
          npc->view.front = 104 * 0x200;
          break;

        case 5:
          npc->xvel = -0x200;
          npc->view.back = 56 * 0x200;
          npc->view.front = 56 * 0x200;
          break;

        case 6:
          npc->xvel = -0x100;
          npc->view.back = 32 * 0x200;
          npc->view.front = 32 * 0x200;
          break;

        case 7:
          npc->xvel = -0x80;
          npc->view.back = 20 * 0x200;
          npc->view.front = 20 * 0x200;
          break;
      }

      break;

    case 1:
      npc->x += npc->xvel;
      npc->y += npc->yvel;

      if (npc->x < -64 * 0x200) npc->cond = 0;
      if (npc->y < -32 * 0x200) npc->cond = 0;

      break;
  }

  npc->rect = &rc[npc->anim];
}

// Cloud generator
void npc_act_296(npc_t *npc) {
  int x, y, dir, pri;

  if (++npc->act_wait > 16) {
    npc->act_wait = m_rand(0, 16);
    dir = m_rand(0, 100) % 4;

    if (npc->dir == 0) {
      switch (dir) {
        case 0:
          pri = 0x180;
          break;

        case 1:
          pri = 0x80;
          break;

        case 2:
          pri = 0x40;
          break;

        default:
          pri = 0x00;
          break;
      }

      x = m_rand(-10, 10) * 0x200 * 0x10 + npc->x;
      y = npc->y;
      npc_spawn(295, x, y, 0, 0, dir, NULL, pri);
    } else {
      switch (dir) {
        case 0:
          pri = 0x80;
          break;

        case 1:
          pri = 0x55;
          break;

        case 2:
          pri = 0x40;
          break;

        default:
          pri = 0x00;
          break;
      }

      x = npc->x;
      y = m_rand(-7, 7) * 0x200 * 0x10 + npc->y;
      npc_spawn(295, x, y, 0, 0, dir + 4, NULL, pri);
    }
  }
}

// Sue in dragon's mouth
void npc_act_297(npc_t *npc) {
  static const rect_t rc = {112, 48, 0x80, 64};

  npc->x = npc->parent->x + (16 * 0x200);
  npc->y = npc->parent->y + (8 * 0x200);

  npc->rect = &rc;
}

// Doctor (opening)
void npc_act_298(npc_t *npc) {
  static const rect_t rc[8] = {
    {72, 0x80, 88, 160},   {88, 0x80, 104, 160}, {104, 0x80, 120, 160}, {72, 0x80, 88, 160},
    {120, 0x80, 136, 160}, {72, 0x80, 88, 160},  {104, 160, 120, 192},  {120, 160, 136, 192},
  };

  switch (npc->act) {
    case 0:
      npc->act = 1;
      npc->y -= 8 * 0x200;
      // Fallthrough

    case 1:
      npc->anim = 0;
      break;

    case 10:
      npc->act = 11;
      npc->anim = 0;
      npc->anim_wait = 0;
      npc->count1 = 0;
      // Fallthrough

    case 11:
      if (++npc->anim_wait > 6) {
        npc->anim_wait = 0;
        ++npc->anim;
      }

      if (npc->anim > 1) {
        npc->anim = 0;

        if (++npc->count1 > 7) {
          npc->anim = 0;
          npc->act = 1;
        }
      }

      break;

    case 20:
      npc->act = 21;
      npc->anim = 2;
      npc->anim_wait = 0;
      // Fallthrough

    case 21:
      if (++npc->anim_wait > 10) {
        npc->anim_wait = 0;
        ++npc->anim;
      }

      if (npc->anim > 5) npc->anim = 2;

      npc->x += 0x100;

      break;

    case 30:
      npc->anim = 6;
      break;

    case 40:
      npc->act = 41;
      npc->anim = 6;
      npc->anim_wait = 0;
      npc->count1 = 0;
      // Fallthrough

    case 41:
      if (++npc->anim_wait > 6) {
        npc->anim_wait = 0;
        ++npc->anim;
      }

      if (npc->anim > 7) {
        npc->anim = 6;

        if (++npc->count1 > 7) {
          npc->anim = 6;
          npc->act = 30;
        }
      }

      break;
  }

  npc->rect = &rc[npc->anim];
}

// Balrog/Misery (opening)
void npc_act_299(npc_t *npc) {
  static const rect_t rc[2] = {
    {0, 0, 48, 48},
    {48, 0, 96, 48},
  };

  if (npc->act == 0) {
    npc->act = 1;

    if (npc->dir == 0) {
      npc->anim = 1;
      npc->act_wait = 25;
      npc->y -= 0x40 * (50 / 2);
    } else {
      npc->anim = 0;
      npc->act_wait = 0;
    }
  }

  if (++npc->act_wait / 50 % 2)
    npc->y += 0x40;
  else
    npc->y -= 0x40;

  npc->rect = &rc[npc->anim];
}
