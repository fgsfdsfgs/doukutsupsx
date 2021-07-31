#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "common.h"
#include "vram.h"
#include "surface.h"

#define NUM_LIST_ARGS 10
#define NUM_SURF_ARGS 3
#define NUM_RECT_ARGS 4

static inline bool parse_vram_surf_rect(char **args) {
  if (!isdigit(args[0][7])) {
    fprintf(stderr, "warning: SURFSET has to have a page numer at the end\n");
    return false;
  }
  const int pg = args[0][7] - '0';
  if (pg > VRAM_NUM_PAGES) {
    fprintf(stderr, "warning: SURFSET page number too high\n");
    return false;
  }
  const vram_rect_t r = { atoi(args[1]), atoi(args[2]), atoi(args[3]), atoi(args[4]) };
  vram_restrict_surf_rect(pg, &r);
  printf("surf page %d limited by rect: (%d, %d, %d, %d)\n", pg, r.x, r.y, r.w, r.h);
}

static inline bool parse_vram_clut_rect(char **args) {
  if (!isdigit(args[0][7])) {
    fprintf(stderr, "warning: CLUTSET has to have a page numer at the end\n");
    return false;
  }
  const int pg = args[0][7] - '0';
  if (pg > VRAM_NUM_CLUT_PAGES) {
    fprintf(stderr, "warning: CLUTSET page number too high\n");
    return false;
  }
  const vram_rect_t r = { atoi(args[1]), atoi(args[2]), atoi(args[3]), atoi(args[4]) };
  vram_restrict_clut_rect(pg, &r);
  printf("clut page %d limited by rect: (%d, %d, %d, %d)\n", pg, r.x, r.y, r.w, r.h);
}

static inline bool parse_surf_entry(char **args, surf_list_t *list) {
  const uint32_t id = atoi(args[0]);
  if (id > SURFACE_ID_MAX) {
    fprintf(stderr, "warning: invalid surface id %u\n", id);
    return false;
  }

  const char *path = trim_whitespace(args[1]);
  if (!path[0]) {
    fprintf(stderr, "warning: empty surface path for surface %u\n", id);
    return false;
  }

  const int pad = atoi(args[2]);
  if (pad < 0 || pad > 256) {
    fprintf(stderr, "warning: invalid pad value %d\n", pad);
    return false;
  }

  strncpy(list->fname, path, sizeof(list->fname) - 1);
  list->id = id;
  list->img.leftpad = pad;
  return true;
}

int read_surflist(surf_list_t *list, FILE *f) {
  char line[2048];
  char *token[1 + NUM_LIST_ARGS] = { NULL };
  char *p;
  int num = 0;

  while (true) {
    p = fgets(line, sizeof(line) - 1, f);
    if (!p) break;

    p = skip_whitespace(p);
    if (!*p || *p == '#') continue;

    int i = 0;
    token[0] = p = strtok(p, ",\r\n");
    for (i = 1; i < NUM_LIST_ARGS && p; ++i)
      token[i] = p = strtok(NULL, ",\r\n");
    if (i < NUM_RECT_ARGS || i > NUM_LIST_ARGS) {
      fprintf(stderr, "warning: ignoring malformed directive:\n%s\n", line);
      continue;
    }

    if (!strncasecmp(token[0], "SURF", 5) && i >= NUM_SURF_ARGS && num < MAX_SURFACES)
      num += (int)parse_surf_entry(&token[1], &list[num]);
    else if (!strncasecmp(token[0], "SURFSET", 7) && i >= NUM_RECT_ARGS)
      parse_vram_surf_rect(token);
    else if (!strncasecmp(token[0], "CLUTSET", 7) && i >= NUM_RECT_ARGS)
      parse_vram_clut_rect(token);
  }

  return num;
}

int convert_surface(const uint32_t id, const struct bitmap *img) {
  const uint16_t mode = (img->numcolors > 16);

  const uint16_t clut = vram_fit_clut(img->palette, mode);
  if (!clut) {
    fprintf(stderr, "error: could not fit CLUT for #%u in VRAM!\n", id);
    return -1;
  }

  // tilesets need to be aligned to address them more easily
  // backgrounds are just sometimes fuckhuge
  const bool align_to_page = (id == SURFACE_ID_LEVEL_TILESET) ||
    (id == SURFACE_ID_LEVEL_BACKGROUND);
  vram_surf_t *vsurf = vram_fit_surf(img, clut, align_to_page);
  if (!vsurf) {
    fprintf(stderr, "error: could not fit #%u in VRAM!\n", id);
    return -2;
  }

  vsurf->id = id;

  return 0;
}

int read_bmp(struct bitmap *bmp, FILE *f) {
  struct {
    char id[2]; // BM
    uint32_t fsize;
    uint16_t reserved[2];
    uint32_t dataofs;
  } __attribute__((packed)) header;

  struct {
    uint32_t size;
    int32_t w;
    int32_t h;
    uint16_t planes;
    uint16_t bpp;
    uint32_t compression;
    uint32_t datasize;
    int32_t ppm_x;
    int32_t ppm_y;
    uint32_t numcolors;
    uint32_t usedcolors;
  } __attribute__((packed)) dib;

  fread(&header, sizeof(header), 1, f);
  if (header.id[0] != 'B' || header.id[1] != 'M')
    return -1;

  fread(&dib, sizeof(dib), 1, f);
  if (dib.size != sizeof(dib))
    return -2;
  if (dib.bpp != 1 && dib.bpp != 4 && dib.bpp != 8)
    return -3;
  if ((dib.numcolors < 2 && dib.numcolors) || dib.numcolors > 256)
    return -4;
  if (dib.compression != 0)
    return -5;

  if (dib.bpp)
    bmp->numcolors = 1 << dib.bpp;
  else
    bmp->numcolors = dib.numcolors;

  if (dib.bpp < 4)
    dib.bpp = 4; // can't have 1-bit images

  bmp->width = dib.w;
  bmp->height = dib.h;
  bmp->stride = 4 * ((dib.bpp * dib.w + 31) / 32);

  fread(bmp->palette, bmp->numcolors * 4, 1, f);

  bmp->data = calloc(1, bmp->stride * bmp->height);
  if (!bmp->data) return -6;

  if (bmp->numcolors == 2) {
    bmp->numcolors = 16;
    return 0; // assume the image is all black
  }

  fseek(f, header.dataofs, SEEK_SET);

  // read in reverse because BMPs are mirrored vertically
  uint8_t *dst = bmp->data + bmp->stride * (bmp->height - 1);
  uint8_t linebuf[bmp->stride];
  for (int y = 0; y < dib.h; ++y) {
    fread(linebuf, bmp->stride, 1, f);
    memcpy(dst, linebuf, bmp->stride);
    dst -= bmp->stride;
  }

  return 0;
}
