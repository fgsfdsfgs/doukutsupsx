#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "common/common.h"
#include "common/surface.h"
#include "common/vram.h"

#define MAX_CREDITS_IMAGES 19

static struct bitmap bmps[MAX_CREDITS_IMAGES];
static int num_bmps = 0;

static int load_image(const char *path) {
  bmps[num_bmps].width = 0;
  bmps[num_bmps].height = 0;
  bmps[num_bmps].numcolors = 0;

  FILE *f = fopen(path, "rb");
  if (!f) { num_bmps++; return -1; }

  const int ret = read_bmp(&bmps[num_bmps++], f);
  fclose(f);
  if (ret < 0) return -2;

  return 0;
}

static uint8_t *transform_image(uint8_t *dst, const struct bitmap *bmp) {
  const uint8_t *src = bmp->data;
  const int linelen = (bmp->numcolors > 16) ? bmp->width : (bmp->width + 1) >> 1;
  uint8_t *pdst = dst;
  for (int y = 0; y < bmp->height; ++y) {
    if (bmp->numcolors > 16) {
      memcpy(pdst, src, linelen);
      pdst += linelen;
    } else {
      // in BMP the leftmost pixel is in the topmost nibble because fuck you
      const uint8_t *psrc = src;
      pdst = &dst[linelen * y];
      for (int i = 0; i < linelen; ++i, ++psrc, ++pdst)
        *pdst = ((*psrc & 0xF) << 4) | (*psrc >> 4);
    }
    src += bmp->stride;
  }
  return pdst;
}

int main(int argc, char **argv) {
  if (argc < 3) {
    printf("usage: creditspack <data_path> <output_file>\n");
    return -1;
  }

  const char *datapath = argv[1];
  const char *outpath = argv[2];
  char buf[2048];

  uint32_t total_size = sizeof(ram_surfbank_t);

  snprintf(buf, sizeof(buf), "%s/casts.pbm", datapath);
  if (load_image(buf) < 0) {
    fprintf(stderr, "error: could not read bmp '%s'\n", buf);
    return -1;
  }

  int linelen = (bmps[0].numcolors > 16) ? bmps[0].width : (bmps[0].width + 1) >> 1;
  total_size += sizeof(ram_surf_t) + (linelen * bmps[0].height) +
    ((bmps[0].numcolors > 16) ? 256 : 16) * 2;

  for (int i = 1; i < MAX_CREDITS_IMAGES; ++i) {
    snprintf(buf, sizeof(buf), "%s/credit/credit%02d.bmp", datapath, i);
    load_image(buf);
    linelen = (bmps[i].numcolors > 16) ? bmps[i].width : (bmps[i].width + 1) >> 1;
    total_size += sizeof(ram_surf_t) + (linelen * bmps[i].height) +
      ((bmps[i].numcolors > 16) ? 256 : 16) * 2;
    printf("* (%02d) '%s': %dx%d, %d colors\n", i, buf, bmps[i].width, bmps[i].height, bmps[i].numcolors);
  }

  printf("total_size = %u\n", total_size);

  ram_surfbank_t *bank = calloc(1, total_size);
  assert(bank);

  uint8_t *surfdata = (uint8_t *)bank + sizeof(*bank) + sizeof(*bank->surf) * num_bmps;
  bank->numsurf = num_bmps;

  for (int i = 0; i < num_bmps; ++i) {
    if (bmps[i].width == 0) {
      bank->surf[i].mode = -1;
    } else {
      bank->surf[i].mode = (bmps[i].numcolors > 16);
      bank->surf[i].w = bmps[i].width;
      bank->surf[i].h = bmps[i].height;
      bank->surf[i].ofs = surfdata - (uint8_t *)bank;
      uint8_t *start = surfdata;
      surfdata = transform_image(surfdata, &bmps[i]);
      vram_transform_clut((uint16_t *)surfdata, bmps[i].palette, bmps[i].numcolors);
      surfdata += 2 * (bank->surf[i].mode ? 256 : 16);
      bank->surf[i].size = (surfdata - start) / 2;
      printf("* (%02d) %3dx%3d, %3d colors: ofs %06x size %06x words\n", i, bank->surf[i].w, bank->surf[i].h, bmps[i].numcolors, bank->surf[i].ofs, bank->surf[i].size);
    }
  }

  FILE *f = fopen(outpath, "wb");
  if (!f) {
    fprintf(stderr, "error: could not open '%s' for writing\n", outpath);
    free(bank);
    return -1;
  }

  fwrite(bank, surfdata - (uint8_t *)bank, 1, f);
  fclose(f);

  return 0;
}
