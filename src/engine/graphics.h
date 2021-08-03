#pragma once

#include <sys/types.h>
#include <psxgpu.h>

#include "engine/filesystem.h"
#include "engine/surfacelist.h"

#define GFX_MAX_SURFACES 40
#define GFX_MAIN_BANK "\\MAIN\\MAIN.GFX;1"
#define GFX_FONT_WIDTH 6
#define GFX_FONT_HEIGHT 12

#define GFX_RGB(r, g, b) ((((b) >> 3) << 10) | (((g) >> 3) << 5) | ((r) >> 3))

// these are mostly for tile drawing
enum gfx_layer {
  GFX_LAYER_BACK,
  GFX_LAYER_FRONT,
  GFX_NUM_LAYERS
};

#pragma pack(push, 1)

typedef struct {
  s8 id;     // in-game surface id (or -1 if unloaded)
  u8 mode;   // 1 for 256-color and 0 for 16-color
  u16 clut;  // CLUT address as given by vram_fit_clut()
  u16 tex_x; // absolute X of top left corner in VRAM
  u16 tex_y; // absolute Y of top left corner in VRAM
} gfx_surf_t;

typedef struct {
  u16 numsurf; // total number of surfaces in bank
  u16 numclut; // total number of CLUTs in bank
  RECT clut_rect[2]; // where to copy the data for each CLUT page
  RECT surf_rect[2]; // where to copy the data for each surface page
  gfx_surf_t surf[]; // [numsurf] surface headers
  // [clutdata_h * VRAM_CLUT_PAGE_WIDTH] words of clut data follows
  // [surfdata_h * VRAM_PAGE_WIDTH] words of surface data follows
} gfx_bank_t;

#pragma pack(pop)

extern gfx_surf_t gfx_surf[];

typedef struct {
  rect_t r;  // rect inside surface
  u32 surf;  // surface ID
  u16 tpage; // getTPage(rect.x, rect.y)
  u8 u;      // starting UV inside surface
  u8 v;
} gfx_texrect_t;

int gfx_init(void);
void gfx_init_fonts(void);

// loads a surface bank into a gfx_bank_t struct and uploads it to VRAM
int gfx_upload_gfx_bank(gfx_bank_t *bank, u8 *bank_data);
int gfx_read_gfx_bank(fs_file_t *f);
int gfx_load_gfx_bank(const char *path);

void gfx_swap_buffers(void);
void gfx_draw_texrect(const gfx_texrect_t *texrect, const int layer, const int x, const int y);
void gfx_draw_texrect_16x16(const gfx_texrect_t *texrect, const int layer, const int x, const int y);
void gfx_draw_tile(u8 tile_x, u8 tile_y, const int layer, const int x, const int y);
void gfx_draw_string(const char *str, const int layer, int x, int y);
void gfx_draw_fillrect(const u8 *rgba, const int layer, const int x, const int y, const int w, const int h);

// converts the `rect` field of `r` into tpage address, UVs and XYWH instead of LTRB
static inline void gfx_set_texrect(gfx_texrect_t *tr, const int s) {
  const int mode = gfx_surf[s].mode;
  const int shift = 2 - mode;
  const int tpage_x = (gfx_surf[s].tex_x + (tr->r.x >> shift)) & 0x3C0;
  const int tpage_y = (gfx_surf[s].tex_y + tr->r.y) & 0x100;
  // align up to 16-bit pixel
  tr->r.w = tr->r.right - tr->r.left;
  tr->r.h = tr->r.bottom - tr->r.top;
  tr->surf = s;
  tr->tpage = getTPage(mode, 0, tpage_x, tpage_y);
  tr->u = ((gfx_surf[s].tex_x - tpage_x) << shift) + tr->r.x;
  tr->v = (gfx_surf[s].tex_y - tpage_y) + tr->r.y;
}
