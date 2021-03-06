#include <stdio.h>

#include "engine/common.h"
#include "engine/memory.h"
#include "engine/graphics.h"
#include "engine/sound.h"
#include "engine/org.h"
#include "engine/timer.h"

#include "game/game.h"
#include "game/player.h"
#include "game/bullet.h"
#include "game/caret.h"
#include "game/camera.h"
#include "game/dmgnum.h"
#include "game/tsc.h"
#include "game/npc.h"
#include "game/hud.h"
#include "game/stage.h"

// pointer to current stage
stage_t *stage_data;
// water level
int stage_water_y;

// currently loaded stage bank and its id
stage_bank_t *stage_bank;
u32 stage_bank_id;

// pointers to loaded stages indexed by id, or NULL if unloaded
static stage_t *stages[MAX_STAGES];

// path to current bank
static char bank_path[CD_MAX_PATH] = STAGE_PATH_PREFIX STAGE_PATH_FORMAT;

// rect for that one stupid tile in NpcSym
static gfx_texrect_t rc_snack_tile = { 256, 48, 272, 64 };

// rects for water/wind currents
static gfx_texrect_t rc_wind_tile[4][16];

// rect for the background
static gfx_texrect_t rc_back[2];
// background position
static int bg_x;
static int bg_y;

// previous music track
static u32 music_prev = 0;
// position in previous music track
static u32 music_prev_pos = 0;

// water/wind current animation frame
static u32 wind_count = 0;

void stage_init(void) {
  // the surface should be already loaded by now
  if (rc_snack_tile.tpage == 0)
    gfx_set_texrect(&rc_snack_tile, SURFACE_ID_NPC_SYM);
  // water current tiles
  if (rc_wind_tile[0][0].tpage == 0) {
    for (int i = 0; i < 16; ++i) {
      rc_wind_tile[0][i].r.left = 224 + i;
      rc_wind_tile[0][i].r.top = 48;
      gfx_set_texrect(&rc_wind_tile[0][i], SURFACE_ID_CARET);
    }
    for (int i = 0; i < 16; ++i) {
      rc_wind_tile[1][i].r.left = 224;
      rc_wind_tile[1][i].r.top = 48 + i;
      gfx_set_texrect(&rc_wind_tile[1][i], SURFACE_ID_CARET);
    }
    for (int i = 0; i < 16; ++i) {
      rc_wind_tile[2][i].r.left = 240 - i;
      rc_wind_tile[2][i].r.top = 48;
      gfx_set_texrect(&rc_wind_tile[2][i], SURFACE_ID_CARET);
    }
    for (int i = 0; i < 16; ++i) {
      rc_wind_tile[3][i].r.left = 224;
      rc_wind_tile[3][i].r.top = 64 - i;
      gfx_set_texrect(&rc_wind_tile[3][i], SURFACE_ID_CARET);
    }
  }
}

void stage_reset(void) {
  bg_x = 0;
  bg_y = 0;
  music_prev = 0;
  music_prev_pos = 0;
  stage_free_stage_bank();
}

int stage_load_stage_bank(const u32 id) {
  // form path string
  // stages have to be separated into folders because dir headers are very small
  const char *hex_chars = "0123456789ABCDEF";
  const int id_hi = hex_chars[id >> 4];
  const int id_lo = hex_chars[id & 0xF];
  bank_path[STAGE_PATH_START + 0] = id_hi;
  bank_path[STAGE_PATH_START + 7] = id_hi;
  bank_path[STAGE_PATH_START + 8] = id_lo;

  fs_file_t *f = fs_fopen(bank_path, 0);
  if (!f)
    PANIC("could not load stage bank\n%s", bank_path);

  // just load the entire fucking thing at once
  const u32 bank_len = fs_fsize(f);
  stage_bank_t *bank = mem_alloc(bank_len);
  fs_fread_or_die(bank, bank_len, 1, f);

  fs_fclose(f);

  // upload the gfx bank and chop it off since it's at the end
  ASSERT(bank->surfofs < bank_len);
  gfx_bank_t *gfx_bank = (gfx_bank_t *)((u8 *)bank + bank->surfofs);
  gfx_upload_gfx_bank(gfx_bank, NULL);
  bank = mem_realloc(bank, bank->surfofs);

  // set up stage pointers
  for (u32 i = 0; i < bank->numstages; ++i) {
    stage_t *stage = (stage_t *)((u8 *)bank + bank->stageofs[i]);
    stages[stage->id] = stage;
  }

  stage_bank = bank;
  stage_bank_id = id;

  printf("stage bank %02x loaded\nfree mem: %u bytes\n", id, mem_get_free_space());

  return TRUE;
}

void stage_free_stage_bank(void) {
  if (!stage_bank)
    return;

  // clear stage pointers
  for (u32 i = 0; i < stage_bank->numstages; ++i) {
    stage_t *stage = (stage_t *)((u8 *)stage_bank + stage_bank->stageofs[i]);
    stages[stage->id] = NULL;
  }

  stage_bank = NULL;
  stage_data = NULL;
  stage_bank_id = 0;

  mem_free_to_mark(MEM_MARK_HI);
}

static inline void stage_init_background(void) {
  // set background texrects
  if (stage_data->bk_type == BACKGROUND_TYPE_WATER) {
    rc_back[0].r.left = 0;
    rc_back[0].r.top = 0;
    rc_back[0].r.right = 32;
    rc_back[0].r.bottom = 16;
    rc_back[1].r.left = 0;
    rc_back[1].r.top = 16;
    rc_back[1].r.right = 32;
    rc_back[1].r.bottom = 48;
    gfx_set_texrect(&rc_back[0], SURFACE_ID_LEVEL_BACKGROUND);
    gfx_set_texrect(&rc_back[1], SURFACE_ID_LEVEL_BACKGROUND);
  } else {
    rc_back[0].r.left = 0;
    rc_back[0].r.top = 0;
    rc_back[0].r.right = stage_bank->bk_width;
    rc_back[0].r.bottom = stage_bank->bk_height;
    gfx_set_texrect(&rc_back[0], SURFACE_ID_LEVEL_BACKGROUND);
  }
}

int stage_transition(const u32 id, const u32 event, int plr_x, int plr_y) {
  if (!stages[id]) {
    gfx_draw_loading();
    stage_free_stage_bank();
    stage_load_stage_bank(id);
  }

  ASSERT(stages[id]);

  stage_data = stages[id];
  stage_init_background();

  tsc_script_t *script = (tsc_script_t *)&stage_data->map_data[stage_data->tsc_offset];
  tsc_set_stage_script(script, stage_data->tsc_size);
  tsc_switch_script(TSC_SCRIPT_STAGE);

  npc_parse_event_list((stage_event_t *)&stage_data->map_data[stage_data->ev_offset], stage_data->ev_count);

  plr_set_pos(TO_FIX(plr_x) * TILE_SIZE, TO_FIX(plr_y) * TILE_SIZE);

  tsc_start_event(event);
  cam_center_on_player();
  bullet_init();
  caret_init();
  dmgnum_init();
  hud_clear();
  plr_star_reset();
  npc_spawn_boss(stage_data->boss_type);

  stage_water_y = TO_FIX(240 * TILE_SIZE);
  game_tick = 0;

  printf("set stage %u: %ux%u ofs %u %u %u\n", stage_data->id, stage_data->width, stage_data->height,
    stage_data->ev_offset, stage_data->tsc_offset, (u8 *)&stage_data->map_data[stage_data->tsc_offset] - (u8 *)stage_bank);
  printf("tileset (%d, %d, %02x, %04x)\n", gfx_surf[2].tex_x, gfx_surf[2].tex_y, gfx_surf[2].mode, gfx_surf[2].clut);

  return TRUE;
}

static inline void stage_load_music(const u32 id) {
  if (id == 0) {
    // NULL music, just free the current track
    org_free();
    return;
  }

  // find song in bank
  const stage_song_t *sng = NULL;
  for (u32 i = 0; i < stage_bank->numsongs; ++i) {
    if (id == stage_bank->songs[i].music_id) {
      sng = &stage_bank->songs[i];
      break;
    }
  }

  // free previous track
  org_free();

  // bail if this song is from a different stage bank
  // this can theoretically happen if we load into a stage bank different than where the music started
  if (!sng) {
    printf("stage_load_music(): music %02x is not in stage bank %02x\n", id, stage_bank_id);
    // ensure that next time the proper music does start
    music_prev = 0;
    music_prev_pos = 0;
    return;
  }

  // copy new track
  sfx_bank_t *orgbank = (sfx_bank_t *)((u8 *)stage_bank + sng->bank_ofs);
  u8 *orgdata = (u8 *)orgbank + sizeof(*orgbank) + sizeof(u32) * orgbank->num_sfx + orgbank->data_size;
  org_load(id, orgdata, orgbank);
}

void stage_change_music(const u32 id) {
  music_prev_pos = org_get_pos();
  music_prev = org_get_id();
  if (music_prev != id) {
    stage_load_music(id);
    if (id != ORG_INVALID)
      org_restart_from(0);
  }
}

void stage_resume_music(void) {
  if (music_prev != ORG_INVALID) {
    stage_load_music(music_prev);
    org_restart_from(music_prev_pos);
  }
}

void stage_update(void) {
  switch (stage_data->bk_type) {
    case BACKGROUND_TYPE_AUTOSCROLL:
      bg_x += TO_FIX(6);
      break;
    case BACKGROUND_TYPE_CLOUDS:
    case BACKGROUND_TYPE_CLOUDS_WINDY:
      bg_x = (bg_x + 1) % (VID_WIDTH * 2);
      break;
  }
  wind_count += 2;
}

static inline void stage_draw_bg_grid(const int start_x, const int start_y) {
  for (int y = start_y; y < VID_HEIGHT; y += rc_back[0].r.h) {
    for (int x = start_x; x < VID_WIDTH; x += rc_back[0].r.w)
      gfx_draw_texrect(&rc_back[0], GFX_LAYER_BACK, x, y);
  }
}

static void stage_draw_bg_wide_strip(const rect_t *r, const int x, const int y) {
  rc_back[0].r = *r;
  gfx_set_texrect(&rc_back[0], SURFACE_ID_LEVEL_BACKGROUND);
  gfx_draw_texrect_wide(&rc_back[0], GFX_LAYER_BACK, x, y);
}

// FIXME: drawing the rects right away causes a lot of tpage switching,
//        maybe prepare them in update() instead
static inline void stage_draw_bg_clouds(const int cam_vx, const int cam_vy) {
  static rect_t rc[] = {
    { 0, 0,   320,  88 }, // sky
    { 0, 88,  320, 123 }, // cloud strip 1
    { 0, 123, 320, 146 }, // cloud strip 2
    { 0, 146, 320, 176 }, // cloud strip 3
    { 0, 176, 320, 240 }, // cloud strip 4
  };

  const int xofs[] = { 0, bg_x / 2, bg_x, bg_x * 2, bg_x * 4 };

  // sky
  stage_draw_bg_wide_strip(&rc[0], 0, 0);

  // scrolling cloud strips
  for (int i = 1; i < 5; ++i) {
    const int tx = xofs[i] % 320;
    rc[i].left = tx;
    stage_draw_bg_wide_strip(&rc[i], 0, rc[i].top);
    rc[i].left = 0;
    stage_draw_bg_wide_strip(&rc[i], 320 - tx, rc[i].top);
  }
}

static inline void stage_draw_bg(const int cam_vx, const int cam_vy) {
  switch (stage_data->bk_type) {
    case BACKGROUND_TYPE_STATIONARY:
      stage_draw_bg_grid(0, 0);
      break;

    case BACKGROUND_TYPE_MOVE_DISTANT:
      stage_draw_bg_grid(-((cam_vx / 2) % rc_back[0].r.w), -((cam_vy / 2) % rc_back[0].r.h));
      break;

    case BACKGROUND_TYPE_MOVE_NEAR:
      stage_draw_bg_grid(-(cam_vx % rc_back[0].r.w), -(cam_vy % rc_back[0].r.h));
      break;

    case BACKGROUND_TYPE_AUTOSCROLL:
      stage_draw_bg_grid(-(TO_INT(bg_x) % rc_back[0].r.w), -rc_back[0].r.h);
      break;

    case BACKGROUND_TYPE_CLOUDS:
    case BACKGROUND_TYPE_CLOUDS_WINDY:
      stage_draw_bg_clouds(cam_vx, cam_vy);
      break;
  }
}

static inline void stage_draw_fg(const int cam_vx, const int cam_vy) {
  if (stage_data->bk_type != BACKGROUND_TYPE_WATER)
    return;
  const int x1 = cam_vx / 32;
  const int x2 = x1 + (((VID_WIDTH + (32 - 1)) / 32) + 1);
  const int vwatery = TO_INT(stage_water_y) - cam_vy;
  for (int y = 0; y < 32; ++y) {
    const int ypos = y * 32 + vwatery;
    if (ypos < -32) continue;
    if (ypos > VID_HEIGHT) break;
    for (int x = x1; x < x2; ++x) {
      const int xpos = x * 32 - cam_vx;
      gfx_draw_texrect(&rc_back[1], GFX_LAYER_FRONT, xpos, ypos);
      if (y == 0) gfx_draw_texrect(&rc_back[0], GFX_LAYER_FRONT, xpos, ypos);
    }
  }
}

static inline void stage_draw_current(const int atrb, const int x, const int y) {
  const int idx = wind_count % 16;
  if (atrb < 0x84)
    gfx_draw_texrect_16x16(&rc_wind_tile[atrb - 0x80][idx], GFX_LAYER_FRONT, x, y);
  else if (atrb >= 0xA0 && atrb < 0xA4)
    gfx_draw_texrect_16x16(&rc_wind_tile[atrb - 0xA0][idx], GFX_LAYER_FRONT, x, y);
}

static inline void stage_draw_map(int cam_vx, int cam_vy) {
  const int start_tx = (cam_vx + TILE_SIZE / 2) >> TILE_SHIFT;
  const int start_ty = (cam_vy + TILE_SIZE / 2) >> TILE_SHIFT;
  const int end_tx = start_tx + ((VID_WIDTH + TILE_SIZE - 1) >> TILE_SHIFT) + 1;
  const int end_ty = start_ty + ((VID_HEIGHT + TILE_SIZE - 1) >> TILE_SHIFT) + 1;

  const u32 stride = stage_data->width;
  const u8 *line = &stage_data->map_data[start_ty * stride + start_tx];
  const u8 *ptr;
  u8 tile, atrb;
  int tx, ty;

  // tile rendering is centered
  cam_vx += TILE_SIZE / 2;
  cam_vy += TILE_SIZE / 2;

  for (ty = start_ty; ty < end_ty; ++ty, line += stride) {
    ptr = line;
    for (tx = start_tx; tx < end_tx; ++tx, ++ptr) {
      tile = *ptr;
      if (tile) {
        atrb = stage_data->atrb[tile];
        if (atrb == 0x43)
          gfx_draw_texrect_16x16(&rc_snack_tile, GFX_LAYER_FRONT, (tx << 4) - cam_vx, (ty << 4) - cam_vy);
        else if ((atrb < 0x20) || (atrb >= 0x40 && atrb < 0x80))
          gfx_draw_tile(tile & 0xF, tile >> 4, (atrb >= 0x40), (tx << 4) - cam_vx, (ty << 4) - cam_vy);
        else if (atrb >= 0x80)
          stage_draw_current(atrb, (tx << 4) - cam_vx, (ty << 4) - cam_vy);
      }
    }
  }
}

void stage_draw(int cam_x, int cam_y) {
  cam_x = TO_INT(cam_x);
  cam_y = TO_INT(cam_y);
  stage_draw_bg(cam_x, cam_y);
  stage_draw_map(cam_x, cam_y);
  stage_draw_fg(cam_x, cam_y);
}

int stage_set_tile(const int x, const int y, const int t) {
  const int idx = y * stage_data->width + x;
  if (stage_data->map_data[idx] != t) {
    stage_data->map_data[idx] = t;
    for (int i = 0; i < 3; ++i)
      npc_spawn(NPC_SMOKE, TO_FIX(x) * TILE_SIZE, TO_FIX(y) * TILE_SIZE, 0, 0, 0, NULL, 0);
    return TRUE;
  }
  return FALSE;
}
