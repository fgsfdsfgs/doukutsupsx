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

static struct {
  rect_t rect[2];
  s32 mode;
  s32 act;
  s32 count;
  s32 width;
  s32 x;
  s32 y;
} flash;

void cam_init(void) {
  for (int i = 0; i < 16; ++i) {
    rc_fade[i].r.left = i * 16;
    rc_fade[i].r.top = 0;
    rc_fade[i].r.right = rc_fade[i].r.left + 16;
    rc_fade[i].r.bottom = rc_fade[i].r.top + 16;
    gfx_set_texrect(&rc_fade[i], SURFACE_ID_FADE);
  }

  cam_reset();
}

void cam_reset(void) {
  memset(&camera, 0, sizeof(camera));
  memset(&fade, 0, sizeof(fade));
  memset(&flash, 0, sizeof(flash));
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

void cam_target_boss(const int boss_num, const int delay) {
  camera.tgt_x = &npc_boss[boss_num].x;
  camera.tgt_x = &npc_boss[boss_num].y;
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

static inline void fade_out_center(void) {
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

static inline void fade_in_center(void) {
  int x, y;

  for (y = 0; y < (FADE_HEIGHT / 2); ++y)
    for (x = 0; x < (FADE_WIDTH / 2); ++x)
      if ((FADE_WIDTH - 1) - fade.count == x + y)
        fade.flag[y][x] = TRUE;

  for (y = 0; y < (FADE_HEIGHT / 2); ++y)
    for (x = (FADE_WIDTH / 2); x < FADE_WIDTH; ++x)
      if ((FADE_WIDTH - 1) - fade.count == y + ((FADE_WIDTH - 1) - x))
        fade.flag[y][x] = TRUE;

  for (y = (FADE_HEIGHT / 2); y < FADE_HEIGHT; ++y)
    for (x = 0; x < (FADE_WIDTH / 2); ++x)
      if ((FADE_WIDTH - 1) - fade.count == x + ((FADE_HEIGHT - 1) - y))
        fade.flag[y][x] = TRUE;

  for (y = (FADE_HEIGHT / 2); y < FADE_HEIGHT; ++y)
    for (x = (FADE_WIDTH / 2); x < FADE_WIDTH; ++x)
      if ((FADE_WIDTH - 1) - fade.count == ((FADE_WIDTH - 1) - x) + ((FADE_HEIGHT - 1) - y))
        fade.flag[y][x] = TRUE;
}

static inline void cam_update_fade(void) {
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
      if (fade.mode == 1)
        fade_in_center();
      else
        fade_out_center();
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
    gfx_draw_clear(gfx_clear_rgb, GFX_LAYER_FRONT);
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

void cam_start_quake_small(const int duration) {
  camera.quake = duration;
}

void cam_start_quake_big(const int duration) {
  camera.quake2 = duration;
}

void cam_stop_quake(void) {
  camera.quake = 0;
  camera.quake2 = 0;
}

void cam_start_flash(const int x, const int y, const int mode) {
  flash.act = 0;
  flash.mode = mode;
  flash.x = x;
  flash.y = y;
  flash.count = 0;
  flash.width = 0;
}

void cam_stop_flash(void) {
  flash.mode = FLASH_MODE_NONE;
}

void cam_draw_flash(void) {
  static const u8 rgb[] = { 0xFF, 0xFF, 0xFE };
  if (flash.mode != FLASH_MODE_NONE) {
    for (int i = 0; i < 2; ++i) {
      if (flash.rect[i].w)
        gfx_draw_fillrect(rgb, GFX_LAYER_FRONT, flash.rect[i].x, flash.rect[i].y, flash.rect[i].w, flash.rect[i].h);
    }
  }
}

static inline void flash_update_flash(void) {
  int x, y, w, h;

  switch (flash.act) {
    case 0: // stage 1: expand
      flash.count += 0x200;
      flash.width += flash.count;
      x = TO_INT(flash.x - camera.x - flash.width);
      y = TO_INT(flash.y - camera.y - flash.width);
      w = TO_INT(flash.width * 2);
      h = TO_INT(flash.width * 2);
      if (x < 0) x = 0;
      if (y < 0) y = 0;
      if (x + w > VID_WIDTH) w = VID_WIDTH - x;
      if (y + h > VID_HEIGHT) h = VID_HEIGHT - y;
      // vertical strip
      flash.rect[0].x = x;
      flash.rect[0].y = 0;
      flash.rect[0].w = w;
      flash.rect[0].h = VID_HEIGHT;
      // horizontal strip
      flash.rect[1].x = 0;
      flash.rect[1].y = y;
      flash.rect[1].w = VID_WIDTH;
      flash.rect[1].h = h;
      if (flash.width > TO_FIX(VID_WIDTH) * 4) {
        flash.act = 0;
        flash.count = 0;
        flash.width = TO_FIX(VID_HEIGHT);
      }
      break;

    case 1: // stage 2: shrink
      flash.width -= flash.width / 8;
      if (flash.width < 0x100)
        flash.mode = FLASH_MODE_NONE;
      y = TO_INT(flash.y - camera.y - flash.width);
      if (y < 0) y = 0;
      h = TO_INT(flash.width * 2);
      if (y + h > VID_HEIGHT) h = VID_HEIGHT - y;
      // vertical strip doesn't show
      flash.rect[0].x = 0;
      flash.rect[0].y = 0;
      flash.rect[0].w = 0;
      flash.rect[0].h = 0;
      // horizontal strip
      flash.rect[1].x = 0;
      flash.rect[1].y = y;
      flash.rect[1].w = VID_WIDTH;
      flash.rect[1].h = h;
      break;
  }
}

static inline void flash_update_explosion(void) {
  ++flash.count;

  flash.rect[0].left = 0;
  flash.rect[0].right = 0;
  flash.rect[0].top = 0;
  flash.rect[0].bottom = 0;

  if (flash.count / 2 % 2) {
    flash.rect[1].x = 0;
    flash.rect[1].y = 0;
    flash.rect[1].w = VID_WIDTH;
    flash.rect[1].h = VID_HEIGHT;
  } else {
    flash.rect[1].x = 0;
    flash.rect[1].y = 0;
    flash.rect[1].w = 0;
    flash.rect[1].h = 0;
  }

  if (flash.count > 20)
    flash.mode = FLASH_MODE_NONE;
}

static inline void cam_update_flash(void) {
  switch (flash.mode) {
    case FLASH_MODE_EXPLOSION:
      flash_update_explosion();
      break;
    case FLASH_MODE_FLASH:
      flash_update_flash();
      break;
  }
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
    --camera.quake;
  }

  if (fade.mode != 0)
    cam_update_fade();

  if (flash.mode != FLASH_MODE_NONE)
    cam_update_flash();
}
