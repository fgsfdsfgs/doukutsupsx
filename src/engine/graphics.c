#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <libetc.h>
#include <libgte.h>
#include <libgpu.h>

#include "engine/common.h"
#include "engine/memory.h"
#include "engine/filesystem.h"
#include "engine/graphics.h"

// block fill primitive
// taken from psn00bsdk
typedef struct _FILL {
  u_long tag;
  u_char r0, g0, b0, code;
  u_short x0, y0;
  u_short w, h;
} FILL;

#define setFill(p) setlen(p, 3), setcode(p, 0x02)

// shut up compiler
#undef termPrim
#define termPrim(p) setaddr(p, 0xFFFFFF)

#define GPU_BUF_SIZE 32768

// dimensions of free vram (as in, space to the right of the two framebuffers)
#define VRAM_XSTART      VID_WIDTH
#define VRAM_NUM_PAGES   2
#define VRAM_PAGE_WIDTH  (1024 - VRAM_XSTART)
#define VRAM_PAGE_HEIGHT 256

// CLUT pages are in between the two framebuffers
#define VRAM_NUM_CLUT_PAGES   2
#define VRAM_CLUT_PAGE_WIDTH  VID_WIDTH
#define VRAM_CLUT_PAGE_HEIGHT (VRAM_PAGE_HEIGHT - VID_HEIGHT)
#define VRAM_CLUT_PAGE_Y(n)   (VID_HEIGHT + (n)*VRAM_PAGE_HEIGHT)

// since I want to draw back to front without having giant OTs, this will have to do
typedef struct  {
  u32 first;
  u8 *last;
} plist_t;

// currently loaded surfaces
gfx_surf_t gfx_surf[GFX_MAX_SURFACES];

// common clear color
const u8 gfx_clear_rgb[3] = { 0x00, 0x00, 0x20 };

// when TRUE, don't show loading screens
bool gfx_suppress_loading = FALSE;

static struct fb
{
  DRAWENV draw;
  DISPENV disp;
  plist_t primlist[GFX_NUM_LAYERS];
  u8 gpubuf[GPU_BUF_SIZE];
  u8 *gpubuf_end;
} fb[2];

static struct fb *cur_fb;
static int cur_fb_num;

static u8 *primptr;
static plist_t *primlist;
static u16 cur_tpage[GFX_NUM_LAYERS];

// texrects for fonts
static gfx_texrect_t fnt_rect;

// texrect for the currently loaded tileset
static gfx_texrect_t tiles_rect;

// "LOADING" icon; embedded into the executable so I don't need to load the loading screen
static u16 img_loading[] = {
#include "engine/loading.inc"
};
static const u32 img_loading_w = 64;
static const u32 img_loading_h = 8;

static inline void plist_append(plist_t *list, const u32 size) {
  if (!list->last)
    catPrim(&list->first, primptr);
  else
    catPrim(list->last, primptr);
  list->last = primptr;
  primptr += size;
}

static inline void plist_clear(plist_t *list) {
  list->first = 0xFFFFFF;
  list->last = NULL;
}

int gfx_init(void) {
  ResetGraph(3);

  // set up double buffer
  SetDefDispEnv(&fb[0].disp, 0,   0, VID_WIDTH, VID_HEIGHT);
  SetDefDrawEnv(&fb[1].draw, 0,   0, VID_WIDTH, VID_HEIGHT);
  SetDefDispEnv(&fb[1].disp, 0, 256, VID_WIDTH, VID_HEIGHT);
  SetDefDrawEnv(&fb[0].draw, 0, 256, VID_WIDTH, VID_HEIGHT);

  // add Y offset if we're running in PAL
  if (GetVideoMode() == MODE_PAL) {
    fb[0].disp.screen.y = 24;
    fb[1].disp.screen.y = 24;
  }

  for (int i = 0; i < 2; ++i) {
    fb[i].gpubuf_end = fb[i].gpubuf + sizeof(fb[i].gpubuf);
    fb[i].draw.isbg = 1;
    fb[i].draw.r0 = gfx_clear_rgb[0];
    fb[i].draw.g0 = gfx_clear_rgb[1];
    fb[i].draw.b0 = gfx_clear_rgb[2];
  }

  // clear screen ASAP
  // need two FILL primitives because h=512 doesn't work correctly
  FILL fill = { 0 };
  setFill(&fill);
  fill.w = 512;
  fill.h = 256;
  DrawPrim(&fill);
  fill.y0 = 256;
  DrawPrim(&fill);
  DrawSync(0);

  cur_fb_num = 0;
  cur_fb = &fb[cur_fb_num];
  cur_tpage[GFX_LAYER_BACK] = cur_tpage[GFX_LAYER_FRONT] = cur_fb->draw.tpage;

  primptr = cur_fb->gpubuf;
  primlist = cur_fb->primlist;
  plist_clear(&primlist[GFX_LAYER_BACK]);
  plist_clear(&primlist[GFX_LAYER_FRONT]);

  // set default front and work buffer
  PutDispEnv(&cur_fb->disp);
  PutDrawEnv(&cur_fb->draw);

  // mark all surfaces as unloaded
  for (int i = 0; i < GFX_MAX_SURFACES; ++i)
    gfx_surf[i].id = -1;

  // draw loading screen
  gfx_draw_loading();

  // enable display
  SetDispMask(1);

  return TRUE;
}

void gfx_swap_buffers(void) {
  DrawSync(0);
  VSync(0);

  // change to new display area first
  cur_fb_num ^= 1;
  PutDispEnv(&fb[cur_fb_num].disp);
  PutDrawEnv(&fb[cur_fb_num].draw);

  // then start drawing the old prim queue
  if (primptr != cur_fb->gpubuf) {
    ASSERT(primptr <= cur_fb->gpubuf_end);
    if (primlist[GFX_LAYER_BACK].last)
      catPrim(primlist[GFX_LAYER_BACK].last, &primlist[GFX_LAYER_FRONT].first);
    if (primlist[GFX_LAYER_FRONT].last)
      termPrim(primlist[GFX_LAYER_FRONT].last);
    DrawOTag((u_long *)&primlist[GFX_LAYER_BACK].first);
  }

  // now we can switch the fb pointer
  cur_fb = &fb[cur_fb_num];
  cur_tpage[GFX_LAYER_BACK] = cur_tpage[GFX_LAYER_FRONT] = cur_fb->draw.tpage;

  primptr = cur_fb->gpubuf;
  primlist = cur_fb->primlist;
  plist_clear(&primlist[GFX_LAYER_BACK]);
  plist_clear(&primlist[GFX_LAYER_FRONT]);
}

static inline void gfx_update_tpage(const int layer, const u16 tpage) {
  if (cur_tpage[layer] != tpage) {
    cur_tpage[layer] = tpage;
    DR_TPAGE *tp = (DR_TPAGE *)primptr;
    setDrawTPage(tp, 1, 0, tpage);
    plist_append(&primlist[layer], sizeof(*tp));
  }
}

void gfx_draw_texrect(const gfx_texrect_t *texrect, const int layer, const int x, const int y) {
  // SPRTs have no tpage field, so we have to make do
  gfx_update_tpage(layer, texrect->tpage);
  SPRT *prim = (SPRT *)primptr;
  setSprt(prim);
  setRGB0(prim, 0x80, 0x80, 0x80);
  setXY0(prim, x, y);
  setUV0(prim, texrect->u, texrect->v);
  setWH(prim, texrect->r.w, texrect->r.h);
  prim->clut = gfx_surf[texrect->surf].clut;
  plist_append(&primlist[layer], sizeof(*prim));
}

void gfx_draw_texrect_wide(const gfx_texrect_t *texrect, const int layer, const int x, const int y) {
  int u0 = texrect->u;
  int w0 = texrect->r.w;
  const int v0 = texrect->v;
  const int h0 = texrect->r.h;
  const u16 tpage0 = texrect->tpage;
  int w1 = 0;
  if (u0 + w0 > 256) {
    // texrect intersects texpage boundary
    const int wt = 256 - u0;
    w1 = w0 - wt;
    w0 = wt;
  }

  // draw first half
  gfx_update_tpage(layer, tpage0);
  SPRT *prim = (SPRT *)primptr;
  setSprt(prim);
  setRGB0(prim, 0x80, 0x80, 0x80);
  setXY0(prim, x, y);
  setUV0(prim, u0, v0);
  setWH(prim, w0, h0);
  prim->clut = gfx_surf[texrect->surf].clut;
  plist_append(&primlist[layer], sizeof(*prim));

  // draw second half if required
  if (w1) {
    const int x1 = x + w0;
    if (x1 < VID_WIDTH) {
      // advance tpage by 1 to the right
      gfx_update_tpage(layer, tpage0 + 1);
      prim = (SPRT *)primptr;
      setSprt(prim);
      setRGB0(prim, 0x80, 0x80, 0x80);
      setXY0(prim, x + w0, y);
      setUV0(prim, 0, v0);
      setWH(prim, w1, h0);
      prim->clut = gfx_surf[texrect->surf].clut;
      plist_append(&primlist[layer], sizeof(*prim));
    }
  }
}

void gfx_draw_texrect_scaled(const gfx_texrect_t *texrect, const int layer, int x, int y, const int scale) {
  const int w = texrect->r.w;
  const int h = texrect->r.h;
  const int sw = w * scale;
  const int sh = h * scale;
  const int u0 = texrect->u;
  const int v0 = texrect->v;
  POLY_FT4 *prim = (POLY_FT4 *)primptr;
  setPolyFT4(prim);
  setRGB0(prim, 0x80, 0x80, 0x80);
  setXY4(prim, x, y, x + sw, y, x, y + sh, x + sw, y + sh);
  setUV4(prim, u0, v0, u0 + w, v0, u0, v0 + h, u0 + w, v0 + h);
  prim->clut = gfx_surf[texrect->surf].clut;
  prim->tpage = texrect->tpage;
  plist_append(&primlist[layer], sizeof(*prim));
}

void gfx_draw_texrect_ofs(const gfx_texrect_t *texrect, const int layer, const int x, const int y, const int du, const int dv) {
  // SPRTs have no tpage field, so we have to make do
  gfx_update_tpage(layer, texrect->tpage);
  SPRT *prim = (SPRT *)primptr;
  setSprt(prim);
  setRGB0(prim, 0x80, 0x80, 0x80);
  setXY0(prim, x, y);
  setUV0(prim, texrect->u + du, texrect->v + dv);
  setWH(prim, texrect->r.w - du, texrect->r.h - dv);
  prim->clut = gfx_surf[texrect->surf].clut;
  plist_append(&primlist[layer], sizeof(*prim));
}

void gfx_draw_texrect_16x16(const gfx_texrect_t *texrect, const int layer, const int x, const int y) {
  // SPRTs have no tpage field, so we have to make do
  gfx_update_tpage(layer, texrect->tpage);
  SPRT_16 *prim = (SPRT_16 *)primptr;
  setSprt16(prim);
  setRGB0(prim, 0x80, 0x80, 0x80);
  setXY0(prim, x, y);
  setUV0(prim, texrect->u, texrect->v);
  prim->clut = gfx_surf[texrect->surf].clut;
  plist_append(&primlist[layer], sizeof(*prim));
}

// for hud numbers
void gfx_draw_texrect_8x8(const gfx_texrect_t *texrect, const int layer, const int x, const int y) {
  // SPRTs have no tpage field, so we have to make do
  gfx_update_tpage(layer, texrect->tpage);
  SPRT_8 *prim = (SPRT_8 *)primptr;
  setSprt8(prim);
  setRGB0(prim, 0x80, 0x80, 0x80);
  setXY0(prim, x, y);
  setUV0(prim, texrect->u, texrect->v);
  prim->clut = gfx_surf[texrect->surf].clut;
  plist_append(&primlist[layer], sizeof(*prim));
}

// tiles always start at the top left of a texture page, so no need to figure them out
// they are also at most 256x256 texels wide in total
void gfx_draw_tile(u8 tile_x, u8 tile_y, const int layer, const int x, const int y) {
  // SPRTs have no tpage field, so we have to make do
  gfx_update_tpage(layer, tiles_rect.tpage);
  SPRT_16 *prim = (SPRT_16 *)primptr;
  setSprt16(prim);
  setRGB0(prim, 0x80, 0x80, 0x80);
  setSemiTrans(prim, 0);
  setXY0(prim, x, y);
  setUV0(prim, tile_x << 4, tiles_rect.v + (tile_y << 4));
  prim->clut = gfx_surf[SURFACE_ID_LEVEL_TILESET].clut;
  plist_append(&primlist[layer], sizeof(*prim));
}

void gfx_draw_string(const char *str, const int layer, int x, int y) {
  // SPRTs have no tpage field, so we have to make do
  gfx_update_tpage(layer, fnt_rect.tpage);
  for (const char *ch = str; *ch; ++ch, x += GFX_FONT_WIDTH) {
    const int n = *ch - ' ';
    SPRT *prim = (SPRT *)primptr;
    setSprt(prim);
    setRGB0(prim, 0x80, 0x80, 0x80);
    setSemiTrans(prim, 0);
    setXY0(prim, x, y);
    setUV0(prim, fnt_rect.u + (n & 0x0F) * GFX_FONT_WIDTH, fnt_rect.v + (n >> 4) * GFX_FONT_HEIGHT);
    setWH(prim, GFX_FONT_WIDTH, GFX_FONT_HEIGHT);
    prim->clut = gfx_surf[SURFACE_ID_FONT1].clut;
    plist_append(&primlist[layer], sizeof(*prim));
  }
}

void gfx_draw_string_rgb(const char *str, const u8 *rgb, const int layer, int x, int y) {
  // SPRTs have no tpage field, so we have to make do
  gfx_update_tpage(layer, fnt_rect.tpage);
  for (const char *ch = str; *ch; ++ch, x += GFX_FONT_WIDTH) {
    const int n = *ch - ' ';
    SPRT *prim = (SPRT *)primptr;
    setSprt(prim);
    setSemiTrans(prim, 0);
    setRGB0(prim, rgb[0] >> 1, rgb[1] >> 1, rgb[2] >> 1);
    setXY0(prim, x, y);
    setUV0(prim, fnt_rect.u + (n & 0x0F) * GFX_FONT_WIDTH, fnt_rect.v + (n >> 4) * GFX_FONT_HEIGHT);
    setWH(prim, GFX_FONT_WIDTH, GFX_FONT_HEIGHT);
    prim->clut = gfx_surf[SURFACE_ID_FONT1].clut;
    plist_append(&primlist[layer], sizeof(*prim));
  }
}

void gfx_draw_clear(const u8 *rgb, const int layer) {
  FILL *prim = (FILL *)primptr;
  const RECT *curclip = &fb[!cur_fb_num].draw.clip;
  setFill(prim);
  setRGB0(prim, rgb[0], rgb[1], rgb[2]);
  setXY0(prim, curclip->x, curclip->y);
  setWH(prim, curclip->w, curclip->h);
  plist_append(&primlist[layer], sizeof(*prim));
}

void gfx_draw_fillrect(const u8 *rgb, const int layer, const int x, const int y, const int w, const int h) {
  TILE *prim = (TILE *)primptr;
  setTile(prim);
  setRGB0(prim, rgb[0], rgb[1], rgb[2]);
  setXY0(prim, x, y);
  setWH(prim, w, h);
  plist_append(&primlist[layer], sizeof(*prim));
}

void gfx_push_cliprect(const int layer, const int x, const int y, const int w, const int h) {
  DR_AREA *prim = (DR_AREA *)primptr;
  const RECT *curclip = &fb[!cur_fb_num].draw.clip;
  RECT tmprect = { x + curclip->x, y + curclip->y, w, h };
  SetDrawArea(prim, &tmprect);
  plist_append(&primlist[layer], sizeof(*prim));
}

void gfx_pop_cliprect(const int layer) {
  DR_AREA *prim = (DR_AREA *)primptr;
  SetDrawArea(prim, &fb[!cur_fb_num].draw.clip);
  plist_append(&primlist[layer], sizeof(*prim));
}

void gfx_draw_clear_immediate(const u8 *rgb) {
  FILL fill;
  setFill(&fill);
  setRGB0(&fill, rgb[0], rgb[1], rgb[2]);
  setXY0(&fill, fb[0].draw.clip.x, fb[0].draw.clip.y);
  setWH(&fill, fb[0].draw.clip.w, fb[0].draw.clip.h);
  DrawPrim(&fill);
  setXY0(&fill, fb[1].draw.clip.x, fb[1].draw.clip.y);
  DrawPrim(&fill);
}

void gfx_draw_loading(void) {
  if (gfx_suppress_loading)
    return;
  // clear both framebuffers
  gfx_draw_clear_immediate(gfx_clear_rgb);
  // just plop it into the currently shown framebuffer
  const RECT *curclip = &fb[!cur_fb_num].draw.clip;
  RECT rc = {
    curclip->x + VID_WIDTH - 88,
    curclip->y + VID_HEIGHT - 32,
    img_loading_w,
    img_loading_h
  };
  LoadImage(&rc, (u_long *)img_loading);
}

void gfx_upload_image(u8 *data, int w, int h, const int mode, const int surf_id, const bool sync) {
  const int shift = 2 - mode;
  w >>= shift;

  RECT rect = { 0, 0, w, h };
  if (gfx_surf[surf_id].tex_x || gfx_surf[surf_id].tex_y) {
    // there's already a surface with this ID, overwrite it
    rect.x = gfx_surf[surf_id].tex_x;
    rect.y = gfx_surf[surf_id].tex_y;
  } else {
    // slam it into the bottom right corner and hope it doesn't overwrite anything
    rect.x = 1024 - ALIGN(w, 16);
    rect.y = 512 - h - 1;
  }

  if (mode == 0) {
    // 16 colors; CLUT is safely underneath image
    ++rect.h;
    gfx_surf[surf_id].clut = getClut(rect.x, rect.y + h);
  } else if (mode == 1) {
    // 256 colors; CLUT is too big for this shit, we have to do a separate upload
    // shove it into the bottommost line under the framebuffer
    RECT clutrect = { 0, 511, 256, 1 };
    u8 *clutptr = data + (rect.w * rect.h * 2);
    LoadImage(&clutrect, (u_long *)clutptr);
    gfx_surf[surf_id].clut = getClut(clutrect.x, clutrect.y);
  }

  gfx_surf[surf_id].mode = mode;
  gfx_surf[surf_id].id = surf_id;
  gfx_surf[surf_id].tex_x = rect.x;
  gfx_surf[surf_id].tex_y = rect.y;
  LoadImage(&rect, (u_long *)data);
  if (sync) DrawSync(0);
}

int gfx_upload_gfx_bank(gfx_bank_t *bank, u8 *bank_data) {
  if (!bank_data)
    bank_data = (u8 *)bank + sizeof(*bank) + sizeof(gfx_surf_t) * bank->numsurf;

  // remap surfaces
  for (u32 i = 0; i < bank->numsurf; ++i)
    gfx_surf[bank->surf[i].id] = bank->surf[i];

  const u32 clut_size[2] = {
    2 * bank->clut_rect[0].w * bank->clut_rect[0].h,
    2 * bank->clut_rect[1].w * bank->clut_rect[1].h,
  };
  const u32 surf_size[2] = {
    2 * bank->surf_rect[0].w * bank->surf_rect[0].h,
    2 * bank->surf_rect[1].w * bank->surf_rect[1].h,
  };

  const u32 total_clut_size = clut_size[0] + clut_size[1];

  if (clut_size[0])
    LoadImage(&bank->clut_rect[0], (u_long *)bank_data);
  if (clut_size[1])
    LoadImage(&bank->clut_rect[1], (u_long *)(bank_data + clut_size[0]));

  if (surf_size[0])
    LoadImage(&bank->surf_rect[0], (u_long *)(bank_data + total_clut_size));
  if (surf_size[1])
    LoadImage(&bank->surf_rect[1], (u_long *)(bank_data + total_clut_size + surf_size[0]));

  DrawSync(0);

  // save tileset metadata for later
  gfx_set_texrect(&tiles_rect, SURFACE_ID_LEVEL_TILESET);

  return TRUE;
}

// gonna leave most of _upload_ duplicated here for now
int gfx_read_gfx_bank(fs_file_t *f) {
  gfx_bank_t *bank = mem_alloc(sizeof(*bank) + sizeof(gfx_surf_t) * GFX_MAX_SURFACES);
  fs_fread_or_die(bank, sizeof(*bank), 1, f);
  fs_fread_or_die(bank->surf, sizeof(gfx_surf_t) * bank->numsurf, 1, f);

  // remap surfaces
  for (u32 i = 0; i < bank->numsurf; ++i)
    gfx_surf[bank->surf[i].id] = bank->surf[i];

  const u32 clut_size[2] = {
    2 * bank->clut_rect[0].w * bank->clut_rect[0].h,
    2 * bank->clut_rect[1].w * bank->clut_rect[1].h,
  };
  const u32 surf_size[2] = {
    2 * bank->surf_rect[0].w * bank->surf_rect[0].h,
    2 * bank->surf_rect[1].w * bank->surf_rect[1].h,
  };

  const u32 total_clut_size = clut_size[0] + clut_size[1];
  const u32 total_surf_size = surf_size[0] + surf_size[1];

  // if this turns out to be too much for our poor memory,
  // we'll load CLUTs first and then the surfaces
  u8 *buf = mem_alloc(total_surf_size + total_clut_size);
  fs_fread_or_die(buf, total_clut_size + total_surf_size, 1, f);

  if (clut_size[0])
    LoadImage(&bank->clut_rect[0], (u_long *)buf);
  if (clut_size[1])
    LoadImage(&bank->clut_rect[1], (u_long *)(buf + clut_size[0]));

  if (surf_size[0])
    LoadImage(&bank->surf_rect[0], (u_long *)(buf + total_clut_size));
  if (surf_size[1])
    LoadImage(&bank->surf_rect[1], (u_long *)(buf + total_clut_size + surf_size[0]));

  DrawSync(0);

  mem_free(buf);
  mem_free(bank);

  return TRUE;
}

int gfx_load_gfx_bank(const char *path) {
  fs_file_t *f = fs_fopen(path, 0);
  if (!f) PANIC("could not open\n%s", path);
  const int ret = gfx_read_gfx_bank(f);
  fs_fclose(f);
  return ret;
}

void gfx_init_fonts(void) {
  fnt_rect.r.x = 0;
  fnt_rect.r.y = 0;
  fnt_rect.r.w = GFX_FONT_WIDTH;
  fnt_rect.r.h = GFX_FONT_HEIGHT;
  gfx_set_texrect(&fnt_rect, SURFACE_ID_FONT1);
}
