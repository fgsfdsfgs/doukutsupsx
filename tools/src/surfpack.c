#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "common/common.h"
#include "common/surface.h"
#include "common/vram.h"

static surf_list_t surflist[MAX_SURFACES];
static int surfnum;

static inline int load_surface(int i) {
  printf("loading %03d from %s: ", surflist[i].id, surflist[i].fname);

  FILE *f = fopen(surflist[i].fname, "rb");
  if (!f) {
    fprintf(stderr, "error: could not open bitmap '%s'\n", surflist[i].fname);
    return -4;
  }

  const int ret = read_bmp(&surflist[i].img, f);
  fclose(f);
  if (ret < 0) {
    fprintf(stderr, "error: could not read BMP: %d\n", ret);
    return -5;
  }

  printf("%03dx%03d, %03d colors\n", surflist[i].img.width, surflist[i].img.height, surflist[i].img.numcolors);

  return 0;
}

int main(int argc, char **argv) {
  if (argc < 3) {
    printf("usage: surfpack [-e] <surface_list> <output_file>\n");
    return -1;
  }

  bool do_export = false;
  const char *infile;
  const char *outfile;
  if (argc == 4) {
    do_export = !strcmp(argv[1], "-e");
    infile = argv[2];
    outfile = argv[3];
  } else {
    infile = argv[1];
    outfile = argv[2];
  }

  FILE *f = fopen(infile, "r");
  if (!f) {
    fprintf(stderr, "error: could not open '%s' for reading\n", infile);
    return -2;
  }

  surfnum = read_surflist(surflist, f);
  fclose(f);
  puts("");

  if (surfnum <= 0) {
    fprintf(stderr, "error: empty or invalid surface list in '%s'\n", infile);
    return -3;
  }

  for (int i = 0; i < surfnum; ++i) {
    if (load_surface(i) < 0)
      return -4;
    if (convert_surface(surflist[i].id, &surflist[i].img) < 0)
      return -5;
  }

  f = fopen(outfile, "wb");
  if (!f) {
    fprintf(stderr, "error: could not open '%s' for writing\n", outfile);
    return -6;
  }

  vram_write_surf_bank(f);
  fclose(f);

  if (do_export)
    vram_export_png(outfile);

  const vram_rect_t *r;
  for (int i = 0; i < VRAM_NUM_PAGES; ++i) {
    r = vram_get_filled_surf_rect(i);
    printf("space taken up on surface page %d: (%d, %d, %d, %d)\n", i, r->x, r->y, r->w, r->h);
  }
  for (int i = 0; i < VRAM_NUM_CLUT_PAGES; ++i) {
    r = vram_get_filled_clut_rect(i);
    printf("space taken up on CLUT page %d: (%d, %d, %d, %d)\n", i, r->x, r->y, r->w, r->h);
  }

  return 0;
}
