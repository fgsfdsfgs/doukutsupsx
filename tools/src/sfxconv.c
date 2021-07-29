#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <assert.h>

#define DR_WAV_IMPLEMENTATION
#include "common/dr_wav.h"
#include "libpsxav/libpsxav.h"
#include "common/common.h"

// output PSX SPURAM
static uint8_t spuram[SPURAM_SIZE + 1024]; // 1kb of grace zone
static int spuram_ptr = SPURAM_START;

static struct sfx sfx[MAX_SFX]; // 0 is dummy
static int max_sfx = 0;

static struct bank_hdr bank_hdr;

static bool load_wav(struct sfx *sfx, const char *fname) {
  drwav wav;
  if (!drwav_init_file(&wav, fname, NULL))
    return false;

  if (wav.bitsPerSample != 8 || wav.channels != 1 || wav.sampleRate != 22050) {
    fprintf(stderr, "error: '%s' is not a mono unsigned 8-bit 22khz PCM WAV file!\n", fname);
    drwav_uninit(&wav);
    return false;
  }

  int16_t *pcm = malloc(sizeof(int16_t) * wav.totalPCMFrameCount);
  assert(pcm);

  const uint32_t total_read = drwav_read_pcm_frames_s16(&wav, wav.totalPCMFrameCount, pcm);
  assert(total_read);

  sfx->data = pcm;
  sfx->len = total_read;
  sfx->freq = wav.sampleRate;

  drwav_uninit(&wav);

  return true;
}

static bool load_all_sfx(const char *path) {
  char fname[2048];
  int i = 1;
  for (; i < MAX_SFX; ++i) {
    snprintf(fname, sizeof(fname), "%s/%d.wav", path, i);
    if (load_wav(&sfx[i], fname))
      max_sfx = i;
  }
  return (i > 1);
}

static void cleanup(void) {
  for (int i = 0; i < max_sfx; ++i) {
    if (sfx[i].data)
      free(sfx[i].data);
  }
}

static inline bool is_sfx_looping(const int sfx) {
  return (sfx == 40 || sfx == 41 || sfx == 58 || sfx == 7);
}

int main(int argc, char **argv) {
  if (argc != 3) {
    printf("usage: sfxconv <wavdir> <out_bank>\n");
    return -1;
  }

  atexit(cleanup);

  const char *wavpath = argv[1];
  const char *outfname = argv[2];

  if (!load_all_sfx(wavpath)) {
    fprintf(stderr, "error: could not load samples from '%s'\n", wavpath);
    return -2;
  }

  printf("%d samples loaded\n", max_sfx);

  // convert samples

  for (int i = 1; i <= max_sfx; ++i) {
    if (sfx[i].data == NULL)
      continue;
    const int loop_start = is_sfx_looping(i) ? 0 : -1;
    const int adpcm_len = psx_audio_spu_encode_simple(sfx[i].data, sfx[i].len, spuram + spuram_ptr, loop_start);
    if (adpcm_len <= 0) {
      fprintf(stderr, "error: could not encode sfx %d\n", i);
      return -3;
    }
    sfx[i].addr = spuram_ptr;
    spuram_ptr += ALIGN(adpcm_len, 8);
    if (spuram_ptr >= SPURAM_SIZE) {
      fprintf(stderr, "error: ran out of SPU RAM packing sfx %d\n", i);
      return -4;
    }
  }

  bank_hdr.num_sfx = max_sfx + 1;
  bank_hdr.data_size = spuram_ptr - SPURAM_START;
  bank_hdr.sfx_addr[0] = 0;

  printf("SPU RAM total usage: %u/%u bytes\n", spuram_ptr, SPURAM_SIZE);
  printf("SPU RAM start address: %u\n", SPURAM_START);
  printf("bank size: %u bytes\n", bank_hdr.data_size);
  printf("bank start: %u\n", SPURAM_START);
  printf("bank ident: %02x %02x %02x %02x\n",
    spuram[SPURAM_START+0], spuram[SPURAM_START+1], spuram[SPURAM_START+2], spuram[SPURAM_START+3]);

  FILE *f = fopen(outfname, "wb");
  if (!f) {
    fprintf(stderr, "error: could not open '%s' for writing\n", outfname);
    return -5;
  }

  // write header
  fwrite(&bank_hdr, sizeof(bank_hdr), 1, f);
  // write sample addresses
  for (int i = 1; i <= max_sfx; ++i)
    fwrite(&sfx[i].addr, sizeof(uint32_t), 1, f);
  // write sample data
  fwrite(spuram + SPURAM_START, spuram_ptr - SPURAM_START, 1, f);

  fclose(f);

  return 0;
}
