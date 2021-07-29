#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <assert.h>
#include <string.h>
#include <unistd.h>

// #define SAVE_WAVS 1

#ifdef SAVE_WAVS
#define DR_WAV_IMPLEMENTATION
#include "common/dr_wav.h"
#endif

#include "libpsxav/libpsxav.h"
#include "common/common.h"

#define MAX_TRACKS 16
#define MAX_MELODY_TRACKS 8
#define NUM_WAVEFORMS 100
#define WAVEFORM_LEN 256
#define NUM_OCT 8

#define ORG_MAGIC "Org-0"
#define ORG_MAGICLEN 6 // +1 char for version

#pragma pack(push, 1)

typedef struct {
  uint16_t freq;     // frequency modifier (default = 1000)
  uint8_t wave_no;   // waveform index in the wavetable
  uint8_t pipi;      // loop flag?
  uint16_t note_num; // number of notes in track
} org_trackhdr_t;

typedef struct {
  uint16_t wait;
  uint8_t line;
  uint8_t dot;
  int32_t repeat_x;
  int32_t end_x;
  org_trackhdr_t tdata[MAX_TRACKS];
} org_hdr_t;

#pragma pack(pop)

// organya header
static org_hdr_t org_data;

// output PSX SPURAM
static uint8_t spuram[SPURAM_SIZE + 1024]; // 1kb of grace zone
static int spuram_ptr = SPURAM_START;
static int spuram_start = SPURAM_START;

// wave.dat
static int8_t wavetable[NUM_WAVEFORMS][WAVEFORM_LEN];

// output instrument samples
static struct sfx inst[MAX_MELODY_TRACKS][NUM_OCT];

static const struct {
  short wave_size;
  short oct_par;
  short oct_size;
} oct_wave[NUM_OCT] = {
  { 256,   1,  4 }, // 0 Oct
  { 256,   2,  8 }, // 1 Oct
  { 128,   4, 12 }, // 2 Oct
  { 128,   8, 16 }, // 3 Oct
  {  64,  16, 20 }, // 4 Oct
  {  32,  32, 24 }, // 5 Oct
  {  16,  64, 28 }, // 6 Oct
  {   8, 128, 32 }, // 7 Oct
};

static struct bank_hdr bank_hdr;

#ifdef SAVE_WAVS
static void save_track_sample(const int i, const int j, const char *fmask) {
  char path[2048];
  snprintf(path, sizeof(path), "%s%d.wav", fmask, i * NUM_OCT + j);
  drwav wav;
  drwav_data_format format;
  format.container = drwav_container_riff;
  format.format = DR_WAVE_FORMAT_PCM;
  format.channels = 1;
  format.sampleRate = 22050;
  format.bitsPerSample = 16;
  if (drwav_init_file_write(&wav, path, &format, NULL)) {
    drwav_write_pcm_frames(&wav, inst[i][j].len, inst[i][j].data);
    drwav_uninit(&wav);
  }
}
#endif

static inline int lcm(const int a, const int b) {
  register int step, div, x;
  if (a > b) {
    step = b;
    div = a;
  } else {
    step = a;
    div = b;
  }
  x = step;
  while (x % div)
    x += step;
  return x;
}

// this is basically MakeSoundObject8 from Organya.cpp
static void build_track_samples(const int idx, const int8_t *wavep, const bool pipi) {
  for (int j = 0; j < NUM_OCT; ++j) {
    // generate unsigned 8-bit PCM sample
    const uint32_t wave_size = oct_wave[j].wave_size;
    const uint32_t num_samples = lcm(wave_size, 28);
    int16_t *wp = malloc(num_samples * sizeof(int16_t));
    assert(wp);
    uint32_t wav_tp = 0;
    int16_t *wp_sub = wp;
    for (uint32_t i = 0; i < num_samples; ++i) {
      int work = *(wavep + wav_tp);
      work *= 256;
      *wp_sub = (int16_t)work;
      wav_tp += 0x100 / wave_size;
      if (wav_tp > 0xFF)
        wav_tp -= 0x100;
      wp_sub++;
    }
    inst[idx][j].data = wp;
    inst[idx][j].len = num_samples;
#ifdef SAVE_WAVS
    save_track_sample(idx, j, "orgwave/");
#endif
  }
}

static bool load_wavetable(const char *fname) {
  FILE *f = fopen(fname, "rb");
  if (!f) return false;
  const size_t rx = fread(wavetable, sizeof(wavetable), 1, f);
  fclose(f);
  return (rx > 0);
}

static bool load_org_tracks(const char *fname) {
  FILE *f = fopen(fname, "rb");
  if (!f) return false;

  char magic[ORG_MAGICLEN + 1] = { 0 };
  fread(magic, ORG_MAGICLEN, 1, f);
  if (memcmp(magic, ORG_MAGIC, ORG_MAGICLEN - 1)) {
    fclose(f);
    fprintf(stderr, "error: '%s' is not a valid Org file\n", fname);
    return false;
  }

  const int ver = magic[ORG_MAGICLEN - 1] - '0';
  if (ver != 1 && ver != 2) {
    fclose(f);
    fprintf(stderr, "error: '%s' has version %d, expected 1 or 2\n", fname, ver);
    return false;
  }

  fread(&org_data, sizeof(org_data), 1, f);
  fclose(f);

  // don't know if this is required
  if (ver == 1) {
    for (int i = 0; i < MAX_TRACKS; ++i)
        org_data.tdata[i].pipi = 0;
  }

  for (int i = 0; i < MAX_MELODY_TRACKS; ++i)
    build_track_samples(i, wavetable[org_data.tdata[i].wave_no], org_data.tdata[i].pipi);

  return true;
}

static void cleanup(void) {
  for (int i = 0; i < MAX_MELODY_TRACKS; ++i) {
    for (int j = 0; j < NUM_OCT; ++j) {
      if (inst[i][j].data)
        free(inst[i][j].data);
    }
  }
}

int main(int argc, char **argv) {
  if (argc < 4) {
    printf("usage: orgconv <org_file> <wave_dat> <out_bank> [<spu_start_addr>]\n");
    return -1;
  }

  atexit(cleanup);

  const char *orgfname = argv[1];
  const char *datfname = argv[2];
  const char *outfname = argv[3];
  if (argc > 4) {
    int addr = atoi(argv[4]);
    if (addr > SPURAM_START)
      spuram_ptr = spuram_start = addr;
  }

  if (!load_wavetable(datfname)) {
    fprintf(stderr, "error: could not load wavetable from '%s'\n", datfname);
    return -2;
  }

#ifdef SAVE_WAVS
  plat_mkdir("orgwave", 0755);
#endif

  if (!load_org_tracks(orgfname)) {
    fprintf(stderr, "error: could not load track data from '%s'\n", orgfname);
    return -3;
  }

  for (int i = 0; i < MAX_MELODY_TRACKS; ++i) {
    for (int j = 0; j < NUM_OCT; ++j) {
      const int adpcm_len = psx_audio_spu_encode_simple(inst[i][j].data, inst[i][j].len, spuram + spuram_ptr, 0);
      if (adpcm_len <= 0) {
        fprintf(stderr, "error: could not encode instrument sample %d/%d\n", i, j);
        return -4;
      }
      inst[i][j].addr = spuram_ptr;
      spuram_ptr += ALIGN(adpcm_len, 8);
      if (spuram_ptr >= SPURAM_SIZE) {
        fprintf(stderr, "error: ran out of SPU RAM packing instrument sample %d/%d\n", i, j);
        return -5;
      }
      // printf(" * instrument [%d][%d]: addr=%05x pcmlen=%06u adpcmlen=%06d\n", i, j, inst[i][j].addr, inst[i][j].len, adpcm_len);
    }
  }

  bank_hdr.num_sfx = MAX_MELODY_TRACKS * NUM_OCT;
  bank_hdr.data_size = spuram_ptr - spuram_start;

  /*
  printf("bank addr:\n");
  for (int i = 0; i < MAX_MELODY_TRACKS; ++i)
    for (int j = 0; j < NUM_OCT; ++j)
      printf("* (%03d) 0x%06x\n", i*NUM_OCT + j, inst[i][j].addr);
  */

  printf("SPU RAM total usage: %u/%u bytes\n", spuram_ptr, SPURAM_SIZE);
  printf("SPU RAM start address: %u\n", SPURAM_START);
  printf("bank size: %u bytes\n", bank_hdr.data_size);
  printf("bank start: %u\n", spuram_start);
  printf("bank ident: %02x %02x %02x %02x\n",
    spuram[spuram_start+0], spuram[spuram_start+1], spuram[spuram_start+2], spuram[spuram_start+3]);

  FILE *f = fopen(outfname, "wb");
  if (!f) {
    fprintf(stderr, "error: could not open '%s' for writing\n", outfname);
    return -5;
  }

  // write header without the first address because fuck this shit
  fwrite(&bank_hdr, sizeof(bank_hdr) - sizeof(uint32_t), 1, f);
  // write sample addresses starting with 0, 0
  for (int i = 0; i < MAX_MELODY_TRACKS; ++i)
    for (int j = 0; j < NUM_OCT; ++j)
      fwrite(&inst[i][j].addr, sizeof(uint32_t), 1, f);
  // write sample data
  fwrite(spuram + spuram_start, bank_hdr.data_size, 1, f);

  fclose(f);

  return 0;
}
