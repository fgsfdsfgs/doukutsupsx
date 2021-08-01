#include <stdio.h>
#include <string.h>

#include "engine/common.h"
#include "engine/memory.h"
#include "engine/graphics.h"
#include "engine/sound.h"
#include "engine/input.h"
#include "engine/filesystem.h"

#include "game/game.h"
#include "game/stage.h"
#include "game/player.h"
#include "game/camera.h"
#include "game/npc.h"
#include "game/tsc.h"

#define OP_STR_START 0xFF
#define MAX_OP_ARGS 6

// might drop the name later, not really required
typedef struct {
  char name[3];
  u8 num_args;
} tsc_opcode_t;

// currently executing text script
tsc_state_t tsc_state;

int tsc_mode;

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
  { "CLR", 0 }, { "FAC", 1 }, { "GIT", 1 }, { "NUM", 0 },
  // player
  { "MM0", 0 }, { "HMC", 0 }, { "SMC", 0 }, { "UNI", 1 },
  { "MYB", 1 }, { "MYD", 1 }, { "MOV", 2 }, { "TRA", 4 },
  // camera
  { "FOB", 1 }, { "FOM", 1 }, { "FON", 2 }, { "FLA", 0 },
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
  { "SVP", 0 }, { "LDP", 0 }, { "SVT", 0 }, { "CRE", 0 },
  { "XX1", 1 }, { "SIL", 1 }, { "CIL", 0 }, { "ESC", 0 },
  { "INI", 0 }, { "PS+", 2 }, { "SLP", 0 }, { "ZAM", 0 },
  { "STC", 0 },
};

#define NUM_OPCODES (sizeof(tsc_optab) / sizeof(*tsc_optab))

void tsc_init(void) {
  memset(text, 0, sizeof(text));

  game_flags &= ~4;

  // preload special scripts

  const char *init_scripts[TSC_MAX_LOADED_SCRIPTS - 1] = {
    "\\MAIN\\ARMSITEM.BSC;1",
    "\\MAIN\\STAGESEL.BSC;1",
    "\\MAIN\\CREDIT.BSC;1",
  };

  for (int i = 0; i < TSC_MAX_LOADED_SCRIPTS - 1; ++i) {
    fs_file_t *f = fs_fopen(init_scripts[i], 0);
    ASSERT(f);
    const u32 size = fs_ftell(f);
    tsc_script_t *data = mem_alloc(size + 1);
    fs_fread_or_die(data, size, 1, f);
    fs_fclose(f);
    scripts[i].script = data;
    scripts[i].size = size;
  }
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
    memset(text[line], 0, sizeof(text[line]));
  }
}

void tsc_set_face(const int face) {
  tsc_state.face = face;
  tsc_state.face_x = TO_FIX(VID_WIDTH / 2 - 156);
  // TODO: face texrect
}

void tsc_set_item(const int item) {
  tsc_state.item = item;
  tsc_state.item_y = VID_HEIGHT - 112;
  // TODO: item texrect
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

static inline bool tsc_parse_string(void) {
  if (*tsc_state.readptr == '\r') {
    // newline
    ++tsc_state.readptr;
    // skip end of string too
    if (!*tsc_state.readptr)
      ++tsc_state.readptr;
    tsc_state.writepos = 0;
    if (tsc_state.flags & 1) {
      ++tsc_state.line;
      tsc_check_newline();
    }
    return FALSE;
  }

  if (tsc_state.flags & 0x10) {
    // SAT/CAT/TUR printing
    char *end = (char *)tsc_state.readptr;
    while (*end && *end != '\r') {
      // skip shift-jis
      if (*end & 0x80) ++end;
      ++end;
    }

    const int len = end - (char *)tsc_state.readptr;
    tsc_state.writepos = len;
  
    const int line = tsc_state.line % TSC_MAX_LINES;
    memcpy(text[line], tsc_state.readptr, len);
    text[line][len] = 0;

    tsc_state.readptr = *end ? end : end + 1;
    if (len >= TSC_LINE_LEN)
      tsc_check_newline();
  } else {
    // normal printing
    char ch[2];

    ch[0] = *tsc_state.readptr++;
    // shift-jis
    if (ch[0] & 0x80)
      ch[1] = *tsc_state.readptr++;
    else
      ch[1] = 0;

    // TODO: print chars

    const int line = tsc_state.line % TSC_MAX_LINES;
    text[line][tsc_state.writepos++] = ch[0];
    if (ch[1]) text[line][tsc_state.writepos++] = ch[1];

    snd_play_sound(CHAN_MISC, 2, SOUND_MODE_PLAY);
    tsc_state.blink = 0;

    if (tsc_state.writepos >= TSC_LINE_LEN) {
      tsc_check_newline();
      tsc_state.writepos = 0;
      ++tsc_state.line;
      tsc_check_newline();
    }

    // skip end of string
    if (!*tsc_state.readptr)
      ++tsc_state.readptr;
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
      return tsc_op_condjmp(0 /*map_active()*/, args[0]);
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
      return tsc_op_condjmp(0 /*get_skipflag()*/, args[1]);
    case 0x0D: // AMJ
      return tsc_op_condjmp(player.arms[args[0]].owned, args[1]);
    /* common opcodes */
    case 0x0E: // FRE
      game_flags |= 0x03;
      return FALSE;
    case 0x0F: // FAI
      tsc_state.mode = TSC_MODE_FADE;
      // hud_start_fade_in(args[0])
      return TRUE;
    case 0x10: // FAO
      tsc_state.mode = TSC_MODE_FADE;
      // hud_start_fade_out(args[0])
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
      player.flags |= PLRCOND_INVISIBLE;
      return FALSE;
    case 0x20: // SMC
      player.flags &= ~PLRCOND_INVISIBLE;
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
      plr_set_pos(args[0], args[1]);
      return FALSE;
    case 0x25: // TRA
      stage_transition(args[0], args[1], args[2], args[3]);
      return FALSE;
    /* camera */
    case 0x26: // FOB
      // cam_target_boss(args[0]);
      return FALSE;
    case 0x27: // FOM
      cam_target_player(args[0]);
      return FALSE;
    case 0x28: // FON
      cam_target_npc(args[0], args[1]);
      return FALSE;
    case 0x29: // FLA
      // SetFlash(0, 0, FLASH_MODE_FLASH);
      return FALSE;
    case 0x2A: // QUA
      // SetQuake(args[0]);
      return FALSE;
    // map
    case 0x3F: // MNA
      printf("map name: %s\n", stage_data->title);
      return FALSE;
    // sound and music
    case 0x47: // SOU
      snd_play_sound(-1, args[0], SOUND_MODE_PLAY);
      return FALSE;
    case 0x4C: // CMU
      printf("change music %02x\n", args[0]);
      return FALSE;
    /* catch-all in case I forgot to implement something */
    default:
      printf("unimplemented opcode %02x\n", opcode);
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
        if (opcode == OP_STR_START)
          stop = tsc_parse_string();
        else
          stop = tsc_exec_opcode(opcode);
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
      // if (hud_fade_active()) break;
      tsc_state.mode = TSC_MODE_PARSE;
      tsc_state.blink = 0;
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
