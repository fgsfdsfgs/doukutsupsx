#pragma once

#include "engine/common.h"
#include "engine/filesystem.h"

#define MAX_SFX 160
#define SFX_NUM_CHANNELS 8
#define SFX_MAIN_BANK "\\MAIN\\MAIN.SFX;1"


typedef enum {
  CHAN_SOUND = 0, // sfx channels from this to CHAN_MUSIC
  CHAN_MUSIC = 8, // music channels from this to CHAN_COUNT
  CHAN_COUNT = 24
} sound_channel_t;

typedef enum {
  PRIO_LOW = 1,
  PRIO_NORMAL = 2,
  PRIO_HIGH = 3,
} sound_priority_t;

typedef struct {
  u32 data_size;   // size of raw SPU data at the end
  u32 num_sfx;     // number of samples in bank, including #0 (dummy) and all the unused samples
  u32 sfx_addr[];  // address in SPU RAM of each sample, first one is always 0, others may be 0 (means it's unused)
} sfx_bank_t;

// main sample bank; never unloaded
extern sfx_bank_t *snd_main_bank;

// initializes the spu and loads the main sample bank
int snd_init(const char *mainbankpath);

// loads a sample bank into a sfx_bank struct and uploads it to spu ram
int snd_upload_sfx_bank(sfx_bank_t *bank, u8 *bank_data);
sfx_bank_t *snd_read_sfx_bank(fs_file_t *f);
sfx_bank_t *snd_load_sfx_bank(const char *path);

// frees a sfx_bank struct
void snd_free_sfx_bank(sfx_bank_t *bank);
void snd_free_sfx_bank_data(sfx_bank_t *bank);

// plays a sample from the main sample bank
// samples with higher `prio` override samples with lower `prio`
int snd_play_sound(int prio, const int no, const bool loop);

// plays a sample from the main sample bank with specified frequency
// samples with higher `prio` override samples with lower `prio`
int snd_play_sound_freq(int prio, const int no, const int freq, const bool loop);

// stop sample `no` if it's playing
void snd_stop_sound(const int no);

// stops channel `ch` if something's playing on it
void snd_stop_channel(const int ch);
