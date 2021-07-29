#pragma once

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>
#include <unistd.h>

#define BMP_MAX_PALETTE 256

#define NUM_INST 100
#define INST_LEN 256
#define MAX_SFX  160

#define SPURAM_ALIGN 8
#define SPURAM_START 0x1000
#define SPURAM_SIZE  (512 * 1024)
#define SPURAM_AVAIL (SPURAM_SIZE - SPURAM_START)

#define ALIGN(x, align) (((x) + ((align) - 1)) & ~((align) - 1))

#ifdef _WIN32
#define plat_mkdir(x, y) mkdir(x)
#else
#define plat_mkdir(x, y) mkdir(x, y)
#endif

#pragma pack(push, 1)

struct bank_hdr {
  uint32_t data_size;   // size of raw SPU data at the end
  uint32_t num_sfx;     // number of samples in bank, including #0 (dummy) and all the unused samples
  uint32_t sfx_addr[1]; // address in SPU RAM of each sample, first one is always 0, others may be 0 (means it's unused)
  // after the last sfx_addr, raw SPU data follows
};

struct sfx {
  int16_t *data;
  uint32_t len; // in samples
  uint32_t freq;
  uint32_t addr;
};

struct bitmap {
  uint8_t *data;
  uint8_t palette[BMP_MAX_PALETTE * 4];
  int width;
  int height;
  int numcolors;
  int stride;
  int leftpad;
};

#pragma pack(pop)

static inline char *skip_whitespace(char *p) {
  while (*p && isspace(*p)) ++p;
  return p;
}

static inline char *trim_whitespace(char *p) {
  while (*p && isspace(*p)) ++p;
  const unsigned len = strlen(p);
  for (unsigned i = len; i >= 0 && p[i] && isspace(p[i]); --i)
    p[i] = '\0';
  return p;
}

static inline uint8_t *read_file(const char *fname, uint32_t *out_size) {
  FILE *f = fopen(fname, "rb");
  if (!f) return NULL;

  fseek(f, 0, SEEK_END);
  const uint32_t size = ftell(f);
  const uint32_t asize = ALIGN(size, 4);
  fseek(f, 0, SEEK_SET);

  uint8_t *buf = calloc(1, asize);
  assert(buf);

  fread(buf, size, 1, f);
  fclose(f);

  *out_size = asize;

  return buf;
}
