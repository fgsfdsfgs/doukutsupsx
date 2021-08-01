#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <assert.h>

#include "common/common.h"
#include "common/tsc.h"

int main(int argc, char **argv) {
  if (argc < 3) {
    printf("usage: tscc [-d] <input_tsc> [<output_bsc>]\n");
    return -1;
  }

  const char *infile = NULL;
  const char *outfile = NULL;
  bool decode_only = !strcmp(argv[1], "-d");
  if (!decode_only) {
    infile = argv[1];
    outfile = argv[2];
  } else {
    infile = argv[2];
  }

  FILE *f = fopen(infile, "rb");
  if (!f) {
    fprintf(stderr, "error: could not open '%s' for reading\n", infile);
    return -2;
  }

  fseek(f, 0, SEEK_END);
  const int size = ftell(f);
  fseek(f, 0, SEEK_SET);

  char *buf = calloc(1, size + 1); // add NUL
  assert(buf);
  fread(buf, size, 1, f);
  fclose(f);

  tsc_decode((uint8_t *)buf, size);

  if (decode_only) {
    printf("%s\n", buf);
    free(buf);
    return 0;
  }

  int tsc_size = -1;
  tsc_script_t *tsc = tsc_compile(buf, size, &tsc_size);
  free(buf);
  if (tsc_size <= 0)
    return -3;

  f = fopen(outfile, "wb");
  if (!f) {
    fprintf(stderr, "error: could not open '%s' for writing\n", outfile);
    free(tsc);
    return -2;
  }

  fwrite(tsc, tsc_size, 1, f);
  fclose(f);

  free(tsc);

  return 0;
}
