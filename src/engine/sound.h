#pragma once

#include "engine/common.h"
#include "engine/filesystem.h"

#define MAX_SFX 160
#define SFX_NUM_CHANNELS 8
#define SFX_MAIN_BANK "\\MAIN\\MAIN.SFX;1"

typedef enum {
  SOUND_MODE_PLAY_LOOP = -1,
  SOUND_MODE_STOP = 0,
  SOUND_MODE_PLAY = 1
} sound_mode_t;

typedef enum {
  CHAN_STEP  = 0, // footsteps, jump sounds, etc
  CHAN_ARMS  = 1, // gun
  CHAN_ITEM  = 2, // item pickups
  CHAN_MISC  = 3, // other player-related sounds
  CHAN_ALLOC = 4, // channels from this to CHAN_MUSIC are allocated dynamically
  CHAN_LOOP1 = 6, // channel for special looping noises, normally used by other sounds too
  CHAN_LOOP2 = 7, // channel for special looping noises, normally used by other sounds too
  CHAN_MUSIC = 8, // music channels from this to CHAN_COUNT
  CHAN_COUNT = 24
} sound_channel_t;

typedef struct {
  u32 data_size;   // size of raw SPU data at the end
  u32 num_sfx;     // number of samples in bank, including #0 (dummy) and all the unused samples
  u32 sfx_addr[1]; // address in SPU RAM of each sample, first one is always 0, others may be 0 (means it's unused)
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

// plays/stops a sample from the main sample bank
// if ch is -1, plays on next available channel
int snd_play_sound(int ch, const int no, const sound_mode_t mode);

// plays/stops a sample from the main sample bank with specified frequency
// if ch is -1, plays on next available channel
int snd_play_sound_freq(int ch, const int no, const int freq, const sound_mode_t mode);
