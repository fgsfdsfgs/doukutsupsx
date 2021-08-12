#include <stdio.h>
#include <string.h>

#include "engine/common.h"
#include "engine/memory.h"
#include "engine/graphics.h"
#include "engine/sound.h"
#include "engine/org.h"
#include "engine/input.h"
#include "engine/filesystem.h"

#include "game/game.h"
#include "game/stage.h"
#include "game/player.h"
#include "game/camera.h"
#include "game/npc.h"
#include "game/hud.h"
#include "game/menu.h"
#include "game/profile.h"
#include "game/tsc.h"

#define FACE_SIZE 48
#define ITEM_SIZE 16

#define OP_STR_START 0xFF
#define MAX_OP_ARGS 6

// might drop the name later, not really required
typedef struct {
  char name[3];
  u8 num_args;
} tsc_opcode_t;

// currently executing text script
tsc_state_t tsc_state;

static char text[4][0x40];

static struct {
  tsc_script_t *script;
  u32 size;
} scripts[TSC_MAX_LOADED_SCRIPTS];

static const tsc_opcode_t tsc_optab[] = {
  // special opcodes
  { "END", 0 },
  // string output opcodes
  { "MSG", 0 }, { "MS2", 0 }, { "MS3", 0 },
  // jumps
  { "EVE", 1 }, { "UNJ", 1 }, { "MPJ", 1 }, { "YNJ", 1 },
  { "NCJ", 2 }, { "ECJ", 2 }, { "FLJ", 2 }, { "ITJ", 2 },
  { "SKJ", 2 }, { "AMJ", 2 },
  // common opcodes
  { "FRE", 0 }, { "FAI", 1 }, { "FAO", 1 }, { "WAI", 1 },
  { "WAS", 0 }, { "KEY", 0 }, { "PRI", 0 }, { "NOD", 0 },
  { "CAT", 0 }, { "SAT", 0 }, { "TUR", 0 }, { "CLO", 0 },
  { "CLR", 0 }, { "FAC", 1 }, { "GIT", 1 }, { "NUM", 1 },
  // player
  { "MM0", 0 }, { "HMC", 0 }, { "SMC", 0 }, { "UNI", 1 },
  { "MYB", 1 }, { "MYD", 1 }, { "MOV", 2 }, { "TRA", 4 },
  // camera
  { "FOB", 2 }, { "FOM", 1 }, { "FON", 2 }, { "FLA", 0 },
  { "QUA", 1 },
  // npc
  { "ANP", 3 }, { "CNP", 3 }, { "INP", 3 }, { "MNP", 4 },
  { "SNP", 4 }, { "DNA", 1 }, { "DNP", 1 }, { "BOA", 1 },
  { "BSL", 1 },
  // item
  { "EQ+", 1 }, { "EQ-", 1 }, { "IT+", 1 }, { "IT-", 1 },
  { "AM+", 2 }, { "AM-", 1 }, { "TAM", 3 }, { "AE+", 0 },
  { "ML+", 1 }, { "LI+", 1 },
  // map
  { "MLP", 0 }, { "MNA", 0 }, { "CMP", 3 }, { "SMP", 2 },
  // flags
  { "FL+", 1 }, { "FL-", 1 }, { "MP+", 1 }, { "SK+", 1 },
  { "SK-", 1 },
  // sound and music
  { "SOU", 1 }, { "SSS", 1 }, { "CSS", 0 }, { "SPS", 0 },
  { "CPS", 0 }, { "CMU", 1 }, { "FMU", 0 }, { "RMU", 0, },
  // other
  { "SVP", 0 }, { "LDP", 0 }, { "STC", 0 }, { "CRE", 0 },
  { "XX1", 1 }, { "SIL", 1 }, { "CIL", 0 }, { "ESC", 0 },
  { "INI", 0 }, { "PS+", 2 }, { "SLP", 0 }, { "ZAM", 0 },
};

#define NUM_OPCODES (sizeof(tsc_optab) / sizeof(*tsc_optab))

static gfx_texrect_t rc_face;
static gfx_texrect_t rc_item;

// the textbox texture sheet is aligned to page X, so we can draw them normally
static gfx_texrect_t rc_textbox[3] = {
  {{ 0, 0,  244, 8  }},
  {{ 0, 8,  244, 16 }},
  {{ 0, 16, 244, 24 }},
};
static gfx_texrect_t rc_itembox[5] = {
  {{ 0,   0,  72,  16 }},
  {{ 0,   8,  72,  24 }},
  {{ 240, 0,  244, 8  }},
  {{ 240, 8,  244, 16 }},
  {{ 240, 16, 244, 24 }},
};
static gfx_texrect_t rc_yesno = {{ 152, 48, 244, 80 }};
static gfx_texrect_t rc_yesno_cur = {{ 112, 88, 128, 104 }};

void tsc_init(void) {
  tsc_reset();

  // preload special scripts

  const char *init_scripts[TSC_MAX_LOADED_SCRIPTS - 1] = {
    "\\MAIN\\ARMSITEM.BSC;1",
    "\\MAIN\\STAGESEL.BSC;1",
    "\\MAIN\\CREDIT.BSC;1",
  };

  for (int i = 0; i < TSC_MAX_LOADED_SCRIPTS - 1; ++i) {
    fs_file_t *f = fs_fopen(init_scripts[i], 0);
    ASSERT(f);
    const u32 size = fs_fsize(f);
    tsc_script_t *data = mem_alloc(size + 1);
    fs_fread_or_die(data, size, 1, f);
    fs_fclose(f);
    scripts[i].script = data;
    scripts[i].size = size;
  }

  for (int i = 0; i < 3; ++i)
    gfx_set_texrect(&rc_textbox[i], SURFACE_ID_TEXT_BOX);
  for (int i = 0; i < 5; ++i)
    gfx_set_texrect(&rc_itembox[i], SURFACE_ID_TEXT_BOX);
  gfx_set_texrect(&rc_yesno, SURFACE_ID_TEXT_BOX);
  gfx_set_texrect(&rc_yesno_cur, SURFACE_ID_TEXT_BOX);
}

void tsc_reset(void) {
  memset(text, 0, sizeof(text));
  memset(&tsc_state, 0, sizeof(tsc_state));
  game_flags &= ~4;
}

void tsc_set_stage_script(tsc_script_t *data, const u32 size) {
  scripts[TSC_SCRIPT_STAGE].script = data;
  scripts[TSC_SCRIPT_STAGE].size = size;
}

void tsc_switch_script(const int idx) {
  tsc_state.script = scripts[idx].script;
  tsc_state.size = scripts[idx].size;
}

static inline u8 *tsc_find_event(const int num) {
  for (u32 i = 0; i < tsc_state.script->num_ev; ++i) {
    if (tsc_state.script->ev_map[i].id == num)
      return (u8 *)tsc_state.script + tsc_state.script->ev_map[i].ofs;
  }
  return NULL;
}

void tsc_start_event(const int num) {
  // reset state
  tsc_state.mode = TSC_MODE_PARSE;
  tsc_state.flags = 0;
  tsc_state.item = 0;
  tsc_state.face = 0;
  tsc_state.wait = 4;
  tsc_state.blink = 0;
  tsc_state.text_x = TEXT_LEFT;
  tsc_state.text_y = TEXT_TOP;
  tsc_clear_text();

  game_flags |= 5;
  player.shock = 0;

  tsc_state.readptr = tsc_find_event(num);
  if (!tsc_state.readptr)
    panic("started unknown event #%04d\n", num);
}

void tsc_jump_event(const int num) {
  tsc_state.mode = TSC_MODE_PARSE;
  tsc_state.wait = 4;
  tsc_state.blink = 0;
  tsc_clear_text();

  game_flags |= 4;

  tsc_state.readptr = tsc_find_event(num);
  if (!tsc_state.readptr)
    panic("jumped to unknown event #%04d", num);
}

void tsc_stop_event(void) {
  tsc_state.mode = TSC_MODE_OFF;
  tsc_state.flags = 0;
  game_flags &= ~4;
  game_flags |= 3;
}

void tsc_clear_text(void) {
  tsc_state.line = 0;
  tsc_state.writepos = 0;
  for (int i = 0; i < 4; ++i)
    tsc_state.line_y[i] = i * 16;
  memset(text, 0, sizeof(text));
}

void tsc_check_newline(void) {
  const int line = tsc_state.line % TSC_MAX_LINES;
  if (tsc_state.line_y[line] == 48) {
    tsc_state.mode = TSC_MODE_NEWLINE;
    game_flags |= 4;
    text[line][0] = 0;
  }
}

void tsc_set_face(const int face) {
  tsc_state.face = face;
  tsc_state.face_x = TO_FIX(VID_WIDTH / 2 - 156);
  rc_face.r.left = FACE_SIZE * (face % 6);
  rc_face.r.top = FACE_SIZE * (face / 6);
  rc_face.r.right = rc_face.r.left + FACE_SIZE;
  rc_face.r.bottom = rc_face.r.top + FACE_SIZE;
  gfx_set_texrect(&rc_face, SURFACE_ID_FACE);
}

void tsc_set_item(const int item) {
  tsc_state.item = item;
  tsc_state.item_y = VID_HEIGHT - 112;
  rc_item.r.left = ITEM_SIZE * (item % 16);
  rc_item.r.top = ITEM_SIZE * (item / 16);
  rc_item.r.right = rc_item.r.left + ITEM_SIZE;
  rc_item.r.bottom = rc_item.r.top + ITEM_SIZE;
  gfx_set_texrect(&rc_item, SURFACE_ID_ITEM_IMAGE);
}

void tsc_print_number(const int id) {
  const int x = tsc_state.num[id % TSC_MAX_NUMBERS];

  const int line = tsc_state.line % TSC_MAX_LINES;
  char *buf = &text[line][tsc_state.writepos];
  const int len = snprintf(buf, TSC_LINE_LEN - tsc_state.writepos + 1, "%d", x);

  snd_play_sound(CHAN_MISC, 2, SOUND_MODE_PLAY);
  tsc_state.blink = 0;

  tsc_state.writepos += len;
  if (tsc_state.writepos >= TSC_LINE_LEN) {
    tsc_state.writepos = 0;
    ++tsc_state.line;
    tsc_check_newline();
  }
}

static inline bool tsc_parse_string(const char in_ch) {
  if (in_ch == '\r') {
    // newline
    tsc_state.writepos = 0;
    if (tsc_state.flags & 1) {
      ++tsc_state.line;
      tsc_check_newline();
    }
    return FALSE;
  }

  if (tsc_state.flags & 0x10) {
    // SAT/CAT/TUR printing
    char *start = (char *)tsc_state.readptr - 1;
    char *end = start;
    while (*end && *end != '\r') {
      // skip shift-jis
      if (*end & 0x80) ++end;
      ++end;
    }

    const int len = end - start;
    tsc_state.writepos = len;
  
    const int line = tsc_state.line % TSC_MAX_LINES;
    memcpy(text[line], start, len);
    text[line][len] = 0;

    tsc_state.readptr = end;
    if (len >= TSC_LINE_LEN)
      tsc_check_newline();
  } else {
    // normal printing
    char ch[3];
    ch[2] = 0;

    ch[0] = in_ch;
    // shift-jis
    if (ch[0] & 0x80)
      ch[1] = *tsc_state.readptr++;
    else
      ch[1] = 0;

    const int line = tsc_state.line % TSC_MAX_LINES;
    text[line][tsc_state.writepos++] = ch[0];
    if (ch[1]) text[line][tsc_state.writepos++] = ch[1];
    text[line][tsc_state.writepos] = 0; // terminate for now

    snd_play_sound(CHAN_MISC, 2, SOUND_MODE_PLAY);
    tsc_state.blink = 0;

    if (tsc_state.writepos >= TSC_LINE_LEN) {
      tsc_check_newline();
      tsc_state.writepos = 0;
      ++tsc_state.line;
      tsc_check_newline();
    }
  }

  return TRUE;
}

static inline bool tsc_op_condjmp(const bool cond, const u16 tgt) {
  if (cond) tsc_jump_event(tgt);
  return FALSE;
}

static inline bool tsc_exec_opcode(const u8 opcode) {
  if (opcode > NUM_OPCODES)
    panic("unknown TSC opcode %02x", opcode);

  npc_t *npc;
  u16 args[MAX_OP_ARGS];
  const tsc_opcode_t *op = &tsc_optab[opcode];
  for (u32 i = 0; i < op->num_args; ++i) {
    // might be misaligned, don't risk it
    args[i] = *tsc_state.readptr++;
    args[i] |= (*tsc_state.readptr++) << 8;
  }

  // pulling this apart into functions would waste even more space, so fuck it
  switch (opcode) {
    /* special opcodes */
    case 0x00: // END
      tsc_state.mode = TSC_MODE_OFF;
      tsc_state.face = 0;
      player.cond &= ~PLRCOND_UNKNOWN01;
      game_flags |= 3;
      return TRUE;
    /* string output opcodes */
    case 0x01: // MSG
      tsc_clear_text();
      tsc_state.flags |= 0x03;
      tsc_state.flags &= ~0x30;
      if (tsc_state.flags & 0x40)
        tsc_state.flags |= 0x10;
      return TRUE;
    case 0x02: // MS2
      tsc_clear_text();
      tsc_state.flags &= ~0x12;
      tsc_state.flags |= 0x21;
      if (tsc_state.flags & 0x40)
        tsc_state.flags |= 0x10;
      tsc_state.face = 0;
      return TRUE;
    case 0x03: // MS3
      tsc_clear_text();
      tsc_state.flags &= ~0x10;
      tsc_state.flags |= 0x23;
      if (tsc_state.flags & 0x40)
        tsc_state.flags |= 0x10;
      return TRUE;
    /* jump opcodes */
    case 0x04: // EVE
      tsc_jump_event(args[0]);
      return FALSE;
    case 0x05: // UNJ
      return tsc_op_condjmp((player.unit == args[0]), args[1]);
    case 0x06: // MPJ
      return tsc_op_condjmp(game_get_mapflag(stage_data->id), args[0]);
    case 0x07: // YNJ
      tsc_state.next_event = args[0];
      tsc_state.mode = TSC_MODE_YESNO;
      tsc_state.wait = 0;
      tsc_state.yesno = 0;
      snd_play_sound(-1, 5, SOUND_MODE_PLAY);
      return TRUE;
    case 0x08: // NCJ
      return tsc_op_condjmp((npc_find_by_class(args[0]) != NULL), args[1]);
    case 0x09: // ECJ
      return tsc_op_condjmp((npc_find_by_event_num(args[0]) != NULL), args[1]);
    case 0x0A: // FLJ
      return tsc_op_condjmp(npc_get_flag(args[0]), args[1]);
    case 0x0B: // ITJ
      return tsc_op_condjmp(player.items[args[0]], args[1]);
    case 0x0C: // SKJ
      return tsc_op_condjmp(game_get_skipflag(args[0]), args[1]);
    case 0x0D: // AMJ
      return tsc_op_condjmp(player.arms[args[0]].owned, args[1]);
    /* common opcodes */
    case 0x0E: // FRE
      game_flags |= 0x03;
      return FALSE;
    case 0x0F: // FAI
      tsc_state.mode = TSC_MODE_FADE;
      cam_start_fade_in(args[0]);
      return TRUE;
    case 0x10: // FAO
      tsc_state.mode = TSC_MODE_FADE;
      cam_start_fade_out(args[0]);
      return TRUE;
    case 0x11: // WAI
      tsc_state.mode = TSC_MODE_WAIT;
      tsc_state.wait_next = args[0];
      return TRUE;
    case 0x12: // WAS
      tsc_state.mode = TSC_MODE_WAIT_PLAYER;
      return TRUE;
    case 0x13: // KEY
      game_flags &= ~GFLAG_INPUT_ENABLED;
      game_flags |= 1;
      player.up = FALSE;
      player.shock = 0;
      return FALSE;
    case 0x14: // PRI
      game_flags &= ~3;
      player.shock = 0;
      return FALSE;
    case 0x15: // NOD
      tsc_state.mode = TSC_MODE_NOD;
      return TRUE;
    case 0x16: // CAT
    case 0x17: // SAT
      tsc_state.flags |= 0x40;
      return FALSE;
    case 0x18: // TUR
      tsc_state.flags |= 0x10;
      return FALSE;
    case 0x19: // CLO
      tsc_state.flags &= ~0x33;
      return FALSE;
    case 0x1A: // CLR
      tsc_clear_text();
      return FALSE;
    case 0x1B: // FAC
      if (tsc_state.face != args[0])
        tsc_set_face(args[0]);
      return FALSE;
    case 0x1C: // GIT
      if (tsc_state.item != args[0])
        tsc_set_item(args[0]);
      return FALSE;
    case 0x1D: // NUM
      tsc_print_number(args[0]);
      return FALSE;
    /* player */
    case 0x1E: // MM0
      player.xvel = 0;
      return FALSE;
    case 0x1F: // HMC
      player.cond |= PLRCOND_INVISIBLE;
      return FALSE;
    case 0x20: // SMC
      player.cond &= ~PLRCOND_INVISIBLE;
      return FALSE;
    case 0x21: // UNI
      player.unit = args[0];
      return FALSE;
    case 0x22: // MYB
      plr_jump_back(args[0]);
      return FALSE;
    case 0x23: // MYD
      plr_face_towards(args[0]);
      return FALSE;
    case 0x24: // MOV
      plr_set_pos(TO_FIX(args[0]) * TILE_SIZE, TO_FIX(args[1]) * TILE_SIZE);
      return FALSE;
    case 0x25: // TRA
      stage_transition(args[0], args[1], args[2], args[3]);
      return FALSE;
    /* camera */
    case 0x26: // FOB
      cam_target_boss(args[0], args[1]);
      return FALSE;
    case 0x27: // FOM
      cam_target_player(args[0]);
      return FALSE;
    case 0x28: // FON
      cam_target_npc(args[0], args[1]);
      return FALSE;
    case 0x29: // FLA
      cam_start_flash(0, 0, FLASH_MODE_FLASH);
      return FALSE;
    case 0x2A: // QUA
      cam_start_quake_small(args[0]);
      return FALSE;
    // npcs
    case 0x2B: // ANP
      npc = npc_find_by_event_num(args[0]);
      if (npc)
        npc_change_action(npc, args[1], args[2]);
      return FALSE;
    case 0x2C: // CNP
      npc_change_class_by_event_num(args[0], args[1], args[2], 0);
      return FALSE;
    case 0x2D: // INP
      npc_change_class_by_event_num(args[0], args[1], args[2], NPC_INTERACTABLE);
      return FALSE;
    case 0x2E: // MNP
      npc = npc_find_by_event_num(args[0]);
      if (npc)
        npc_set_pos(npc, TO_FIX(args[1]) * TILE_SIZE, TO_FIX(args[2]) * TILE_SIZE, args[3]);
      return FALSE;
    case 0x2F: // SNP
      npc_spawn(args[0], TO_FIX(args[1]) * TILE_SIZE, TO_FIX(args[2]) * TILE_SIZE, 0, 0, args[3], NULL, NPC_STARTIDX_DYNAMIC);
      return FALSE;
    case 0x30: // DNA
      npc_delete_by_class(args[0], TRUE);
      return FALSE;
    case 0x31: // DNP
      npc_delete_by_event_num(args[0]);
      return FALSE;
    case 0x32: // BOA
      npc_set_boss_act(args[0]);
      return FALSE;
    case 0x33: // BSL
      npc = args[0] ? npc_find_by_event_num(args[0]) : &npc_boss[0];
      if (npc) hud_init_boss_life(npc);
      return FALSE;
    // item
    case 0x34: // EQ+
    case 0x35: // EQ-
      plr_item_equip(args[0], (opcode == 0x34));
      return FALSE;
    case 0x36: // IT+
      snd_play_sound(CHAN_ITEM, 38, SOUND_MODE_PLAY);
      plr_item_give(args[0]);
      return FALSE;
    case 0x37: // IT-
      plr_item_take(args[0]);
      return FALSE;
    case 0x38: // AM+
      tsc_state.num[0] = args[1];
      snd_play_sound(CHAN_ITEM, 38, SOUND_MODE_PLAY);
      plr_arm_give(args[0], args[1]);
      return FALSE;
    case 0x39: // AM-
      plr_arm_take(args[0]);
      return FALSE;
    case 0x3A: // TAM
      plr_arm_trade(args[0], args[1], args[2]);
      return FALSE;
    case 0x3B: // AE+
      plr_arms_refill_all();
      return FALSE;
    case 0x3C: // ML+
      plr_add_max_life(args[0]);
      return FALSE;
    case 0x3D: // LI+
      plr_add_life(args[0]);
      return FALSE;
    // map
    case 0x3E: // MLP
      menu_open(MENU_MAP);
      return TRUE;
    case 0x3F: // MNA
      hud_show_map_name();
      return FALSE;
    case 0x40: // CMP
      stage_set_tile(args[0], args[1], args[2]);
      return FALSE;
    case 0x41: // SMP
      stage_shift_tile(args[0], args[1]);
      return FALSE;
    // flags
    case 0x42: // FL+
      npc_set_flag(args[0]);
      return FALSE;
    case 0x43: // FL-
      npc_clear_flag(args[0]);
      return FALSE;
    case 0x44: // MP+
      game_set_mapflag(args[0]);
      return FALSE;
    case 0x45: // SK+
      game_set_skipflag(args[0]);
      return FALSE;
    case 0x46: // SK-
      game_clear_skipflag(args[0]);
      return FALSE;
    // sound and music
    case 0x47: // SOU
      snd_play_sound(-1, args[0], SOUND_MODE_PLAY);
      return FALSE;
    case 0x48: // SSS
      snd_play_sound_freq(CHAN_LOOP1, 40, args[0], SOUND_MODE_PLAY_LOOP);
      snd_play_sound_freq(CHAN_LOOP2, 41, args[0] + 100, SOUND_MODE_PLAY_LOOP);
      return FALSE;
    case 0x49: // CSS
      snd_play_sound(CHAN_LOOP1, 40, SOUND_MODE_STOP);
      snd_play_sound(CHAN_LOOP2, 41, SOUND_MODE_STOP);
      return FALSE;
    case 0x4A: // SPS
      snd_play_sound(CHAN_LOOP2, 58, SOUND_MODE_PLAY_LOOP);
      return FALSE;
    case 0x4B: // CPS
      snd_play_sound(CHAN_LOOP2, 58, SOUND_MODE_STOP);
      return FALSE;
    case 0x4C: // CMU
      stage_change_music(args[0]);
      return FALSE;
    case 0x4D: // FMU
      org_start_fade();
      return FALSE;
    case 0x4E: // RMU
      stage_resume_music();
      return FALSE;
    // other
    case 0x4F: // SVP
      profile_write();
      return FALSE;
    case 0x50: // LDP
      if (!profile_read()) {
        game_reset();
        game_start();
      }
      return TRUE;
    case 0x51: // STC
      printf("TODO: save time\n");
      return FALSE;
    case 0x52: // CRE
      printf("TODO: credits\n");
      game_flags |= 8;
      return FALSE;
    case 0x53: // XX1
      printf("TODO: falling island\n");
      return TRUE;
    case 0x54: // SIL
      printf("TODO: credits show illust\n");
      return FALSE;
    case 0x55: // CIL
      printf("TODO: credits hide illust\n");
      return FALSE;
    case 0x56: // ESC
    case 0x57: // INI
      // can't exit, so we restart instead in both cases
      game_reset();
      game_start();
      return TRUE;
    case 0x58: // PS+
      game_add_tele_dest(args[0], args[1]);
      return FALSE;
    case 0x59: // SLP
      menu_open(MENU_STAGESELECT);
      return TRUE;
    case 0x5A: // ZAM
      plr_arms_empty_all();
      return FALSE;
    /* catch-all in case I forgot to implement something */
    default:
      printf("unimplemented TSC opcode %02x\n", opcode);
      return TRUE;
  }
}

static inline void tsc_yesno_prompt(void) {
  if (tsc_state.wait < 16) {
    ++tsc_state.wait;
    return;
  }

  if (input_trig & IN_OK) {
    // confirm
    snd_play_sound(-1, 18, SOUND_MODE_PLAY);
    if (tsc_state.yesno == 1) {
      tsc_jump_event(tsc_state.next_event);
    } else {
      tsc_state.mode = TSC_MODE_PARSE;
      tsc_state.blink = 0;
    }
  } else if (input_trig & IN_LEFT) {
    // yes
    tsc_state.yesno = 0;
    snd_play_sound(-1, 1, SOUND_MODE_PLAY);
  } else if (input_trig & IN_RIGHT) {
    // no
    tsc_state.yesno = 1;
    snd_play_sound(-1, 1, SOUND_MODE_PLAY);
  }
}

bool tsc_update(void) {
  bool stop = FALSE;

  switch (tsc_state.mode) {
    case TSC_MODE_PARSE:
      ++tsc_state.wait;

      if (!(game_flags & GFLAG_INPUT_ENABLED) && input_held & (IN_OK | IN_CANCEL))
        tsc_state.wait += 4;

      if (tsc_state.wait < 4)
        break;

      tsc_state.wait = 0;

      while (!stop) {
        const u8 opcode = *tsc_state.readptr++;
        // check if this is a string start or a real opcode
        if (opcode == OP_STR_START) {
          // start parsing the string
          stop = FALSE;
          tsc_state.flags |= TSCFLAG_IN_STRING;
        } else if (tsc_state.flags & TSCFLAG_IN_STRING) {
          // if we're in a string and next char is the end of it, stop parsing string
          if (opcode == 0x00)
            tsc_state.flags &= ~TSCFLAG_IN_STRING;
          else
            stop = tsc_parse_string((char)opcode);
        } else {
          // regular opcode
          stop = tsc_exec_opcode(opcode);
        }
      }
      break;

    case TSC_MODE_NOD:
      if (input_trig & (IN_OK | IN_CANCEL))
        tsc_state.mode = TSC_MODE_PARSE;
      break;

    case TSC_MODE_NEWLINE:
      for (int i = 0; i < TSC_MAX_LINES; ++i) {
        tsc_state.line_y[i] -= 4;
        if (tsc_state.line_y[i] == 0)
          tsc_state.mode = TSC_MODE_PARSE;
        else if (tsc_state.line_y[i] == -16)
          tsc_state.line_y[i] = 48;
      }
      break;

    case TSC_MODE_WAIT:
      if (tsc_state.wait_next == 9999)
        break;
      if (tsc_state.wait != 9999)
        ++tsc_state.wait;
      if (tsc_state.wait >= tsc_state.wait_next) {
        tsc_state.mode = TSC_MODE_PARSE;
        tsc_state.blink = 0;
      }
      break;

    case TSC_MODE_FADE:
      if (!cam_is_fading()) {
        tsc_state.mode = TSC_MODE_PARSE;
        tsc_state.blink = 0;
      }
      break;

    case TSC_MODE_WAIT_PLAYER:
      if (player.flags & 8) {
        tsc_state.mode = TSC_MODE_PARSE;
        tsc_state.blink = 0;
      }
      break;

    case TSC_MODE_YESNO:
      tsc_yesno_prompt();
      break;
  }

  if (tsc_state.mode == 0)
    game_flags &= ~4;
  else
    game_flags |= 4;

  return FALSE; // don't quit yet
}

void tsc_draw(void) {
  if (tsc_state.mode == TSC_MODE_OFF || (tsc_state.flags & 1) == 0)
    return;

  const int text_y = ((tsc_state.flags & 0x20) ? 32 : TEXT_TOP) - 16;

  // draw textbox
  if (tsc_state.flags & 2) {
    gfx_draw_texrect(&rc_textbox[0], GFX_LAYER_FRONT, TEXT_BOX_LEFT, text_y - 10);
    for (int i = 1; i < 7; ++i)
      gfx_draw_texrect(&rc_textbox[1], GFX_LAYER_FRONT, TEXT_BOX_LEFT, text_y - 10 + i * 8);
    gfx_draw_texrect(&rc_textbox[2], GFX_LAYER_FRONT, TEXT_BOX_LEFT, text_y - 10 + 7 * 8);
  }

  gfx_push_cliprect(GFX_LAYER_FRONT, TEXT_BOX_LEFT + 16, text_y + 6, 244, 48);

  // draw face
  int text_ofs_x = 0;
  if (tsc_state.face) {
    text_ofs_x = 56;
    if (tsc_state.face_x < TO_FIX(TEXT_LEFT))
      tsc_state.face_x += 0x1000;
    gfx_draw_texrect(&rc_face, GFX_LAYER_FRONT, TO_INT(tsc_state.face_x) - 4, text_y - 1);
  }

  // append NOD cursor
  int p = tsc_state.writepos;
  const int line = tsc_state.line % TSC_MAX_LINES;
  if (tsc_state.mode == TSC_MODE_NOD && (tsc_state.blink++ % 20 > 12)) {
    text[line][p++] = '|';
    text[line][p++] = 0;
  } else {
    text[line][p] = 0;
  }

  // draw text
  for (int i = 0; i < TSC_MAX_LINES; ++i) {
    if (text[i][0])
      gfx_draw_string(text[i], GFX_LAYER_FRONT, TEXT_LEFT + 8 + text_ofs_x, text_y + tsc_state.line_y[i] + 8);
  }

  gfx_pop_cliprect(GFX_LAYER_FRONT);

  // draw item
  if (tsc_state.item) {
    gfx_draw_texrect(&rc_itembox[0], GFX_LAYER_FRONT, TEXT_BOX_LEFT + 82, VID_HEIGHT - 112 - 16);
    gfx_draw_texrect(&rc_itembox[1], GFX_LAYER_FRONT, TEXT_BOX_LEFT + 82, VID_HEIGHT - 96 - 16);
    gfx_draw_texrect(&rc_itembox[2], GFX_LAYER_FRONT, TEXT_BOX_LEFT + 154, VID_HEIGHT - 112 - 16);
    gfx_draw_texrect(&rc_itembox[3], GFX_LAYER_FRONT, TEXT_BOX_LEFT + 154, VID_HEIGHT - 104 - 16);
    gfx_draw_texrect(&rc_itembox[3], GFX_LAYER_FRONT, TEXT_BOX_LEFT + 154, VID_HEIGHT - 96 - 16);
    gfx_draw_texrect(&rc_itembox[4], GFX_LAYER_FRONT, TEXT_BOX_LEFT + 154, VID_HEIGHT - 88 - 16);

    if (tsc_state.item_y < VID_HEIGHT - 104)
      ++tsc_state.item_y;

    if (tsc_state.item < 1000)
      gfx_draw_texrect_16x16(&hud_rc_arms[tsc_state.item], GFX_LAYER_FRONT, TEXT_BOX_LEFT + 110 + 8, tsc_state.item_y + 8 - 16);
    else
      gfx_draw_texrect(&hud_rc_item[tsc_state.item - 1000], GFX_LAYER_FRONT, TEXT_BOX_LEFT + 102, tsc_state.item_y - 16);
  }

  // draw yes/no
  if (tsc_state.mode == TSC_MODE_YESNO) {
    const int ty = (tsc_state.wait < 2) ?
      (VID_HEIGHT - 96 - 16) + (2 - tsc_state.wait) * 4 :
      (VID_HEIGHT - 96 - 16);
    gfx_draw_texrect(&rc_yesno, GFX_LAYER_FRONT, TEXT_BOX_LEFT + 178, ty);
    if (tsc_state.wait == 16)
      gfx_draw_texrect(&rc_yesno_cur, GFX_LAYER_FRONT, tsc_state.yesno * 41 + TEXT_BOX_LEFT + 174, ty + 8);
  }
}
