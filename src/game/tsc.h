#pragma once

#include "engine/common.h"

#define TSC_MAX_SIZE 0x5000
#define TSC_MAX_LINES 4
#define TSC_MAX_NUMBERS 4
#define TSC_MAX_LOADED_SCRIPTS 4
#define TSC_LINE_LEN 35

#define TEXT_LEFT (VID_WIDTH / 2 - 108)
#define TEXT_BOX_LEFT (VID_WIDTH / 2 - 122)
#define TEXT_TOP (VID_HEIGHT - 56)

enum tsc_script_id {
  TSC_SCRIPT_ARMS_ITEM,
  TSC_SCRIPT_STAGE_SELECT,
  TSC_SCRIPT_CREDITS,
  TSC_SCRIPT_STAGE,
};

enum tsc_mode {
  TSC_MODE_OFF = 0,
  TSC_MODE_PARSE = 1,
  TSC_MODE_NOD = 2,
  TSC_MODE_NEWLINE = 3,
  TSC_MODE_WAIT = 4,
  TSC_MODE_FADE = 5,
  TSC_MODE_YESNO = 6,
  TSC_MODE_WAIT_PLAYER = 7,
};

enum tsc_flags {
  TSCFLAG_IN_STRING = 2048,
};

#pragma pack(push, 1)

typedef struct {
  u16 id;  // event number (the 4 digits after the '#')
  u16 ofs; // offset to event code from start of tsc_script_t
} tsc_event_t;

typedef struct {
  u32 num_ev;           // number of events in script
  tsc_event_t ev_map[]; // [num_ev]
  // compiled tsc bytecode follows
} tsc_script_t;

#pragma pack(pop)

typedef struct {
  tsc_script_t *script; // current script buffer
  u32 size;             // script buffer size

  u8 *readptr;          // read position in script

  s16 flags;
  s16 writepos;         // x position in line
  s16 line;             // current line
  s16 line_y[TSC_MAX_LINES];
  s16 face;
  s16 face_x;
  s16 item;
  s16 item_y;
  s16 text_x;
  s16 text_y;
  s16 num[TSC_MAX_NUMBERS];

  s16 wait;
  s16 wait_next;
  s16 next_event;

  s8 mode;              // current mode (e.g. NOD, WAI)
  s8 yesno;             // selection in yes/no prompt
  u8 blink;             // cursor blink
} tsc_state_t;

extern tsc_state_t tsc_state;

void tsc_init(void);
bool tsc_update(void);
void tsc_draw(void);
void tsc_set_stage_script(tsc_script_t *data, const u32 size);
void tsc_switch_script(const int idx);
void tsc_start_event(const int num);
void tsc_jump_event(const int num);
void tsc_stop_event(void);
void tsc_clear_text(void);