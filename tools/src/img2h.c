#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>

#define STB_IMAGE_IMPLEMENTATION
#include "common/stb_image.h"

#define PSXRGB(r, g, b) ((((b) >> 3) << 10) | (((g) >> 3) << 5) | ((r) >> 3))

int main(int argc, char **argv) {
  if (argc < 2) {
    printf("usage: img2h <inputfile>\n");
    return -1;
  }

  int w, h;
  uint8_t *data = stbi_load(argv[1], &w, &h, NULL, 3);
  if (!data) {
    fprintf(stderr, "error: could not read image '%s'\n", argv[1]);
    return -1;
  }

  printf("const int img_width = %d;\n", w);
  printf("const int img_height = %d;\n", h);
  printf("const unsigned short img_data[%d] = {\n ", w * h);

  const uint8_t *p = data;
  for (int i = 0; i < w * h; ++i, p += 3) {
    const uint16_t c = PSXRGB(p[0], p[1], p[2]);
    printf(" 0x%04X,", c);
    if (i != (w * h - 1) && (i & 7) == 7)
        printf("\n ");
  }

  puts("\n};");

  free(data);
  return 0;
}
