#include <string.h>

#include "engine/common.h"
#include "engine/math.h"
#include "engine/graphics.h"

#include "game/game.h"
#include "game/stage.h"
#include "game/player.h"
#include "game/npc.h"
#include "game/camera.h"

#define FADE_WIDTH (((VID_WIDTH - 1) / 16) + 1)
#define FADE_HEIGHT (((VID_HEIGHT - 1) / 16) + 1)

cam_t camera;

static gfx_texrect_t rc_fade[16];

static struct {
  s32 mode;
  s32 count;
  s32 dir;
  bool full;
  s8 anim[FADE_HEIGHT][FADE_WIDTH];
  s8 flag[FADE_HEIGHT][FADE_WIDTH];
} fade;

void cam_init(void) {
  for (int i = 0; i < 16; ++i) {
    rc_fade[i].r.left = i * 16;
    rc_fade[i].r.top = 0;
    rc_fade[i].r.right = rc_fade[i].r.left + 16;
    rc_fade[i].r.bottom = rc_fade[i].r.top + 16;
    gfx_set_texrect(&rc_fade[i], SURFACE_ID_FADE);
  }
}

static inline void cam_bound(void) {
  const int map_w = stage_data->width;
  const int map_h = stage_data->height;

  if (TO_INT(camera.x) < 0)
    camera.x = 0;
  if (TO_INT(camera.y) < 0)
    camera.y = 0;

  const int x_max = TO_FIX(((map_w - 1) * TILE_SIZE) - VID_WIDTH);
  const int y_max = TO_FIX(((map_h - 1) * TILE_SIZE) - VID_HEIGHT);

  if (camera.x > x_max)
    camera.x = x_max;
  if (camera.y > y_max)
    camera.y = y_max;
}

void cam_update(void) {
  camera.x += (*camera.tgt_x - (TO_FIX(VID_WIDTH) / 2) - camera.x) / camera.wait;
  camera.y += (*camera.tgt_y - (TO_FIX(VID_HEIGHT) / 2) - camera.y) / camera.wait;

  cam_bound();

  if (camera.quake2) {
    camera.x += (m_rand(-5, 5) * 0x200);
    camera.y += (m_rand(-3, 3) * 0x200);
    --camera.quake2;
  } else if (camera.quake) {
    camera.x += (m_rand(-1, 1) * 0x200);
    camera.y += (m_rand(-1, 1) * 0x200);
  }

  if (fade.mode != 0)
    cam_update_fade();
}

void cam_center_on_player(void) {
  camera.x = player.x - TO_FIX(VID_WIDTH / 2);
  camera.y = player.y - TO_FIX(VID_HEIGHT / 2);
  cam_bound();
}

void cam_target_player(const int delay) {
  camera.tgt_x = &player.tgt_x;
  camera.tgt_y = &player.tgt_y;
  camera.wait = delay;
}

void cam_target_npc(const int event_num, const int delay) {
  npc_t *npc = npc_find_by_event_num(event_num);
  if (npc) {
    camera.tgt_x = &npc->x;
    camera.tgt_y = &npc->y;
    camera.wait = delay;
  }
}

void cam_set_pos(const int fx, const int fy) {
  camera.x = fx;
  camera.y = fy;

  cam_bound();

  camera.quake = 0;
  camera.quake2 = 0;
}

void cam_set_target(s32 *tx, s32 *ty, const int wait) {
  if (wait >= 0)
    camera.wait = wait;
  camera.tgt_x = tx;
  camera.tgt_y = ty;
}

void cam_start_fade_out(const int dir) {
  fade.mode = 2;
  fade.count = 0;
  fade.dir = dir;
  fade.full = FALSE;
  memset(fade.anim, 0x00, sizeof(fade.anim));
  memset(fade.flag, 0x00, sizeof(fade.flag));
}

void cam_start_fade_in(const int dir) {
  fade.mode = 1;
  fade.count = 0;
  fade.dir = dir;
  fade.full = TRUE;
  memset(fade.anim, 0x0F, sizeof(fade.anim));
  memset(fade.flag, 0x00, sizeof(fade.flag));
}

// jesus take the wheel
static inline void fade_center(void) {
  int x, y;

  for (y = 0; y < (FADE_HEIGHT / 2); ++y)
    for (x = 0; x < (FADE_WIDTH / 2); ++x)
      if (fade.count == x + y)
        fade.flag[y][x] = TRUE;

  for (y = 0; y < (FADE_HEIGHT / 2); ++y)
    for (x = (FADE_WIDTH / 2); x < FADE_WIDTH; ++x)
      if (fade.count == y + ((FADE_WIDTH - 1) - x))
        fade.flag[y][x] = TRUE;

  for (y = (FADE_HEIGHT / 2); y < FADE_HEIGHT; ++y)
    for (x = 0; x < (FADE_WIDTH / 2); ++x)
      if (fade.count == x + ((FADE_HEIGHT - 1) - y))
        fade.flag[y][x] = TRUE;

  for (y = (FADE_HEIGHT / 2); y < FADE_HEIGHT; ++y)
    for (x = (FADE_WIDTH / 2); x < FADE_WIDTH; ++x)
      if (fade.count == ((FADE_WIDTH - 1) - x) + ((FADE_HEIGHT - 1) - y))
        fade.flag[y][x] = TRUE;
}

void cam_update_fade(void) {
  int t;

  switch (fade.dir) {
    case DIR_LEFT:
      t = (FADE_WIDTH - 1) - fade.count;
      if (t >= 0) {
        for (int y = 0; y < FADE_HEIGHT; ++y)
          fade.flag[y][t] = TRUE;
      }
      break;
    case DIR_RIGHT:
      t = fade.count;
      if (t < FADE_WIDTH) {
        for (int y = 0; y < FADE_HEIGHT; ++y)
          fade.flag[y][t] = TRUE;
      }
      break;
    case DIR_UP:
      t = (FADE_HEIGHT - 1) - fade.count;
      if (t >= 0) {
        for (int x = 0; x < FADE_WIDTH; ++x)
          fade.flag[t][x] = TRUE;
      }
      break;
    case DIR_DOWN:
      t = fade.count;
      if (t < FADE_HEIGHT) {
        for (int x = 0; x < FADE_WIDTH; ++x)
          fade.flag[t][x] = TRUE;
      }
      break;
    default: // from center
      fade_center();
      break;
  }

  ++fade.count;

  if (fade.mode == 1) {
    // fade in
    fade.full = FALSE;
    for (int y = 0; y < FADE_HEIGHT; ++y) {
      for (int x = 0; x < FADE_WIDTH; ++x) {
        if (fade.anim[y][x] > 0 && fade.flag[y][x])
          --fade.anim[y][x];
      }
    }
    if (fade.count > FADE_WIDTH + 16)
      fade.mode = 0;
  } else {
    // fade out
    for (int y = 0; y < FADE_HEIGHT; ++y) {
      for (int x = 0; x < FADE_WIDTH; ++x) {
        if (fade.anim[y][x] < 0x0F && fade.flag[y][x])
          ++fade.anim[y][x];
      }
    }
    if (fade.count > FADE_WIDTH + 16) {
      fade.mode = 0;
      fade.full = TRUE;
    }
  }
}

void cam_draw_fade(void) {
  if (fade.full) {
    // fade out finished or fade in not started; just clear screen
    gfx_draw_fillrect(gfx_clear_rgb, GFX_LAYER_FRONT, 0, 0, VID_WIDTH, VID_HEIGHT);
  } else if (fade.mode) {
    // draw fade animation
    for (int y = 0; y < FADE_HEIGHT; ++y) {
      for (int x = 0; x < FADE_WIDTH; ++x)
        gfx_draw_texrect_16x16(&rc_fade[fade.anim[y][x]], GFX_LAYER_FRONT, x * 16, y * 16);
    }
  }
}

bool cam_is_fading(void) {
  return (fade.mode != 0);
}

void cam_clear_fade(void) {
  fade.mode = 0;
  fade.full = FALSE;
}

void cam_complete_fade(void) {
  fade.full = TRUE;
}
