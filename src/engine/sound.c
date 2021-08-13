#include "engine/sound.h"

#include <stdio.h>
#include <stdlib.h>

#include "engine/common.h"
#include "engine/filesystem.h"
#include "engine/memory.h"
#include "spu.h"

#define SFX_FREQ 22050

sfx_bank_t *snd_main_bank;

// channel each sample last played on
static s16 sample_chan[MAX_SFX];

// last channel a sample was played on
static int last_chan = CHAN_SOUND;

// sfx channel state
static struct {
  s32 sample; // sample playing (or last played) on this channel
  s16 prio;   // priority of the sample playing (or last played) on this channel
  u16 loop;   // is this channel looping?
} chan_state[SFX_NUM_CHANNELS];

int snd_init(const char *mainbankpath) {
  spu_init();

  snd_main_bank = snd_load_sfx_bank(mainbankpath);
  if (!snd_main_bank) panic("could not load\n%s", mainbankpath);

  // turn up all the SFX channels
  for (int i = 0; i < SFX_NUM_CHANNELS; ++i)
    spu_set_voice_volume(i, SPU_MAX_VOLUME);

  // clear sfx->channel mapping
  for (int i = 0; i < MAX_SFX; ++i)
    sample_chan[i] = -1;

  spu_flush_voices();
}

int snd_upload_sfx_bank(sfx_bank_t *bank, u8 *bank_data) {
  if (!bank_data)
    bank_data = (u8 *)bank + sizeof(*bank) + sizeof(u32) * bank->num_sfx;

  ASSERT(spuram_ptr == bank->sfx_addr[0] || spuram_ptr == bank->sfx_addr[1]);

  SpuSetTransferMode(SPU_TRANSFER_BY_DMA);
  spu_set_transfer_addr(spuram_ptr);
  SpuWrite((void *)bank_data, bank->data_size);
  spu_wait_for_transfer();

  spuram_ptr += bank->data_size;

  return TRUE;
}

sfx_bank_t *snd_read_sfx_bank(fs_file_t *f) {
  const u32 buflen = fs_fread_u32(f);
  const u32 num_sfx = fs_fread_u32(f);

  sfx_bank_t *bank = mem_alloc(sizeof(*bank) + sizeof(u32) * num_sfx);
  bank->data_size = buflen;
  bank->num_sfx = num_sfx;
  fs_fread_or_die(&bank->sfx_addr[0], sizeof(u32) * num_sfx, 1, f);

  u8 *buf = mem_alloc(buflen);
  fs_fread_or_die(buf, buflen, 1, f);

  snd_upload_sfx_bank(bank, buf);

  mem_free(buf);

  return bank;
}

sfx_bank_t *snd_load_sfx_bank(const char *path) {
  fs_file_t *f = fs_fopen(path, 0);
  if (!f) panic("could not open\n%s", path);
  sfx_bank_t *ret = snd_read_sfx_bank(f);
  fs_fclose(f);
  return ret;
}

void snd_free_sfx_bank_data(sfx_bank_t *bank) {
  const u32 prevaddr = spuram_ptr - bank->data_size;
  if (prevaddr == bank->sfx_addr[0] || prevaddr == bank->sfx_addr[1])
    spuram_ptr = prevaddr;
}

void snd_free_sfx_bank(sfx_bank_t *bank) {
  ASSERT(bank);
  // free SPU RAM if this is the last loaded bank
  snd_free_sfx_bank_data(bank);
  // free the bank header
  mem_free(bank);
}

static inline int snd_find_chan(const int start_idx, const int prio, const int sfx) {
  // check if the sample is already playing somewhere; if so, override it like in the original game
  const int sample_ch = sample_chan[sfx];
  if (sample_ch >= 0 && chan_state[sample_ch].sample == sfx)
    return sample_ch;

  // otherwise scan through channels and see what we can override
  const u32 endmask = spu_get_voice_end_mask();
  for (int n = start_idx; n < start_idx + SFX_NUM_CHANNELS; ++n) {
    const int i = n % SFX_NUM_CHANNELS;
    // nothing ever played here OR channel was stopped OR playback complete; can be used
    if (chan_state[i].sample == 0 || ((endmask & SPU_VOICECH(i)) && !chan_state[i].loop))
      return i;
  }

  for (int n = start_idx; n < start_idx + SFX_NUM_CHANNELS; ++n) {
    const int i = n % SFX_NUM_CHANNELS;
    // lower priority; can be overridden
    if (prio > chan_state[i].prio)
      return i;
  }

  // nothing found
  return -1;
}

int snd_play_sound_freq(int prio, const int no, const int freq, const bool loop) {
  ASSERT(snd_main_bank);

  if (no < 0 || no >= snd_main_bank->num_sfx)
    panic("unknown sound %03d (max is %03d)", no, snd_main_bank->num_sfx);

  int ch = snd_find_chan(last_chan + 1, prio, no);
  if (ch < 0) {
    if (prio > PRIO_LOW)
      ch = (last_chan + 1) % SFX_NUM_CHANNELS; // just override whatever
    else
      return -1; // don't play low priority sounds when out of channels
  }

  spu_play_sample(ch, snd_main_bank->sfx_addr[no], freq);

  sample_chan[no] = ch;
  chan_state[ch].sample = no;
  chan_state[ch].loop = loop;
  chan_state[ch].prio = prio;
  last_chan = ch;

  return ch;
}

int snd_play_sound(int prio, const int no, const bool loop) {
  return snd_play_sound_freq(prio, no, SFX_FREQ, loop);
}

void snd_stop_sound(const int no) {
  ASSERT(no > 0 && no < MAX_SFX);
  snd_stop_channel(sample_chan[no]);
}

void snd_stop_channel(const int ch) {
  spu_key_off(SPU_VOICECH(ch));
  chan_state[ch].loop = FALSE;
  chan_state[ch].sample = 0;
  chan_state[ch].prio = 0;
}
