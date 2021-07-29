#pragma once

#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>

#include "surface.h"

#define MAX_STAGES 256
#define MAX_STAGE_TITLE 32
#define MAX_STAGE_LINKS 4
#define MAX_STAGE_SONGS 8
#define MAX_STAGE_SURFACES 8
#define MAX_STAGE_SIZE 1024
#define MAX_STAGELIST_LINKS 32
#define MAX_RES_NAME 16
#define MAX_TILESET_SIZE 16
#define MAX_TSC_SIZE 0x5000 // in-game limitation

#define MIN_STAGE_WIDTH 21
#define MIN_STAGE_HEIGHT 16

#pragma pack(push, 1)

typedef struct {
  int16_t x;
  int16_t y;
  int16_t code_flag;
  int16_t code_event;
  int16_t code_char;
  uint16_t bits;
} stage_event_t;

typedef struct {
  uint32_t id;         // map identifier (index in the maplist, starting with 0)
  int16_t width;       // width in tiles
  int16_t height;      // height in tiles
  int16_t bk_type;     // background type
  int16_t boss_type;   // boss type
  uint16_t ev_count;   // number of "event" structs in pxe contents
  uint16_t ev_offset;  // offset from map_data[] to pxe contents
  uint16_t tsc_offset; // offset from map_data[] to decoded tsc data
  uint16_t tsc_size;   // size of the tsc data in bytes
  char title[MAX_STAGE_TITLE]; // title that displays on the automap and/or when you enter
  uint8_t atrb[MAX_TILESET_SIZE][MAX_TILESET_SIZE]; // pxa contents
  uint8_t map_data[]; // [width * height] pxm data
  // [pxe data follows]
  // [decoded tsc data follows]
} stage_t;

typedef struct {
  uint32_t music_id; // in-game song id (index in musiclist.h)
  uint32_t bank_ofs; // offset to sfx_bank + ORG data from start of stage bank
} stage_song_t;

typedef struct {
  uint16_t numstages;   // number of stages in bank (up to MAX_STAGE_LINKS)
  uint16_t numsongs;    // number of sfx_bank + ORG combos (up to MAX_STAGE_LINKS)
  uint32_t surfofs;     // offset to surface bank at the end
  uint32_t stageofs[MAX_STAGE_LINKS]; // offsets to each stage_t and its data (or 0 if unused)
  stage_song_t songs[]; // [numsongs] offsets to each sfx_bank + ORG combo
  // followed by:
  //
  // stage_t stage 0
  // stage 0 data
  // ...
  // stage_t stage (numstages - 1)
  // stage (numstages-1) data
  //
  // sfx_bank bank 0
  // org_file org 0
  // ...
  // sfx_bank bank (numsongs - 1)
  // org_file org (numsongs - 1)
  //
  // vram_surfbank_t
  // VRAM bank data
} stagebank_t;

#pragma pack(pop)

typedef struct {
  char name[MAX_RES_NAME];
  char title[MAX_STAGE_TITLE];
  char tilesheet[MAX_RES_NAME];
  char npcsheet[MAX_RES_NAME];
  char bosssheet[MAX_RES_NAME];
  char bksheet[MAX_RES_NAME];
  uint32_t songs[MAX_STAGE_SONGS];
  uint32_t links[MAX_STAGELIST_LINKS];
  int bktype;
  int bossnum;
  int numsongs;
  int numsurfaces;
  int numlinks;
  bool packed;
  stage_t *stage;
  surf_list_t surfaces[MAX_STAGE_SURFACES];
} stage_list_t;

int read_stagelist(stage_list_t *list, FILE *f);

stage_t *stage_load(const uint32_t id, const char *prefix, const char *tileprefix);

// scan the stage's tsc for music changes
int stage_scan_music(const stage_t *stage, uint32_t *songlist);

// scan the stage's tsc for level transitions
int stage_scan_transitions(const stage_t *stage, uint32_t *linklist);

uint32_t stage_write_bank(const stage_list_t *root, const stage_list_t *stlist, const char *datapath, FILE *f);

bool stage_load_tsc_head(const char *path);
