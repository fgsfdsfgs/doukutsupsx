#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "engine/common.h"
#include "engine/memory.h"
#include "engine/sound.h"
#include "engine/spu.h"
#include "engine/org.h"

#define ORG_MAGIC "Org-0"
#define ORG_MAGICLEN 5
#define ORG_INVALID 0xFFFF

#define MAX_TRACKS ORG_MAX_TRACKS
#define MAX_MELODY_TRACKS 8
#define MAX_DRUM_TRACKS 8

#define NUM_OCTS 8
#define NUM_ALTS 2

#define DRUM_BANK_BASE 150

#define PANDUMMY 0xFF
#define VOLDUMMY 0xFF
#define KEYDUMMY 0xFF

#define ALLOCNOTE 8192

#define DEFVOLUME 200
#define DEFPAN    6

#pragma pack(push, 1)

typedef struct {
  u16 freq;     // frequency modifier (default = 1000)
  u8 wave_no;   // waveform index in the wavetable
  u8 pipi;      // loop flag?
  u16 note_num; // number of notes in track
} org_trackhdr_t;

typedef struct {
  u16 wait;
  u8 line;
  u8 dot;
  s32 repeat_x;
  s32 end_x;
  org_trackhdr_t tdata[MAX_TRACKS];
} org_hdr_t;

#pragma pack(pop)

typedef struct {
  org_note_t *notes;
  org_note_t *cur_note;
  s32 vol;
  u32 sustain;
  s8 mute;
  u8 old_key;
} org_trackstate_t;

typedef struct {
  org_hdr_t info;
  org_trackstate_t tracks[MAX_TRACKS];
  u32 id;
  s32 vol;
  s32 pos;
  s32 paused;
  u8 fadeout;
  s8 track;
  u8 def_pan;
  u8 def_vol;
} org_state_t;

static u32 key_on_mask; // all the keys that got keyed on this tick
static u32 key_off_mask; // all the keys that got keyed off this tick
static org_state_t org;
static sfx_bank_t *inst_bank;

static org_note_t notebuf[ALLOCNOTE];

static const struct {
  s16 wave_size;
  s16 oct_par;
  s16 oct_size;
} oct_wave[NUM_OCTS] = {
  { 256,   1,  4 }, // 0 Oct
  { 256,   2,  8 }, // 1 Oct
  { 128,   4, 12 }, // 2 Oct
  { 128,   8, 16 }, // 3 Oct
  {  64,  16, 20 }, // 4 Oct
  {  32,  32, 24 }, // 5 Oct
  {  16,  64, 28 }, // 6 Oct
  {   8, 128, 32 }, // 7 Oct
};

static const s16 freq_tbl[12] = { 262, 277, 294, 311, 330, 349, 370, 392, 415, 440, 466, 494 };
static const s16 pan_tbl[13] = { 0, 43, 86, 129, 172, 215, 256, 297, 340, 383, 426, 469, 512 };

static const inline u8 *org_read_track(const u8 *ptr, const int track) {
  const org_trackhdr_t *hdr = &org.info.tdata[track];
  org_trackstate_t *dst = &org.tracks[track];
  // read positions ("x coordinate")
  for (u16 i = 0; i < hdr->note_num; ++i, ptr += sizeof(s32)) {
    // don't risk alignment memes
    dst->notes[i].pos = ptr[0] | (ptr[1] << 8) |
      (ptr[2] << 16) | (ptr[3] << 24);
  }
  // read keys ("y coordinate")
  for (u16 i = 0; i < hdr->note_num; ++i, ++ptr)
    dst->notes[i].key = *ptr;
  // read lengths
  for (u16 i = 0; i < hdr->note_num; ++i, ++ptr)
    dst->notes[i].len = *ptr;
  // read volumes
  for (u16 i = 0; i < hdr->note_num; ++i, ++ptr)
    dst->notes[i].vol = *ptr;
  // read pans
  for (u16 i = 0; i < hdr->note_num; ++i, ++ptr)
    dst->notes[i].pan = *ptr;
  return ptr;
}

void org_init(void) {
  org.def_pan = DEFPAN;
  org.def_vol = DEFVOLUME;
  org.id = ORG_INVALID;
  // we'll keep the SPU addresses of the currently playing song until it gets reloaded
  inst_bank = mem_alloc(sizeof(*inst_bank) + 64 * sizeof(inst_bank->sfx_addr[0]));
}

bool org_load(const u32 id, u8 *data, sfx_bank_t *bank) {
  if (id == org.id)
    return TRUE; // already loaded

  if (memcmp(data, ORG_MAGIC, ORG_MAGICLEN))
    panic("org_load: invalid Org magic: %02x%02x%02x%02x", data[0], data[1], data[2], data[3], data[4]);

  const int ver = (char)data[ORG_MAGICLEN] - '0';
  if (ver != 1 && ver != 2)
    panic("org_load: expected version 1 or 2, got %d\n", ver);

  memcpy(&org.info, &data[ORG_MAGICLEN + 1], sizeof(org.info));

  const u8 *srcptr = &data[ORG_MAGICLEN + 1 + sizeof(org.info)];
  org_note_t *dstptr = notebuf;
  for (int i = 0; i < MAX_TRACKS; ++i) {
    if (org.info.tdata[i].note_num) {
      org.tracks[i].notes = dstptr;
      srcptr = org_read_track(srcptr, i);
      dstptr += org.info.tdata[i].note_num;
    } else {
      org.tracks[i].notes = NULL;
    }
  }

  const u32 numnotes = dstptr - notebuf;
  ASSERT(numnotes < ALLOCNOTE);
  printf("org_load: total notes: %u / %u\n", numnotes, ALLOCNOTE);

  org.id = id;
  org.vol = 100;

  // store SPU addresses of this song and upload sample data
  memcpy(inst_bank, bank, sizeof(*bank) + sizeof(u32) * bank->num_sfx);
  snd_upload_sfx_bank(bank, NULL);

  return TRUE;
}

void org_free(void) {
  if (org.id == ORG_INVALID)
    return;

  org_pause(TRUE);

  // do it in reverse cause stack allocator
  for (int i = 0; i < MAX_TRACKS; ++i)
    org.tracks[i].notes = NULL;

  // only free the SPU data
  snd_free_sfx_bank_data(inst_bank);

  org.id = ORG_INVALID;
  org.fadeout = 0;
}

void org_pause(const bool new_paused) {
  if (org.paused != new_paused) {
    org_set_mute_mask(new_paused ? 0xFFFF : 0);
    org.paused = new_paused;
  }
}

void org_start_fade(void) {
  org.fadeout = 1;
}

void org_restart_from(const s32 pos) {
  org.pos = pos;
  org.fadeout = 0;
  for (int i = 0; i < MAX_TRACKS; ++i) {
    org.tracks[i].cur_note = NULL;
    for (int j = 0; j < org.info.tdata[i].note_num; ++j) {
      if (org.tracks[i].notes[j].pos >= pos) {
        org.tracks[i].cur_note = &org.tracks[i].notes[j];
        break;
      }
    }
  }
  org_pause(FALSE);
}

static inline void org_play_melodic(const int trk, int key, int freq, int mode) {
  const u32 ch = ORG_START_CH + trk;
  const int oct = key / 12;
  const int inst = trk * NUM_OCTS + oct;
  switch (mode) {
    case 0: // also stop?
    case 2: // stop
      if (org.tracks[trk].old_key != KEYDUMMY) {
        key_off_mask |= SPU_VOICECH(ch);
        org.tracks[trk].old_key = KEYDUMMY;
      }
      break;
    case -1: // key on?
      org.tracks[trk].old_key = key;
      freq = ((oct_wave[oct].wave_size * freq_tbl[key % 12]) * oct_wave[oct].oct_par) / 8 + (freq - 1000);
      spu_set_voice_addr(ch, inst_bank->sfx_addr[inst]);
      spu_set_voice_freq(ch, freq);
      key_on_mask |= SPU_VOICECH(ch);
      break;
    default:
      break;
  }
}

static inline void org_play_drum(const int trk, int key, int mode) {
  const u32 ch = ORG_START_CH + trk;
  const int inst = trk - MAX_MELODY_TRACKS + DRUM_BANK_BASE;
  switch (mode) {
    case 0: // stop
      key_off_mask |= SPU_VOICECH(ch);
      break;
    case 1: // play
      spu_set_voice_addr(ch, snd_main_bank->sfx_addr[inst]);
      spu_set_voice_freq(ch, key * 800 + 100);
      key_on_mask |= SPU_VOICECH(ch);
      break;
    default:
      break;
  }
}

static inline void org_set_vol(const int trk, int vol) {
  spu_set_voice_volume(ORG_START_CH + trk, vol << 5);
}

static inline void org_set_pan(const int trk, int pan) {
  spu_set_voice_pan(ORG_START_CH + trk, pan_tbl[pan] - 256);
}

void org_tick(void) {
  if (org.paused)
    return;

  if (org.fadeout) {
    org.vol -= 2;
    if (org.vol <= 0) {
      org_pause(TRUE);
      org.vol = 0;
      org.fadeout = 0;
    }
  }

  key_off_mask = 0;
  key_on_mask = 0;

  // waves
  for (int i = 0; i < MAX_MELODY_TRACKS; ++i) {
    const org_note_t *note = org.tracks[i].cur_note;
    if (note && org.pos == note->pos) {
      if (!org.tracks[i].mute && note->key != KEYDUMMY) {
        org_play_melodic(i, note->key, org.info.tdata[i].freq, -1);
        org.tracks[i].sustain = note->len;
      }
      if (note->pan != PANDUMMY)
        org_set_pan(i, note->pan);
      if (note->vol != VOLDUMMY)
        org.tracks[i].vol = note->vol;
      ++org.tracks[i].cur_note;
      if (org.tracks[i].cur_note >= org.tracks[i].notes + org.info.tdata[i].note_num)
        org.tracks[i].cur_note = NULL;
    }

    if (org.tracks[i].sustain == 0)
      org_play_melodic(i, 0, org.info.tdata[i].freq, 2);
    else
      --org.tracks[i].sustain;

    if (org.tracks[i].cur_note)
      org_set_vol(i, org.tracks[i].vol * org.vol / 0x7F);
  }

  // drums
  for (int i = MAX_MELODY_TRACKS; i < MAX_TRACKS; ++i) {
    const org_note_t *note = org.tracks[i].cur_note;
    if (note && org.pos == note->pos) {
      if (!org.tracks[i].mute && note->key != KEYDUMMY)
        org_play_drum(i, note->key, 1);
      if (note->pan != PANDUMMY)
        org_set_pan(i, note->pan);
      if (note->vol != VOLDUMMY)
        org.tracks[i].vol = note->vol;
      ++org.tracks[i].cur_note;
      if (org.tracks[i].cur_note >= org.tracks[i].notes + org.info.tdata[i].note_num)
        org.tracks[i].cur_note = NULL;
    }

    if (org.tracks[i].cur_note)
      org_set_vol(i, org.tracks[i].vol * org.vol / 0x7F);
  }

  spu_flush_voices();
  spu_key_off(key_off_mask);
  spu_key_on(key_on_mask);

  ++org.pos;
  if (org.pos >= org.info.end_x)
    org_restart_from(org.info.repeat_x);
}

int org_get_wait(void) {
  return (org.id == ORG_INVALID) ? 0 : org.info.wait;
}

int org_get_pos(void) {
  return org.pos;
}

u32 org_get_mute_mask(void) {
  register u32 mask = 0;
  for (u32 i = 0; i < MAX_TRACKS; ++i) {
    if (org.tracks[i].mute)
      mask |= (1 << i);
  }
  return mask;
}

u32 org_set_mute_mask(const u32 mask) {
  register u32 oldmask = 0;
  for (u32 i = 0; i < MAX_TRACKS; ++i) {
    if (org.tracks[i].mute)
      oldmask |= (1 << i);
    org.tracks[i].mute = !!(mask & (1 << i));
  }
  spu_key_off(mask << ORG_START_CH);
  return oldmask;
}

org_note_t *org_get_track(const int tracknum, u32 *numnotes) {
  if (numnotes)
    *numnotes = org.info.tdata[tracknum].note_num;
  return org.tracks[tracknum].notes;
}

org_note_t *org_get_track_pos(const int tracknum) {
  return org.tracks[tracknum].cur_note;
}

u32 org_get_id(void) {
  return org.id;
}
