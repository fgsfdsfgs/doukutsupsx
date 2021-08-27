#include <stdio.h>
#include <string.h>

#include "engine/common.h"
#include "engine/graphics.h"
#include "engine/org.h"
#include "engine/filesystem.h"
#include "engine/memory.h"
#include "engine/input.h"

#include "game/game.h"
#include "game/stage.h"
#include "game/npc.h"
#include "game/tsc.h"
#include "game/camera.h"
#include "game/player.h"
#include "game/profile.h"
#include "game/menu.h"
#include "game/credits.h"

#define CREDITS_MAX_CAST ((VID_HEIGHT / 16) + 1)
#define CREDITS_BANK "\\MAIN\\CREDIT.CRD;1"
#define CREDITS_SCRIPT "\\MAIN\\CREDIT.BSC;1"

enum img_act {
  IMG_ACT_IDLE,
  IMG_ACT_SLIDE_IN,
  IMG_ACT_SLIDE_OUT,
};

enum csc_mode {
  CSC_MODE_OFF,
  CSC_MODE_PARSE,
  CSC_MODE_WAIT,
};

static struct {
  s32 mode;
  u8 *data;
  u8 *readptr;
  const char *laststr;
  s32 wait;
  s32 cast_x;
  s32 img;
  s32 img_x;
  s32 img_act;
} csc_state;

typedef struct {
  gfx_texrect_t texrect;
  const char *text;
  bool show;
  s32 x;
  s32 y;
} cast_t;

static cast_t cast_list[CREDITS_MAX_CAST];

// might drop the name later, not really required
typedef struct {
  char name;
  u8 num_args;
} csc_opcode_t;

static const csc_opcode_t csc_optab[] = {
  { 'l', 1 }, { '+', 1 }, { '!', 1 }, { '-', 1 },
  { 'f', 2 }, { 'j', 1 }, { '~', 0 }, { '/', 0 },
  { '[', 0 }, { ']', 1 },
};

#define NUM_OPCODES (sizeof(tsc_optab) / sizeof(*tsc_optab))
#define BASE_OPCODE 0x80

static gfx_rambank_t *cbank;
static gfx_texrect_t rc_illust = {{ 0, 0, 160, 240 }};

void credits_start(void) {
  // show loading screen while loading the credits bank
  gfx_draw_loading();

  memset_word(cast_list, 0, sizeof(cast_list));
  memset_word(&csc_state, 0, sizeof(csc_state));

  fs_file_t *f = fs_fopen(CREDITS_SCRIPT, FALSE);
  if (!f) PANIC("could not open\n" CREDITS_SCRIPT);
  int fsize = fs_fsize(f);
  tsc_script_t *csc = mem_alloc(fsize);
  fs_fread_or_die(csc, fsize, 1, f);
  fs_fclose(f);

  f = fs_fopen(CREDITS_BANK, FALSE);
  if (!f) PANIC("could not open\n" CREDITS_BANK);
  fsize = fs_fsize(f);
  cbank = mem_alloc(fsize);
  fs_fread_or_die(cbank, fsize, 1, f);
  fs_fclose(f);

  // upload the cast surface into the bottom right corner
  const gfx_ramsurf_t *s = &cbank->surf[0];
  gfx_surf[SURFACE_ID_CASTS].id = SURFACE_ID_CASTS;
  gfx_surf[SURFACE_ID_CASTS].tex_x = 1024 - s->w / 2;
  gfx_surf[SURFACE_ID_CASTS].tex_y = 512 - s->h;
  gfx_upload_image((u8 *)cbank + s->ofs, s->w, s->h, s->mode, SURFACE_ID_CASTS, FALSE);

  // upload the first credits image right away to the left of CASTS
  s = &cbank->surf[1];
  gfx_surf[SURFACE_ID_CREDITS_IMAGE].id = SURFACE_ID_CREDITS_IMAGE;
  gfx_surf[SURFACE_ID_CREDITS_IMAGE].tex_x = 1024 - 256;
  gfx_surf[SURFACE_ID_CREDITS_IMAGE].tex_y = 512 - s->h - 1; // 1 is for the CLUT
  gfx_upload_image((u8 *)cbank + s->ofs, s->w, s->h, s->mode, SURFACE_ID_CREDITS_IMAGE, TRUE);

  csc_state.data = csc_state.readptr = (u8 *)csc + csc->ev_map[0].ofs;
  csc_state.img = 1;
  csc_state.img_x = TO_FIX(-160);
  csc_state.img_act = IMG_ACT_IDLE;
  csc_state.mode = CSC_MODE_PARSE;
  gfx_set_texrect(&rc_illust, SURFACE_ID_CREDITS_IMAGE);

  // move hi mark to after the credits bank so credits stages would load after it
  mem_set_mark(MEM_MARK_HI);

  // clear camera fade
  cam_clear_fade();

  // don't show loading screens for the duration of the credits
  gfx_suppress_loading = TRUE;
  game_flags |= GFLAG_SHOW_CREDITS;

  // lock player
  player.cond |= PLRCOND_INVISIBLE;
}

void credits_stop(void) {
  // stop the level script
  tsc_stop_event();
  // fade music if we haven't already
  org_start_fade();

  // free credits data, restore hi mark to lo mark value
  mem_free_to_mark(MEM_MARK_LO);
  mem_set_mark(MEM_MARK_HI);

  csc_state.readptr = NULL;
  csc_state.data = NULL;
  csc_state.img = 0;
  csc_state.mode = CSC_MODE_OFF;
  cbank = NULL;

  gfx_suppress_loading = FALSE;
  game_flags &= ~GFLAG_SHOW_CREDITS;

  // ask the user to save
  profile_save();
  menu_open(MENU_SAVE);
}

static void credits_new_cast(const int id, const char *text) {
  int n = 0;
  while (n < CREDITS_MAX_CAST && cast_list[n].show)
    ++n;
  if (n == CREDITS_MAX_CAST)
    return;
  cast_t *c = &cast_list[n];
  c->show = TRUE;
  c->text = text;
  c->x = csc_state.cast_x;
  c->y = TO_FIX(VID_HEIGHT + 8);
  c->texrect.r.x = (id % 13) * 24;
  c->texrect.r.y = (id / 13) * 24;
  c->texrect.r.right = c->texrect.r.x + 24;
  c->texrect.r.bottom = c->texrect.r.y + 24;
  gfx_set_texrect(&c->texrect, SURFACE_ID_CASTS);
}

static inline void credits_update_cast(void) {
  for (int i = 0; i < CREDITS_MAX_CAST; ++i) {
    cast_t *cast = &cast_list[i];
    if (cast->show) {
      cast->y -= FIX_SCALE / 2;
      if (cast->y < TO_FIX(-16))
        cast->show = FALSE;
    }
  }
}

static inline void credits_draw_cast(void) {
  for (int i = 0; i < CREDITS_MAX_CAST; ++i) {
    cast_t *cast = &cast_list[i];
    if (cast->show) {
      const int cast_y = TO_INT(cast->y);
      const int cast_x = cast->x - GFX_FONT_WIDTH;
      gfx_draw_texrect(&cast->texrect, GFX_LAYER_FRONT, cast_x - 24, cast_y - 8);
      gfx_draw_string(cast->text, GFX_LAYER_FRONT, cast_x, cast_y);
    }
  }
}

static inline void credits_parse_script(void) {
  const u8 opcode = *csc_state.readptr++;
  const csc_opcode_t *op = &csc_optab[opcode - BASE_OPCODE];
  u16 args[4];
  for (u32 i = 0; i < op->num_args; ++i) {
    // might be misaligned, don't risk it
    args[i] = *csc_state.readptr++;
    args[i] |= (*csc_state.readptr++) << 8;
  }

  switch (opcode) {
    case 0x80: // 'l'
      // label; ideally we'll never come across one
      break;
    case 0x81: // '+'
      csc_state.cast_x = args[0];
      break;
    case 0x82: // '!'
      stage_change_music(args[0]);
      break;
    case 0x83: // '-'
      csc_state.mode = CSC_MODE_WAIT;
      csc_state.wait = args[0];
      break;
    case 0x84: // 'f'
      if (npc_get_flag(args[0]))
        csc_state.readptr = csc_state.data + args[1];
      break;
    case 0x85: // 'j'
      csc_state.readptr = csc_state.data + args[0];
      break;
    case 0x86: // '~'
      org_start_fade();
      break;
    case 0x87: // '/'
      csc_state.mode = CSC_MODE_OFF;
      break;
    case 0x88: // '['
      // start of cast definition; remember the string inside
      ASSERT(*csc_state.readptr++ == 0xFF); // check for string start
      csc_state.laststr = (char *)csc_state.readptr;
      // skip til the end of string
      while (*csc_state.readptr) ++csc_state.readptr;
      // check that the definition is properly terminated
      ASSERT(*++csc_state.readptr == 0x89);
      break;
    case 0x89: // ']'
      // end of cast definition; use the string we remembered to set a cast slot
      ASSERT(csc_state.laststr);
      credits_new_cast(args[0], csc_state.laststr);
      break;
    default:
      printf("unimplemented CSC opcode %02x\n", opcode);
      break;
  }
}

void credits_update(void) {
  switch (csc_state.mode) {
    case CSC_MODE_PARSE:
      credits_parse_script();
      break;
    case CSC_MODE_WAIT:
      if (--csc_state.wait <= 0)
        csc_state.mode = CSC_MODE_PARSE;
      break;
  }

  switch (csc_state.img_act) {
    case IMG_ACT_SLIDE_IN:
      csc_state.img_x += TO_FIX(40);
      if (csc_state.img_x > 0)
        csc_state.img_x = 0;
      break;
    case IMG_ACT_SLIDE_OUT:
      csc_state.img_x -= TO_FIX(40);
      if (csc_state.img_x < TO_FIX(-160))
        csc_state.img_x = TO_FIX(-160);
      break;
    default:
      csc_state.img_x = TO_FIX(-160);
      break;
  }

  if (csc_state.mode != CSC_MODE_OFF)
    credits_update_cast();

  if (input_trig & IN_PAUSE) {
    snd_play_sound(PRIO_HIGH, 18, FALSE);
    credits_stop();
    input_trig = 0;
  }
}

void credits_draw(void) {
  gfx_draw_fillrect(gfx_clear_rgb, GFX_LAYER_FRONT, 0, 0, 160, VID_HEIGHT);
  // draw current illustration
  gfx_draw_texrect(&rc_illust, GFX_LAYER_FRONT, TO_INT(csc_state.img_x), 0);
  // draw cast list
  credits_draw_cast();
}

void credits_show_image(const int id) {
  if (csc_state.img != id) {
    // upload new image
    const gfx_ramsurf_t *s = &cbank->surf[id];
    if (s->mode >= 0 && s->w)
      gfx_upload_image((u8 *)cbank + s->ofs, s->w, s->h, s->mode, SURFACE_ID_CREDITS_IMAGE, FALSE);
    csc_state.img = id;
  }
  // slide in from the left
  csc_state.img_act = IMG_ACT_SLIDE_IN;
}

void credits_hide_image(void) {
  // slide off screen to the left
  csc_state.img_act = IMG_ACT_SLIDE_OUT;
}
