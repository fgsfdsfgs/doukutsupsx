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
static short sample_chan[MAX_SFX];
// sample playing (or last played) on each SFX channel
static short chan_sample[SFX_NUM_CHANNELS];
// channel to play next sample on
static short chan_next = CHAN_ALLOC;

int snd_init(const char *mainbankpath) {
  spu_init();

  snd_main_bank = snd_load_sfx_bank(mainbankpath);
  if (!snd_main_bank) panic("could not load\n%s", mainbankpath);

  // turn up all the SFX channels
  for (int i = 0; i < SFX_NUM_CHANNELS; ++i)
    spu_set_voice_volume(i, SPU_MAX_VOLUME);
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

void snd_free_sfx_bank(sfx_bank_t *bank) {
  ASSERT(bank);
  // free SPU RAM if this is the last loaded bank
  const u32 prevaddr = spuram_ptr - bank->data_size;
  if (prevaddr == bank->sfx_addr[0] || prevaddr == bank->sfx_addr[1])
    spuram_ptr = prevaddr;
  // free the bank header
  mem_free(bank);
}

int snd_play_sound(int ch, const int no, const sound_mode_t mode) {
  if (mode != SOUND_MODE_STOP) {
    // play or loop
    ASSERT(snd_main_bank);
    ASSERT(no > 0 && no < snd_main_bank->num_sfx);
    if (ch < 0) {
      ch = chan_next;
      if (++chan_next == CHAN_MUSIC)
        chan_next = CHAN_ALLOC;
    }
    spu_play_sample(ch, snd_main_bank->sfx_addr[no], SFX_FREQ);
    sample_chan[no] = ch;
    chan_sample[ch] = no;
  } else {
    // stop
    if (ch < 0) {
      ASSERT(no > 0 && no < MAX_SFX);
      spu_key_off(SPU_VOICECH(sample_chan[no]));
    } else {
      spu_key_off(SPU_VOICECH(ch));
    }
  }
  return ch;
}
