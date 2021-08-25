#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <assert.h>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#include "common.h"
#include "vram.h"

static uint16_t clutatlas[VRAM_NUM_CLUT_PAGES][VRAM_CLUT_PAGE_SIZE];
static int clutptr[VRAM_NUM_CLUT_PAGES];
static vram_rect_t clutrect[VRAM_NUM_CLUT_PAGES];
static vram_rect_t clutrect_base[VRAM_NUM_CLUT_PAGES];
static int numcluts = 0;

static uint16_t texatlas[VRAM_PAGES_HEIGHT][VRAM_PAGES_WIDTH];
static uint8_t texbitmap[VRAM_NUM_PAGES][VRAM_PAGE_WIDTH][VRAM_PAGE_HEIGHT];
static vram_rect_t texrect[VRAM_NUM_PAGES];
static vram_rect_t texrect_base[VRAM_NUM_PAGES];

static vram_surf_t surfaces[MAX_SURFACES];
static struct surfinfo {
  int tex_w;
  int tex_h;
  int clut_x;
  int clut_y;
} surfinfo[MAX_SURFACES];
static int numsurfaces;

static inline int rect_fits(const int pg, int sx, int sy, int w, int h) {
  for (int x = sx; x < sx + w; ++x) {
    for (int y = sy; y < sy + h; ++y) {
      if (texbitmap[pg][x][y])
        return 0;
    }
  }
  return 1;
}

static inline void rect_fill(const int pg, int sx, int sy, int w, int h) {
  for (int x = sx; x < sx + w; ++x) {
    for (int y = sy; y < sy + h; ++y)
      texbitmap[pg][x][y] = 1;
  }
}

// tries to fit w x h texture into vram image
static int vram_page_fit(vram_surf_t *surf, const int pg, int w, int h, const int xalign, const int yalign) {
  int x = -1;
  int y = -1;

  // bruteforce and see if we can exactly fit anywhere
  for (int sx = 0; sx <= VRAM_PAGE_WIDTH - w && x < 0; sx += xalign) {
    for (int sy = 0; sy <= VRAM_PAGE_HEIGHT - h; sy += yalign) {
      // skip if crossing texpage boundary
      const int vrx = sx - (sx & 0x3C0);
      if (vrx + w > VRAM_TEXPAGE_WIDTH * 4)
        continue;
      if (rect_fits(pg, sx, sy, w, h)) {
        x = sx;
        y = sy;
        break;
      }
    }
  }

  if (x == -1 || y == -1)
    return -1;

  rect_fill(pg, x, y, w, h);

  if (y + h > texrect[pg].h)
    texrect[pg].h = y + h;
  if (x + w > texrect[pg].w)
    texrect[pg].w = x + w;

  x += VRAM_XSTART;
  y += VRAM_PAGE_HEIGHT * pg;

  surf->tex_x = x;
  surf->tex_y = y;

  return 0;
}

static inline void vram_transform_image(int dstx, int dsty, const struct bitmap *bmp) {
  const uint8_t *src = bmp->data;
  const int linelen = (bmp->numcolors > 16) ? bmp->width : (bmp->width + 1) >> 1;
  for (int y = dsty; y < dsty + bmp->height; ++y) {
    if (bmp->numcolors > 16) {
      memcpy(&texatlas[y][dstx], src, linelen);
    } else {
      // in BMP the leftmost pixel is in the topmost nibble because fuck you
      const uint8_t *psrc = src;
      uint8_t *pdst = (uint8_t *)&texatlas[y][dstx];
      for (int i = 0; i < linelen; ++i, ++psrc, ++pdst)
        *pdst = ((*psrc & 0xF) << 4) | (*psrc >> 4);
    }
    src += bmp->stride;
  }
}

vram_surf_t *vram_fit_surf(const struct bitmap *bmp, const uint16_t clut, const bool align) {
  if (numsurfaces == MAX_SURFACES) {
    fprintf(stderr, "vram_fit_surf(): too many surfaces\n");
    return NULL;
  }

  int mode = -1;
  if (bmp->numcolors == 16)
    mode = 0;
  else if (bmp->numcolors == 256)
    mode = 1;
  else
    return NULL;

  vram_surf_t *surf = &surfaces[numsurfaces];
  struct surfinfo *info = &surfinfo[numsurfaces];

  // texture width is halved in 8bpp mode and quartered in 4bpp mode
  // we round up to 4 to avoid fucking up alignment so it shouldn't matter much
  const int divisor = mode ? 2 : 4;
  const int w = ALIGN((bmp->width + bmp->leftpad) / divisor, 4);
  const int h = ALIGN(bmp->height, 2); // just in case

  // available VRAM is organized in two pages: first 256 lines and second 256 lines
  // try fitting into the first one, if that doesn't work, try the second one
  const uint32_t xalignment = (align) ? VRAM_TEXPAGE_WIDTH : 4;
  const uint32_t yalignment = (align) ? 32 : 2;
  for (int i = 0; i < VRAM_NUM_PAGES; ++i) {
    if (vram_page_fit(surf, i, w, h, xalignment, yalignment) == 0) {
      ++numsurfaces;
      surf->clut = clut;
      surf->mode = mode;
      // save some information for the PNG exporter
      info->clut_y = clut >> 6;
      info->clut_x = (clut & 0x3F) << 4;
      info->tex_w = bmp->width;
      info->tex_h = bmp->height;
      surf->tex_x += bmp->leftpad / divisor;
      vram_transform_image(surf->tex_x - VRAM_XSTART, surf->tex_y, bmp);
      return surf;
    }
  }

  return NULL;
}

void vram_transform_clut(uint16_t *dst, const uint8_t *src, const int len) {
  for (int i = 0; i < len; ++i, ++dst, src += 4) {
    const uint16_t color = PSXRGB(src[2], src[1], src[0]);
    // if the original color wasn't pitch black but got transformed into pitch black, make it non-transparent,
    // otherwise leave it as is since the game considers (0, 0, 0) transparent
    if (!color && (src[0] || src[1] || src[2]))
      *dst = 0x8000; // non-transparent black
    else
      *dst = color;
  }
}

uint16_t vram_fit_clut(const uint8_t *clut, uint16_t mode) {
  if (mode > 1) return 0;

  const uint16_t len = (mode == 0) ? 16 : 256;

  for (int i = 0; i < VRAM_NUM_CLUT_PAGES; ++i) {
    if (clutptr[i] + len <= VRAM_CLUT_PAGE_SIZE) {
      uint32_t x = clutptr[i] % VRAM_CLUT_PAGE_WIDTH;
      uint32_t y = clutptr[i] / VRAM_CLUT_PAGE_WIDTH;
      uint32_t w = x + len;
      if (x + len > VRAM_CLUT_PAGE_WIDTH) {
        // palette overlaps border; move it to the next line
        clutptr[i] += VRAM_CLUT_PAGE_WIDTH - x;
        ++y;
        x = 0;
        w = VRAM_CLUT_PAGE_WIDTH;
        if (clutptr[i] + len > VRAM_CLUT_PAGE_SIZE)
          return 0;
      }
      if (y + 1 > clutrect[i].h)
        clutrect[i].h = y + 1;
      if (w > clutrect[i].w)
        clutrect[i].w = w;
      ++numcluts;
      vram_transform_clut(&clutatlas[i][clutptr[i]], clut, len);
      clutptr[i] += len;
      return PSXCLUT(x, y + VRAM_CLUT_PAGE_Y(i));
    }
  }

  return 0;
}

uint32_t vram_write_surf_bank(FILE *f) {
  // write bank header
  vram_surfbank_t hdr;
  hdr.numsurf = numsurfaces;
  hdr.numclut = numcluts;
  // offset rects to point to actual VRAM areas
  hdr.clut_rect[0] = clutrect[0];
  hdr.clut_rect[0].y += VRAM_CLUT_PAGE_Y(0);
  hdr.clut_rect[1] = clutrect[1];
  hdr.clut_rect[1].y += VRAM_CLUT_PAGE_Y(1);
  hdr.surf_rect[0] = texrect[0];
  hdr.surf_rect[0].x += VRAM_XSTART;
  hdr.surf_rect[1] = texrect[1];
  hdr.surf_rect[1].x += VRAM_XSTART;
  hdr.surf_rect[1].y += VRAM_PAGE_HEIGHT;
  fwrite(&hdr, sizeof(hdr), 1, f);

  // write surface headers
  for (int i = 0; i < numsurfaces; ++i)
    fwrite(&surfaces[i], sizeof(*surfaces), 1, f);

  // write CLUT data
  for (int i = 0; i < VRAM_NUM_CLUT_PAGES; ++i) {
    const uint16_t *src = &clutatlas[i][clutrect[i].y * VRAM_CLUT_PAGE_WIDTH + clutrect[i].x];
    for (int y = 0; y < clutrect[i].h; ++y, src += VRAM_CLUT_PAGE_WIDTH)
      fwrite(src, clutrect[i].w * 2, 1, f);
  }

  // write texture data
  for (int i = 0; i < VRAM_NUM_PAGES; ++i) {
    const uint16_t *src = &texatlas[i * VRAM_PAGE_HEIGHT + texrect[i].y][texrect[i].x];
    for (int y = 0; y < texrect[i].h; ++y, src += VRAM_PAGE_WIDTH)
      fwrite(src, texrect[i].w * 2, 1, f);
  }

  return numsurfaces;
}

bool vram_export_png(const char *fname) {
  uint8_t *dst;

  // save palette data in direct mode
  uint8_t *clutrgb = calloc(1, VRAM_CLUT_PAGE_WIDTH * VRAM_HEIGHT * 3);
  if (!clutrgb) return false;

  for (int i = 0; i < VRAM_NUM_CLUT_PAGES; ++i) {
    dst = &clutrgb[VRAM_CLUT_PAGE_Y(i) * VRAM_CLUT_PAGE_WIDTH * 3];
    for (int j = 0; j < VRAM_CLUT_PAGE_SIZE; ++j, dst += 3) {
      const uint16_t c = clutatlas[i][j];
      dst[0] = PSXRED(clutatlas[i][j]);
      dst[1] = PSXGREEN(clutatlas[i][j]);
      dst[2] = PSXBLUE(clutatlas[i][j]);
    }
  }

  // save surface data in 16-color mode
  uint8_t *texrgb = calloc(1, 4 * VRAM_PAGES_WIDTH * VRAM_PAGES_HEIGHT * 3);
  if (!texrgb) { free(clutrgb); return false; }

  dst = texrgb;
  for (int y = 0; y < VRAM_PAGES_HEIGHT; ++y) {
    for (int x = 0; x < VRAM_PAGES_WIDTH; ++x) {
      const uint16_t c = texatlas[y][x];
      for (int i = 0; i < 16; i += 4, dst += 3) {
        dst[0] = ((c >> i) & 0xF) << 4;
        dst[1] = ((c >> i) & 0xF) << 4;
        dst[2] = ((c >> i) & 0xF) << 4;
      }
    }
  }

  /*
  for (int i = 0; i < numsurfaces; ++i) {
    const vram_surf_t *surf = &surfaces[i];
    const struct surfinfo *info = &surfinfo[i];
    const int frac = (surf->mode == 0) ? 4 : 2;
    const int size = (surf->mode == 0) ? 4 : 8;
    const int mask = (surf->mode == 0) ? 0x0F : 0xFF;
    const int pagex = (surf->tex_x - VRAM_XSTART);
    const uint8_t *clut = &clutrgb[(info->clut_y * VRAM_CLUT_PAGE_WIDTH + info->clut_x) * 3];
    printf("%02d clut_x=%03d clut_y=%03d tex_w=%03d tex_h=%03d\n", i, info->clut_x, info->clut_y, info->tex_w, info->tex_h);
    for (int y = surf->tex_y; y < surf->tex_y + info->tex_h; ++y) {
      dst = &texrgb[(y * VRAM_PAGES_WIDTH + pagex) * 3 * 4];
      for (int x = 0; x < info->tex_w; ++x, dst += 3) {
        const int shift = (x & (frac - 1)) * size;
        const uint8_t cidx = (texatlas[y][pagex + x / frac] >> shift) & mask;
        dst[0] = clut[cidx * 3 + 0];
        dst[1] = clut[cidx * 3 + 1];
        dst[2] = clut[cidx * 3 + 2];
      }
    }
  }
  */

  char fpath[2048];
  snprintf(fpath, sizeof(fpath), "%s.clut.png", fname);
  stbi_write_png(fpath, VRAM_CLUT_PAGE_WIDTH,  VRAM_HEIGHT, 3, clutrgb, 0);
  snprintf(fpath, sizeof(fpath), "%s.surf.png", fname);
  stbi_write_png(fpath, 4 * VRAM_PAGES_WIDTH,  VRAM_HEIGHT, 3, texrgb, 0);

  free(clutrgb);
  free(texrgb);

  return true;
}

bool vram_restrict_clut_rect(const int pg, const vram_rect_t *rect) {
  // we're going to assume that everything above the rect is filled
  vram_rect_t r = *rect;
  r.w = VRAM_CLUT_PAGE_WIDTH;
  r.h = r.y + r.h;
  r.x = 0;
  r.y = 0;

  clutrect[pg].x = 0;
  clutrect[pg].y = r.h;
  clutrect_base[pg] = *rect;

  // fake-fill everything inside of the rect with white
  for (int y = r.y; y < r.y + r.h; ++y)
    for(int x = r.x; x < r.x + r.w; ++x)
      clutatlas[pg][y * VRAM_CLUT_PAGE_WIDTH + x] = 0xFFFF;

  // advance clut pointer; assume x is 0
  clutptr[pg] = r.h * VRAM_CLUT_PAGE_WIDTH;

  return true;
}

bool vram_restrict_surf_rect(const int pg, const vram_rect_t *rect) {
  // we're going to assume that everything to the left of the rect is filled
  vram_rect_t r = *rect;
  r.w = r.x + r.w;
  r.h = VRAM_PAGE_HEIGHT;
  r.x = 0;
  r.y = 0;

  texrect[pg].x = r.w;
  texrect[pg].y = 0;
  texrect_base[pg] = *rect;

  // fake-fill everything inside of the rect with white
  for (int y = r.y; y < r.y + r.h; ++y)
    for(int x = r.x; x < r.x + r.w; ++x)
      texatlas[y + pg * VRAM_PAGE_HEIGHT][x] = 0xFFFF;

  // mark everything inside the rect as off limits
  rect_fill(pg, r.x, r.y, r.w, r.h);

  return true;
}

const vram_rect_t *vram_get_filled_surf_rect(const int pg) {
  return &texrect[pg];
}

const vram_rect_t *vram_get_filled_clut_rect(const int pg) {
  return &clutrect[pg];
}

void vram_reset(void) {
  memset(clutptr, 0, sizeof(clutptr));
  memset(clutrect, 0, sizeof(clutrect));
  memset(texbitmap, 0, sizeof(texbitmap));
  memset(texrect, 0, sizeof(texrect));
  memset(texatlas, 0, sizeof(texatlas));

  numcluts = numsurfaces = 0;

  // restore rects
  for (int i = 0; i < VRAM_NUM_CLUT_PAGES; ++i)
    vram_restrict_clut_rect(i, &clutrect_base[i]);
  for (int i = 0; i < VRAM_NUM_PAGES; ++i)
    vram_restrict_surf_rect(i, &texrect_base[i]);
}
