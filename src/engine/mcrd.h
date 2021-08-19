#pragma once

#include "engine/common.h"

// max size of one profile
#define MCRD_MAX_SAVE_SIZE 1536
// max number of profiles in one save file
#define MCRD_MAX_SAVES 4
// max number of memcards per port (TODO: multitap support?)
#define MCRD_CARDS_PER_PORT 1
// max ports
#define MCRD_NUM_PORTS 2

#define MCRD_SAVE_ICON_FRAMES 3

#define MCRD_SECSIZE 128
#define MCRD_BLOCKSIZE 8192

#pragma pack(push, 1)

typedef struct {
  char id[2];    // "SC"
  u8 type;       // number of icon frames (0x11 - one frame, 0x12 - two frames, 0x13 - three frames)
  u8 size;       // size of save file in blocks
  u16 title[32]; // title of save file (encoded in Shift-JIS format)
  u8 pad[28];    // unused
  u16 clut[16];  // color palette of icon frames (16 RGB5X1 16-bit color entries)
  // save icon frames follow
} SAVEHDR;

typedef struct {
  char id[4];    // "CSS\x01"
  u32 slotmask;  // bit N is 1 if slot N is occupied
  u8 pad[MCRD_SECSIZE - 4 * 2];
  u8 data[];     // [MCRD_MAX_SAVES][MCRD_MAX_SAVE_SIZE] 
} mcrd_save_t;

// save structure:
// SAVEHDR savehdr; 1 + NUM_FRAMES sectors
// mcrd_save_t hdr; 1 sector
// u8 data[];       (MCRD_MAX_SAVE_SIZE / MCRD_SECSIZE) * MCRD_MAX_SAVES sectors

#pragma pack(pop)

typedef enum {
  MCRD_NO_SPACE = -5,
  MCRD_NO_SAVE = -4,
  MCRD_WRONG_CARD = -3,
  MCRD_NO_CARD = -2,
  MCRD_ERROR = -1,
  MCRD_SUCCESS,
  MCRD_UNFORMATTED,
} mcrd_result_t;

typedef struct {
  u8 port;
  u8 card;
} mcrd_id_t;

void mcrd_init(void);
void mcrd_start(void);
void mcrd_stop(void);

int mcrd_cards_available(mcrd_id_t *out_cards);
mcrd_result_t mcrd_card_open(const mcrd_id_t id);
mcrd_result_t mcrd_card_close(void);
mcrd_result_t mcrd_card_format(void);

u32 mcrd_save_slots_available(void);
mcrd_result_t mcrd_save_open(const char *name);
mcrd_result_t mcrd_save_create(const char *name);
mcrd_result_t mcrd_save_write_slot(const int slot, const void *data, const int size);
mcrd_result_t mcrd_save_read_slot(const int slot, void *data, const int size);
