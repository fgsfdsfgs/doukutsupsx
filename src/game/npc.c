#include <stdlib.h>
#include <string.h>

#include "engine/common.h"
#include "engine/math.h"
#include "engine/graphics.h"
#include "engine/sound.h"

#include "game/stage.h"
#include "game/npc.h"
#include "game/npctab.h"

u8 npc_flags[NPC_MAX_FLAGS];

npc_t npc_list[NPC_MAX];

// highest live npc in the list
int npc_list_max;

void npc_init(const char *tabpath) {
  npc_list_max = 0;
  memset(&npc_list, 0, sizeof(npc_list));
  // load npc table
  npc_load_classtab(tabpath);
}

static inline void npc_init_instance(npc_t *npc, const u32 class_num) {
  const npc_class_t *nclass = &npc_classtab[class_num];
  npc->info = nclass;
  npc->class_num = class_num;
  npc->life = nclass->life;
  npc->bits = nclass->bits;
  npc->damage = nclass->damage;
  npc->snd_die = nclass->snd_die;
  npc->snd_hit = nclass->snd_hit;
  npc->hit.front = TO_FIX((int)nclass->hit.front);
  npc->hit.back = TO_FIX((int)nclass->hit.back);
  npc->hit.top = TO_FIX((int)nclass->hit.top);
  npc->hit.bottom = TO_FIX((int)nclass->hit.bottom);
  npc->view.front = TO_FIX((int)nclass->view.front);
  npc->view.back = TO_FIX((int)nclass->view.back);
  npc->view.top = TO_FIX((int)nclass->view.top);
  npc->view.bottom = TO_FIX((int)nclass->view.bottom);
}

void npc_kill(npc_t *npc) {
  const int tx = npc->x;
  const int ty = npc->y;
  memset(npc, 0, sizeof(npc));
  npc->x = tx;
  npc->y = ty;
  npc->cond |= NPCCOND_ALIVE;
  npc_init_instance(npc, 3);
}

void npc_death_fx(int x, int y, int w, int num) {
  w = TO_INT(w);

  // spawn smoke
  int ofs_x, ofs_y;
  for (int i = 0; i < num; ++i) {
    ofs_x = TO_FIX(m_rand(-w, w));
    ofs_y = TO_FIX(m_rand(-w, w));
    npc_spawn(NPC_SMOKE, ofs_x, ofs_y, 0, 0, 0, NULL, 0x100);
  }

  // flash
  // SetCaret(x, y, CARET_EXPLOSION, DIR_LEFT);
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
        npc_death_fx(npc->x, npc->y, npc->view.back, 1 << (1 + npc->info->size));
      }
    }
  }
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
  npc_init_instance(npc, class_num);
  npc->cond |= NPCCOND_ALIVE;
  npc->dir = dir;
  npc->x = x;
  npc->y = y;
  npc->xvel = xv;
  npc->yvel = yv;
  npc->other = parent;

  return npc;
}

void npc_parse_event_list(const stage_event_t *ev, const int numev) {
  for (int i = 0; i < numev; ++i) {
    const int dir = (ev[i].bits & NPC_SPAWN_IN_OTHER_DIRECTION) ? 2 : 0;
    const int x = TO_FIX(ev[i].x * TILE_SIZE);
    const int y = TO_FIX(ev[i].y * TILE_SIZE);

    npc_t *npc = &npc_list[i + NPC_STARTIDX_EVENT];
    npc_init_instance(npc, ev[i].class_num);
    npc->cond |= NPCCOND_ALIVE;
    npc->dir = dir;
    npc->x = x;
    npc->y = y;
    npc->event_flag = ev[i].event_flag;
    npc->event_num = ev[i].event_num;
    npc->bits |= ev[i].bits;

    if (npc->bits & NPC_APPEAR_WHEN_FLAG_SET) {
      if (npc_get_flag(npc->event_flag))
        npc->cond |= NPCCOND_ALIVE;
    } else if (npc->bits & NPC_HIDE_WHEN_FLAG_SET) {
      if (!npc_get_flag(npc->event_flag))
        npc->cond |= NPCCOND_ALIVE;
    } else {
      npc->cond |= NPCCOND_ALIVE;
    }

    printf("spawned a %03u at %04d, %04d\n", npc->class_num, ev[i].x << 4, ev[i].y << 4);
  }
  npc_list_max = NPC_STARTIDX_EVENT + numev - 1;
}

void npc_act(void) {
  for (int i = 0; i < npc_list_max; ++i) {
    if (npc_list[i].cond & NPCCOND_ALIVE) {
      npc_func_t actfunc = npc_functab[npc_list[i].class_num];
      actfunc(&npc_list[i]);
      if (npc_list[i].shock)
        --npc_list[i].shock;
    }
  }
}

static inline void npc_draw_instance(npc_t *npc, const int cam_xv, const int cam_yv) {
  // TODO: figure out a better way to cache texrects
  if (npc->rect_prev != npc->rect) {
    npc->rect_prev = npc->rect;
    if (npc->rect) {
      npc->texrect.r = *npc->rect;
      npc->texrect.r.x += npc->rect_delta.x;
      npc->texrect.r.y += npc->rect_delta.y;
      npc->texrect.r.w += npc->rect_delta.w;
      npc->texrect.r.h += npc->rect_delta.h;
      gfx_set_texrect(&npc->texrect, npc->info->surf_id);
    }
  }

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
    if (npc->bits & NPC_SHOW_DAMAGE) {
      // todo
    }
  }

  if (xv + npc->texrect.r.w >= 0 && xv < VID_WIDTH && yv + npc->texrect.r.h >= 0 && yv < VID_HEIGHT)
    gfx_draw_texrect(&npc->texrect, GFX_LAYER_BACK, xv + dx, yv);
}

void npc_draw(int cam_x, int cam_y) {
  cam_x = TO_INT(cam_x);
  cam_y = TO_INT(cam_y);
  for (int i = 0; i <= npc_list_max; ++i) {
    if (npc_list[i].cond & NPCCOND_ALIVE)
      npc_draw_instance(&npc_list[i], cam_x, cam_y);
  }
}
