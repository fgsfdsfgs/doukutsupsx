#include <stdlib.h>
#include <string.h>

#include "engine/common.h"
#include "engine/math.h"
#include "engine/graphics.h"
#include "engine/sound.h"

#include "game/game.h"
#include "game/stage.h"
#include "game/npc.h"
#include "game/player.h"
#include "game/npctab.h"
#include "game/caret.h"
#include "game/dmgnum.h"

u8 npc_flags[NPC_MAX_FLAGS];

npc_t npc_list[NPC_MAX];
// highest live npc in the list
int npc_list_max;

// all npcs that are part of the boss we're currently fighting
npc_t npc_boss[NPC_MAX_BOSS];
// highest live npc in the boss list
int npc_boss_max;

// Curly-related stuff, dunno where else to put it
npc_curly_t npc_curly_state;
// Doctor boss battle globals, same here
npc_doctor_t npc_doctor_state;

void npc_init(const char *tabpath) {
  // clear lists
  npc_reset();
  // load npc table
  npc_load_classtab(tabpath);
}

void npc_reset(void) {
  npc_list_max = 0;
  memset(&npc_list, 0, sizeof(npc_list));
  npc_boss_max = -1;
  memset(&npc_boss, 0, sizeof(npc_boss));
  memset(npc_flags, 0, sizeof(npc_flags));
}

static inline void npc_set_class(npc_t *npc, const u32 class_num) {
  const npc_class_t *nclass = &npc_classtab[class_num];
  npc->info = nclass;
  npc->class_num = class_num;
  npc->life = nclass->life;
  npc->bits |= nclass->bits;
  npc->damage = nclass->damage;
  npc->snd_die = nclass->snd_die;
  npc->snd_hit = nclass->snd_hit;
  npc->surf = nclass->surf_id;
  npc->size = nclass->size;
  npc->hit.front = TO_FIX((int)nclass->hit.front);
  npc->hit.back = TO_FIX((int)nclass->hit.back);
  npc->hit.top = TO_FIX((int)nclass->hit.top);
  npc->hit.bottom = TO_FIX((int)nclass->hit.bottom);
  npc->view.front = TO_FIX((int)nclass->view.front);
  npc->view.back = TO_FIX((int)nclass->view.back);
  npc->view.top = TO_FIX((int)nclass->view.top);
  npc->view.bottom = TO_FIX((int)nclass->view.bottom);
}

// i.e. turn into the damage holder object
void npc_show_death_damage(npc_t *npc) {
  const int tx = npc->x;
  const int ty = npc->y;
  memset(npc, 0, sizeof(npc));
  npc->x = tx;
  npc->y = ty;
  npc->cond |= NPCCOND_ALIVE;
  npc->bits = 0;
  npc_set_class(npc, 3);
}

void npc_kill(npc_t *npc, bool show_damage) {
  int val;

  // Play death sound
  snd_play_sound(-1, npc->snd_die, SOUND_MODE_PLAY);

  // Create smoke
  switch (npc->size) {
    case 1:
      npc_spawn_death_fx(npc->x, npc->y, npc->view.back, 3, 0);
      break;

    case 2:
      npc_spawn_death_fx(npc->x, npc->y, npc->view.back, 7, 0);
      break;

    case 3:
      npc_spawn_death_fx(npc->x, npc->y, npc->view.back, 12, 0);
      break;
  }

  // Create drop
  if (npc->exp != 0) {
    switch (m_rand(1, 5)) {
      case 1:
        // Spawn health
        if (npc->exp > 6)
          val = 6;
        else
          val = 2;
        npc_spawn_life(npc->x, npc->y, val);
        break;

      case 2:
        // Spawn missile launcher ammo
        if (npc->exp > 6)
          val = 3;
        else
          val = 1;
        if (npc_spawn_ammo(npc->x, npc->y, val))
          break;

        // Fallthrough
      default:
        // Spawn weapon energy
        npc_spawn_exp(npc->x, npc->y, npc->exp);
        break;
    }
  }

  // Set flag
  npc_set_flag(npc->event_flag);

  // Create value view
  if (npc->bits & NPC_SHOW_DAMAGE) {
    dmgnum_spawn(&npc->x, &npc->y, npc->damage_view);
    if (show_damage)
      npc_show_death_damage(npc);
  } else {
    npc->cond = 0;
  }
}

void npc_spawn_death_fx(int x, int y, int w, int num, int up) {
  w = TO_INT(w);

  // spawn smoke
  int ofs_x, ofs_y;
  for (int i = 0; i < num; ++i) {
    ofs_x = TO_FIX(m_rand(-w, w));
    ofs_y = TO_FIX(m_rand(-w, w));
    npc_spawn(NPC_SMOKE, x + ofs_x, y + ofs_y, 0, 0, up, NULL, 0x100);
  }

  // flash
  caret_spawn(x, y, CARET_EXPLOSION, DIR_LEFT);
}

void npc_delete(npc_t *npc) {
  if (npc->cond & NPCCOND_ALIVE) {
    const int n = npc - npc_list;
    if (n == npc_list_max)
      --npc_list_max;
    npc->cond = 0;
    npc_set_flag(npc->event_flag);
  }
}

void npc_delete_by_class(const int class_num, const int spawn_smoke) {
  for (int i = 0; i <= npc_list_max; ++i) {
    npc_t *npc = &npc_list[i];
    if ((npc->cond & NPCCOND_ALIVE) && npc->class_num == class_num) {
      npc_delete(npc);
      if (spawn_smoke) {
        snd_play_sound(-1, npc->snd_die, SOUND_MODE_PLAY);
        npc_spawn_death_fx(npc->x, npc->y, npc->view.back, 1 << (1 + npc->size), 0);
      }
    }
  }
}

void npc_delete_by_event_num(const int event_num) {
  for (int i = 0; i <= npc_list_max; ++i) {
    npc_t *npc = &npc_list[i];
    if ((npc->cond & NPCCOND_ALIVE) && npc->event_num == event_num) {
      npc->cond = 0;
      npc_set_flag(npc->event_flag);
    }
  }
}

static inline void npc_init_instance(npc_t *npc, int class_num, int x, int y, int dir) {
  npc->bits = 0;
  npc_set_class(npc, class_num);
  npc->cond |= NPCCOND_ALIVE;
  npc->dir = dir;
  npc->x = x;
  npc->y = y;
}

npc_t *npc_spawn(int class_num, int x, int y, int xv, int yv, int dir, npc_t *parent, int startidx) {
  int n = startidx;
  while (n < NPC_MAX && npc_list[n].cond)
    ++n;

  if (n == NPC_MAX)
    return NULL;

  if (n > npc_list_max)
    npc_list_max = n;

  npc_t *npc = &npc_list[n];
  memset(npc, 0, sizeof(*npc));
  npc_init_instance(npc, class_num, x, y, dir);
  npc->xvel = xv;
  npc->yvel = yv;
  npc->parent = parent;
  npc->exp = npc->info->exp;

  return npc;
}

void npc_parse_event_list(const stage_event_t *ev, const int numev) {
  memset(npc_list, 0, sizeof(npc_list));
  npc_boss[0].cond = 0;

  for (int i = 0; i < numev; ++i) {
    const int dir = (ev[i].bits & NPC_SPAWN_IN_OTHER_DIRECTION) ? 2 : 0;
    const int x = TO_FIX(ev[i].x * TILE_SIZE);
    const int y = TO_FIX(ev[i].y * TILE_SIZE);

    npc_t *npc = &npc_list[i + NPC_STARTIDX_EVENT];
    npc_init_instance(npc, ev[i].class_num, x, y, dir);
    npc->event_flag = ev[i].event_flag;
    npc->event_num = ev[i].event_num;
    npc->bits |= ev[i].bits;
    npc->exp = npc->info->exp;
    npc->cond = 0;

    if (npc->bits & NPC_APPEAR_WHEN_FLAG_SET) {
      if (npc_get_flag(npc->event_flag))
        npc->cond |= NPCCOND_ALIVE;
    } else if (npc->bits & NPC_HIDE_WHEN_FLAG_SET) {
      if (!npc_get_flag(npc->event_flag))
        npc->cond |= NPCCOND_ALIVE;
    } else {
      npc->cond |= NPCCOND_ALIVE;
    }
  }

  npc_list_max = NPC_STARTIDX_EVENT + numev - 1;
  npc_boss_max = -1; // absolutely no boss yet
}

void npc_act(void) {
  // regular npcs update first
  for (int i = 0; i <= npc_list_max; ++i) {
    if (npc_list[i].cond & NPCCOND_ALIVE) {
      npc_func_t actfunc = npc_functab[npc_list[i].class_num];
      actfunc(&npc_list[i]);
      if (npc_list[i].shock)
        --npc_list[i].shock;
    }
  }
  // bosses update last
  if (npc_boss[0].cond & NPCCOND_ALIVE) {
    npc_func_t actfunc = npc_boss_functab[npc_boss[0].class_num];
    actfunc(&npc_boss[0]);
    for (int i = 0; i <= npc_boss_max; ++i) {
      if (npc_boss[i].cond & NPCCOND_ALIVE) {
        if (npc_boss[i].shock)
          --npc_boss[i].shock;
      }
    }
  }
}

static inline void npc_update_texrects(npc_t *npc) {
  npc->rect_prev = npc->rect;
  if (npc->rect) {
    npc->texrect.r.x = npc->rect->x + npc->rect_delta.x;
    npc->texrect.r.y = npc->rect->y + npc->rect_delta.y;
    npc->texrect.r.w = npc->rect->w + npc->rect_delta.w;
    npc->texrect.r.h = npc->rect->h + npc->rect_delta.h;
    npc->rect_delta.x = 0;
    npc->rect_delta.y = 0;
    npc->rect_delta.w = 0;
    npc->rect_delta.h = 0;
    npc->texrect_wide.tpage = 0;
    npc->texrect_wide.r.w = 0;
    gfx_set_texrect(&npc->texrect, npc->surf);
    // if the sprite is so wide it intersects the texture page boundary, add a second texrect
    if ((int)npc->texrect.u + npc->texrect.r.w > 256) {
      const int part_w = (256 - (int)npc->texrect.u);
      npc->texrect_wide.r.x = npc->texrect.r.x + part_w;
      npc->texrect_wide.r.y = npc->texrect.r.y;
      npc->texrect_wide.r.right = npc->texrect_wide.r.x + npc->texrect.r.w - part_w;
      npc->texrect_wide.r.bottom = npc->texrect.r.y + npc->texrect.r.h;
      gfx_set_texrect(&npc->texrect_wide, npc->surf);
      npc->texrect.r.w = part_w;
    }
  }
}

static inline void npc_draw_instance(npc_t *npc, const int cam_xv, const int cam_yv) {
  // TODO: figure out a better way to cache texrects
  if (npc->rect_prev != npc->rect)
    npc_update_texrects(npc);

  if (!npc->rect)
    return;

  const int side = npc->dir ? npc->view.back : npc->view.front;
  const int xv = TO_INT(npc->x - side) - cam_xv;
  const int yv = TO_INT(npc->y - npc->view.top) - cam_yv;

  int dx;
  if (npc->shock) {
    // what the fuck is this
    dx = 2 * ((npc->shock / 2) % 2) - 1;
  } else {
    dx = 0;
    if ((npc->bits & NPC_SHOW_DAMAGE) && npc->damage_view) {
      dmgnum_spawn(&npc->x, &npc->y, npc->damage_view);
      npc->damage_view = 0;
    }
  }

  const int total_w = npc->texrect.r.w + npc->texrect_wide.r.w;
  if (xv + total_w + 8 >= 0 && xv < VID_WIDTH && yv + npc->texrect.r.h + 8 >= 0 && yv < VID_HEIGHT) {
    gfx_draw_texrect(&npc->texrect, GFX_LAYER_BACK, xv + dx, yv);
    if (npc->texrect_wide.tpage)
      gfx_draw_texrect(&npc->texrect_wide, GFX_LAYER_BACK, xv + dx + npc->texrect.r.w, yv);
  }
}

void npc_draw(int cam_x, int cam_y) {
  cam_x = TO_INT(cam_x);
  cam_y = TO_INT(cam_y);
  // draw bosses first
  for (int i = npc_boss_max; i >= 0; --i) {
    if (npc_boss[i].cond & NPCCOND_ALIVE)
      npc_draw_instance(&npc_list[i], cam_x, cam_y);
  }
  // draw regular npcs after
  for (int i = 0; i <= npc_list_max; ++i) {
    if (npc_list[i].cond & NPCCOND_ALIVE)
      npc_draw_instance(&npc_list[i], cam_x, cam_y);
  }
}

void npc_spawn_exp(int x, int y, int exp) {
  int n = NPC_STARTIDX_DYNAMIC;

  while (exp) {
    npc_t *npc = npc_spawn(NPC_EXP, x, y, 0, 0, 0, NULL, n++);
    if (!npc) break;
  
    int sub_exp;
    if (exp >= 20) {
      exp -= 20;
      sub_exp = 20;
    } else if (exp >= 5) {
      exp -= 5;
      sub_exp = 5;
    } else if (exp >= 1) {
      exp -= 1;
      sub_exp = 1;
    }

    npc->exp = sub_exp;
  }
}

npc_t *npc_spawn_life(int x, int y, int val) {
  npc_t *npc = npc_spawn(87, x, y, 0, 0, 0, NULL, NPC_STARTIDX_DYNAMIC);
  if (!npc) return NULL;
  npc->exp = val;
  return npc;
}

npc_t *npc_spawn_ammo(int x, int y, int val) {
  // don't spawn anything if the player has no missile launchers
  if (!player.arms[5].owned && !player.arms[10].owned)
    return NULL;
  npc_t *npc = npc_spawn(86, x, y, 0, 0, 0, NULL, NPC_STARTIDX_DYNAMIC);
  if (!npc) return NULL;
  npc->exp = val;
  return npc;
}

npc_t *npc_find_by_class(const int class_num) {
  for (int i = 0; i <= npc_list_max; ++i) {
    if (npc_list[i].class_num == class_num && (npc_list[i].cond & NPCCOND_ALIVE))
      return &npc_list[i];
  }
  return NULL;
}

npc_t *npc_find_by_event_num(const int event_num) {
  for (int i = 0; i <= npc_list_max; ++i) {
    if (npc_list[i].event_num == event_num && (npc_list[i].cond & NPCCOND_ALIVE))
      return &npc_list[i];
  }
  return NULL;
}

static inline void npc_set_dir(npc_t *npc, const int dir) {
  if (dir == DIR_AUTO) {
    // face player
    if (player.x > npc->x)
      npc->dir = DIR_RIGHT;
    else
      npc->dir = DIR_LEFT;
  } else if (dir != DIR_OTHER) {
    npc->dir = dir;
  }
}

void npc_change_action(npc_t *npc, const int act, const int dir) {
  npc->act = act;
  npc_set_dir(npc, dir);
}

void npc_change_class(npc_t *npc, const int class_num, const int dir, const u16 addbits) {
  npc->bits &= ~(
    NPC_SOLID_SOFT |
    NPC_IGNORE_TILE_44 |
    NPC_INVULNERABLE |
    NPC_IGNORE_SOLIDITY |
    NPC_BOUNCY |
    NPC_SHOOTABLE |
    NPC_SOLID_HARD |
    NPC_REAR_AND_TOP_DONT_HURT |
    NPC_SHOW_DAMAGE
  );
  npc_set_class(npc, class_num);
  npc->bits |= addbits;
  npc->exp = npc->info->exp;
  npc->cond |= NPCCOND_ALIVE;
  npc->act = 0;
  npc->act_wait = 0;
  npc->anim = 0;
  npc->anim_wait = 0;
  npc->count1 = 0;
  npc->count2 = 0;
  npc->rect = NULL;
  npc->rect_prev = NULL;
  npc->rect_delta.x = 0;
  npc->rect_delta.y = 0;
  npc->rect_delta.w = 0;
  npc->rect_delta.h = 0;
  npc->xvel = 0;
  npc->yvel = 0;

  npc_set_dir(npc, dir);

  // tick once
  npc_functab[class_num](npc);
}

void npc_change_class_by_event_num(const int event_num, const int class_num, const int dir, const u16 addbits) {
  for (int i = 0; i <= npc_list_max; ++i) {
    if (npc_list[i].event_num == event_num && (npc_list[i].cond & NPCCOND_ALIVE))
      npc_change_class(&npc_list[i], class_num, dir, addbits);
  }
}

void npc_set_pos(npc_t *npc, const int x, const int y, const int dir) {
  npc->x = x;
  npc->y = y;
  npc_set_dir(npc, dir);
}

npc_t *npc_spawn_boss(const int boss_id) {
  memset(npc_boss, 0, sizeof(npc_boss));
  npc_boss[0].class_num = boss_id;
  if (boss_id) {
    npc_boss[0].cond = NPCCOND_ALIVE;
    npc_boss_max = 0;
  }
  return &npc_boss[0];
}

void npc_set_boss_act(const int act) {
  npc_boss[0].act = act;
}
