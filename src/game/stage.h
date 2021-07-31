#pragma once

#include "engine/common.h"

#define STAGE_PATH_FORMAT "0\\STAGE00.STG;1"
#define STAGE_PATH_PREFIX "\\"
#define STAGE_PATH_START  1

#define MAX_TILESET_SIZE 16
#define MAX_STAGE_LINKS 4
#define MAX_STAGE_TITLE 32

#define MAX_STAGES 100

#pragma pack(push, 1)

typedef struct {
  s16 x;
  s16 y;
  s16 event_flag;
  s16 event_num;
  s16 class_num;
  u16 bits;
} stage_event_t;

typedef struct {
  u32 id;         // map identifier (index in the maplist, starting with 0)
  s16 width;      // width in tiles
  s16 height;     // height in tiles
  s16 bk_type;    // background type
  s16 boss_type;  // boss type
  u16 ev_count;   // number of "event" structs in pxe contents
  u16 ev_offset;  // offset from map_data[] to pxe contents
  u16 tsc_offset; // offset from map_data[] to decoded tsc data
  u16 tsc_size;   // size of the tsc data in bytes
  char title[MAX_STAGE_TITLE]; // title that displays on the automap and/or when you enter
  u8 atrb[MAX_TILESET_SIZE * MAX_TILESET_SIZE]; // pxa contents
  u8 map_data[];  // [width * height] pxm data
  // [StageEvent[ev_count] follows]
  // [decoded tsc data follows]
} stage_t;

typedef struct {
  u32 music_id; // in-game song id (index in musiclist.h)
  u32 bank_ofs; // offset to sfx_bank + ORG data from start of stage bank
} stage_song_t;

typedef struct {
  u16 numstages; // number of stages in bank (up to MAX_STAGE_LINKS)
  u16 numsongs;  // number of sfx_bank + ORG combos (up to MAX_STAGE_LINKS)
  u32 surfofs;   // offset to surface bank at the end
  u32 stageofs[MAX_STAGE_LINKS]; // offsets to each stage and its data (or 0 if unused)
  stage_song_t songs[]; // [numsongs] offsets to each sfx_bank + ORG combo
  // [Stage[numstages] follows]
  // [sfx_bank_t+orgdata[numsongs] follows]
  // [gfx_bank_t follows]
} stage_bank_t;

#pragma pack(pop)

enum bktype {
  BACKGROUND_TYPE_STATIONARY   = 0, // Doesn't move at all
  BACKGROUND_TYPE_MOVE_DISTANT = 1, // Moves at half the speed of the foreground
  BACKGROUND_TYPE_MOVE_NEAR    = 2, // Moves at the same speed as the foreground
  BACKGROUND_TYPE_WATER        = 3, // No background - draws a water foreground layer instead
  BACKGROUND_TYPE_BLACK        = 4, // No background - just black
  BACKGROUND_TYPE_AUTOSCROLL   = 5, // Constantly scrolls to the left (used by Ironhead)
  BACKGROUND_TYPE_CLOUDS_WINDY = 6, // Fancy parallax scrolling, items are blown to the left (used by bkMoon)
  BACKGROUND_TYPE_CLOUDS       = 7  // Fancy parallax scrolling (used by bkFog)
};

extern stage_t *stage_data;
extern int stage_water_y;

void stage_init(void);

int stage_load_stage_bank(const u32 id);
void stage_free_stage_bank(void);

// changes current stage to `id`, executes `event` and puts player at `plr_x`, `plr_y`
// if stage `id` is not in the current stage bank, will change bank to `id`
int stage_transition(const u32 id, const u32 event, int plr_x, int plr_y);

void stage_draw(int cam_x, int cam_y);

static inline u8 stage_get_atrb(const int x, const int y) {
  if (x < 0 || y < 0 || x >= stage_data->width || y >= stage_data->height)
    return 0;
  const u8 t = stage_data->map_data[y * stage_data->width + x];
  return stage_data->atrb[t];
}

static inline void stage_shift_tile(const int x, const int y) {
  --stage_data->map_data[y * stage_data->width + x];
}

int stage_set_tile(const int x, const int y, const int t);
