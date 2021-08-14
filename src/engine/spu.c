#include "spu.h"

#include <psxspu.h>

#include "engine/common.h"

#define SPU_VOICE_BASE ((volatile u16 *)(0x1F801C00))
#define SPU_KEY_ON_LO ((volatile u16 *)(0x1F801D88))
#define SPU_KEY_ON_HI ((volatile u16 *)(0x1F801D8A))
#define SPU_KEY_OFF_LO ((volatile u16 *)(0x1F801D8C))
#define SPU_KEY_OFF_HI ((volatile u16 *)(0x1F801D8E))
#define SPU_KEY_END ((volatile u32 *)(0x1F801D9C))
#define DMA_BASE ((volatile u32 *)(0x1F801080))

struct spu_voice {
  volatile s16 vol_left;
  volatile s16 vol_right;
  volatile u16 sample_rate;
  volatile u16 sample_startaddr;
  volatile u16 attack_decay;
  volatile u16 sustain_release;
  volatile u16 vol_current;
  volatile u16 sample_repeataddr;
};
#define SPU_VOICE(x) (((volatile struct spu_voice *)SPU_VOICE_BASE) + (x))

struct dma_regs {
  volatile u32 madr;
  volatile u32 bcr;
  volatile u32 chcr;
  volatile u32 pad;
};
#define DMA_CTRL(x) (((volatile struct dma_regs *)DMA_BASE) + (x))
#define DMA_CTRL_SPU 4

#define PAN_SHIFT 8

u32 spuram_ptr = SPU_RAM_START;

// saved state for stop/play
static struct {
  u32 addr;
  s16 vol;  // 0 to SPU_MAX_VOLUME
  s16 pan;  // -255 to 255
  u16 freq;
  u16 dirty;
} voice_state[SPU_NUM_VOICES];

void spu_init(void) {
  SpuInit();
  spu_clear_all_voices();
  spuram_ptr = SPU_RAM_START;
}

void spu_key_on(const u32 mask) {
  *SPU_KEY_ON_LO = mask;
  *SPU_KEY_ON_HI = mask >> 16;
}

void spu_key_off(const u32 mask) {
  *SPU_KEY_OFF_LO = mask;
  *SPU_KEY_OFF_HI = mask >> 16;
}

void spu_clear_voice(const u32 v) {
  SPU_VOICE(v)->vol_left = 0;
  SPU_VOICE(v)->vol_right = 0;
  SPU_VOICE(v)->sample_rate = 0;
  SPU_VOICE(v)->sample_startaddr = 0;
  SPU_VOICE(v)->sample_repeataddr = 0;
  SPU_VOICE(v)->attack_decay = 0x000F;
  SPU_VOICE(v)->sustain_release = 0x0000;
  SPU_VOICE(v)->vol_current = 0;
  voice_state[v].vol = 0;
  voice_state[v].pan = 0;
  voice_state[v].addr = 0;
  voice_state[v].freq = 0;
  voice_state[v].dirty = 0;
}

void spu_clear_all_voices(void) {
  spu_key_off(0xFFFFFFFF);
  for (u32 i = 0; i < SPU_NUM_VOICES; ++i)
    spu_clear_voice(i);
}

void spu_set_voice_volume(const u32 v, const s16 vol) {
  voice_state[v].vol = vol;
  voice_state[v].dirty = 1;
}

void spu_set_voice_pan(const u32 v, const s16 pan) {
  voice_state[v].pan = pan;
  voice_state[v].dirty = 1;
}

void spu_set_voice_freq(const u32 v, const u32 hz) {
  voice_state[v].freq = freq_to_pitch(hz);
  voice_state[v].dirty = 1;
}

void spu_set_voice_pitch(const u32 v, const u32 pitch) {
  voice_state[v].freq = pitch;
  voice_state[v].dirty = 1;
}

void spu_set_voice_addr(const u32 v, const u32 addr) {
  voice_state[v].addr = (addr >> 3);
  voice_state[v].dirty = 1;
}

u32 spu_get_voice_end_mask(void) {
  return *SPU_KEY_END;
}

static inline void spu_update_voice_volume(const u32 v) {
  int vol_left = voice_state[v].vol;
  int vol_right = vol_left;
  const int pan = voice_state[v].pan;
  if (pan < 0)
    vol_right = (vol_right * -pan) >> PAN_SHIFT;
  else if (pan > 0)
    vol_left = (vol_left * pan) >> PAN_SHIFT;
  SPU_VOICE(v)->vol_left = vol_left;
  SPU_VOICE(v)->vol_right = vol_right;
}

void spu_flush_voices(void) {
  SpuWait();
  for (int v = 0; v < SPU_NUM_VOICES; ++v) {
    if (voice_state[v].dirty) {
      voice_state[v].dirty = 0;
      spu_update_voice_volume(v);
      SPU_VOICE(v)->sample_rate = voice_state[v].freq;
      SPU_VOICE(v)->sample_startaddr = voice_state[v].addr;
    }
  }
}

void spu_play_sample(const u32 ch, const u32 addr, const u32 freq) {
  // quiet down to ease transfer
  SPU_VOICE(ch)->vol_left = 0;
  SPU_VOICE(ch)->vol_right = 0;
  voice_state[ch].freq = freq_to_pitch(freq);
  voice_state[ch].addr = (addr >> 3);
  SPU_VOICE(ch)->sample_rate = voice_state[ch].freq;
  SPU_VOICE(ch)->sample_startaddr = voice_state[ch].addr;
  spu_key_on(SPU_VOICECH(ch)); // this restarts the channel on the new address
  spu_update_voice_volume(ch); // restore volume
  voice_state[ch].dirty = 0; // make sure next flush call doesn't touch it
}

void spu_wait_for_transfer(void) {
  while ((DMA_CTRL(DMA_CTRL_SPU)->chcr & 0x01000000) != 0) { }
  SpuWait();
}
