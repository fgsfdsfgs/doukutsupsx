#pragma once

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

#include "common.h"

#define MAX_SURFACES 40

// dimensions of the framebuffers
#define VRAM_FB_WIDTH  320
#define VRAM_FB_HEIGHT 240
// dimensions of entire vram
#define VRAM_WIDTH  1024
#define VRAM_HEIGHT 512
// psx texpage dimensions
#define VRAM_TEXPAGE_WIDTH 64
#define VRAM_TEXPAGE_HEIGHT 256
// dimensions of free vram (as in, space to the right of the two framebuffers)
#define VRAM_XSTART       VRAM_FB_WIDTH
#define VRAM_NUM_PAGES    2
#define VRAM_PAGE_WIDTH   (1024 - VRAM_XSTART)
#define VRAM_PAGE_HEIGHT  VRAM_TEXPAGE_HEIGHT
#define VRAM_PAGES_WIDTH  VRAM_PAGE_WIDTH
#define VRAM_PAGES_HEIGHT (VRAM_NUM_PAGES * VRAM_PAGE_HEIGHT)
// CLUT pages are in between the two framebuffers
#define VRAM_NUM_CLUT_PAGES   2
#define VRAM_CLUT_PAGE_WIDTH  VRAM_FB_WIDTH
#define VRAM_CLUT_PAGE_HEIGHT (VRAM_PAGE_HEIGHT - VRAM_FB_HEIGHT)
#define VRAM_CLUT_PAGE_SIZE   (VRAM_CLUT_PAGE_WIDTH * VRAM_CLUT_PAGE_HEIGHT)
#define VRAM_CLUT_PAGE_Y(n)   (VRAM_FB_HEIGHT + (n)*VRAM_PAGE_HEIGHT)

#define PSXRGB(r, g, b) ((((b) >> 3) << 10) | (((g) >> 3) << 5) | ((r) >> 3))
#define PSXTPAGE(tp, abr, x, y) ((((x)&0x3FF)>>6) | (((y)>>8)<<4) | (((abr)&0x3)<<5) | (((tp)&0x3)<<7))
#define PSXCLUT(x, y) (((y)<<6) | (((x)>>4)&0x3F))

#define PSXRED(c)   (((c >>  0) & 0x1F) << 3)
#define PSXGREEN(c) (((c >>  5) & 0x1F) << 3)
#define PSXBLUE(c)  (((c >> 10) & 0x1F) << 3)

#pragma pack(push, 1)

typedef struct {
  int8_t id;      // in-game surface id (or -1 if unloaded)
  uint8_t mode;   // 1 for 256-color and 0 for 16-color
  uint16_t clut;  // CLUT address as given by vram_fit_clut()
  uint16_t tex_x; // absolute X of top left corner in VRAM
  uint16_t tex_y; // absolute Y of top left corner in VRAM
} vram_surf_t;

typedef struct {
  int16_t x;
  int16_t y;
  int16_t w;
  int16_t h;
} vram_rect_t;

typedef struct {
  uint16_t numsurf;                      // total number of surfaces in bank
  uint16_t numclut;                      // total number of CLUTs in bank
	vram_rect_t clut_rect[VRAM_NUM_PAGES]; // where to copy the data for each CLUT page
	vram_rect_t surf_rect[VRAM_NUM_PAGES]; // where to copy the data for each surface page
  vram_surf_t surf[];                    // [numsurf] surface headers
  // [clutdata_h * VRAM_CLUT_PAGE_WIDTH] words of clut data follows
  // [surfdata_h * VRAM_PAGE_WIDTH] words of surface data follows
} vram_surfbank_t;

#pragma pack(pop)

// returns CLUT address (result of getClut(clut_x, clut_y)) on success or 0 if failed
// `mode` is 1 for 256-color CLUTs and 0 for 16-color CLUTs
// `clut` is in BGRX8888 format (like in BMPs)
uint16_t vram_fit_clut(const uint8_t *clut, uint16_t mode);

// returns surface on success or NULL if failed
// `clut` is the CLUT's address in VRAM as returned by vram_fit_clut()
// `mode` is 1 for 256-color CLUTs and 0 for 16-color CLUTs
// `data` is w*h 8-bit pixels
vram_surf_t *vram_fit_surf(const struct bitmap *bmp, const uint16_t clut, const bool align);

// writes vram_surfbank_t into the file
uint32_t vram_write_surf_bank(FILE *f);

// writes current VRAM image in PNG format into the file
bool vram_export_png(const char *fname);

// mark VRAM area as off limits for CLUT data
bool vram_restrict_clut_rect(const int page, const vram_rect_t *rect);

// mark VRAM area as off limits for surface data
bool vram_restrict_surf_rect(const int page, const vram_rect_t *rect);

// get filled data area for surface page
const vram_rect_t *vram_get_filled_surf_rect(const int pg);

// get filled data area for CLUT page
const vram_rect_t *vram_get_filled_clut_rect(const int pg);

// clear vram and surface list
void vram_reset(void);
