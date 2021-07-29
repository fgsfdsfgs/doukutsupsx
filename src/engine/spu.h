#pragma once

#include <psxspu.h>
#include "engine/common.h"

#define SPU_NUM_VOICES 24
#define SPU_MAX_VOLUME 0x3FFF
#define SPU_RAM_START 0x1000

extern u32 spuram_ptr;

void spu_init(void);
void spu_key_on(const u32 mask);
void spu_key_off(const u32 mask);
void spu_clear_voice(const u32 v) ;
void spu_set_voice_volume(const u32 v, const s16 vol);
void spu_set_voice_pan(const u32 v, const s16 pan);
void spu_set_voice_freq(const u32 v, const u32 hz);
void spu_set_voice_pitch(const u32 v, const u32 pitch);
void spu_set_voice_addr(const u32 v, const u32 addr);
void spu_flush_voices(void);
void spu_play_sample(const u32 ch, const u32 addr, const u32 freq);
void spu_wait_for_transfer(void);
void spu_clear_all_voices(void);

static inline u16 freq_to_pitch(const u32 hz) {
  return (hz << 12) / 44100;
}

// unfortunately the psn00bsdk function for this is bugged:
// it checks against 0x1000..0xffff instead of 0x1000..0x7ffff
// fortunately, the address is stored in a global variable
// unfortunately, reading it from C requires GP-relative addressing
// so we have to implement the function in assembly (see spu.s)
extern u32 spu_set_transfer_addr(u32 addr);
