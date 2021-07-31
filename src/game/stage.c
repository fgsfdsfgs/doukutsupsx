#include <stdio.h>

#include "engine/common.h"
#include "engine/memory.h"
#include "engine/graphics.h"
#include "engine/sound.h"

#include "game/game.h"
#include "game/player.h"
#include "game/camera.h"
#include "game/textscript.h"
#include "game/npc.h"
#include "game/stage.h"

// pointer to current stage
stage_t *stage_data;
// water level
int stage_water_y = TO_FIX(240 * 0x10);

// currently loaded stage bank
static stage_bank_t *stage_bank;
// pointers to loaded stages indexed by id, or NULL if unloaded
static stage_t *stages[MAX_STAGES];

// path to current bank
static char bank_path[CD_MAX_PATH] = STAGE_PATH_PREFIX STAGE_PATH_FORMAT;

// rect for that one stupid tile in NpcSym
static gfx_texrect_t rc_snack_tile = { 256, 48, 272, 64 };

void stage_init(void) {
  // the surface should be already loaded by now
  if (rc_snack_tile.tpage == 0)
    gfx_set_texrect(&rc_snack_tile, SURFACE_ID_NPC_SYM);
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
    panic("could not load stage bank\n%s", bank_path);

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

  mem_free(stage_bank);
  stage_bank = NULL;
}

int stage_transition(const u32 id, const u32 event, int plr_x, int plr_y) {
  if (!stages[id]) {
    stage_free_stage_bank();
    stage_load_stage_bank(id);
  }

  ASSERT(stages[id]);

  stage_data = stages[id];
  tsc_data = (char *)stage_data->map_data + stage_data->tsc_offset;

  npc_parse_event_list((stage_event_t *)&stage_data->map_data[stage_data->ev_offset], stage_data->ev_count);

  plr_set_pos(TO_FIX(plr_x) * TILE_SIZE, TO_FIX(plr_y) * TILE_SIZE);
  cam_center_on_player();

  game_tick = 0;

  printf("set stage %u (%p %p): %ux%u ofs %u %u\n", stage_data->id, stage_data, stage_data->map_data,
    stage_data->width, stage_data->height, stage_data->ev_offset, stage_data->tsc_offset);
  printf("tileset (%d, %d, %02x, %04x)\n", gfx_surf[2].tex_x, gfx_surf[2].tex_y, gfx_surf[2].mode, gfx_surf[2].clut);

  return true;
}

void stage_draw(int cam_x, int cam_y) {
  cam_x = TO_INT(cam_x);
  cam_y = TO_INT(cam_y);

  const int start_tx = cam_x >> TILE_SHIFT;
  const int start_ty = cam_y >> TILE_SHIFT;
  const int end_tx = start_tx + (VID_WIDTH >> TILE_SHIFT);
  const int end_ty = start_ty + (VID_HEIGHT >> TILE_SHIFT);

  const u32 stride = stage_data->width;
  const u8 *line = &stage_data->map_data[start_ty * stride + start_tx];
  const u8 *ptr;
  u8 tile, atrb;
  int tx, ty;

  for (ty = start_ty; ty <= end_ty; ++ty, line += stride) {
    ptr = line;
    for (tx = start_tx; tx <= end_tx; ++tx, ++ptr) {
      tile = *ptr;
      atrb = stage_data->atrb[tile];
      if (atrb == 0x43)
        gfx_draw_texrect_16x16(&rc_snack_tile, GFX_LAYER_FRONT, (tx << 4) - cam_x, (ty << 4) - cam_y);
      else if (atrb < 0x20 || (atrb >= 0x40 && atrb < 0x80))
        gfx_draw_tile(tile & 0xF, tile >> 4, (atrb >= 0x40), (tx << 4) - cam_x, (ty << 4) - cam_y);
    }
  }
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
