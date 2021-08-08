#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>

#include "common.h"
#include "musiclist.h"
#include "tsc.h"

#define MAX_OP_ARGS 8

#define OP_STR_START 0xFF

typedef struct {
  union {
    char name[4];
    uint32_t name_cmp;
  };
  uint8_t num_args;
  uint8_t id; // for parse_opcode
} opcode_t;

static opcode_t op_table[] = {
  // special opcodes
  { "<END", 0  },
  // string output opcodes
  { "<MSG", 0  }, { "<MS2", 0  }, { "<MS3", 0  },
  // jumps
  { "<EVE", 1  }, { "<UNJ", 1  }, { "<MPJ", 1  }, { "<YNJ", 1  },
  { "<NCJ", 2  }, { "<ECJ", 2  }, { "<FLJ", 2  }, { "<ITJ", 2  },
  { "<SKJ", 2  }, { "<AMJ", 2  },
  // common opcodes
  { "<FRE", 0  }, { "<FAI", 1  }, { "<FAO", 1  }, { "<WAI", 1  },
  { "<WAS", 0  }, { "<KEY", 0  }, { "<PRI", 0  }, { "<NOD", 0  },
  { "<CAT", 0  }, { "<SAT", 0  }, { "<TUR", 0  }, { "<CLO", 0  },
  { "<CLR", 0  }, { "<FAC", 1  }, { "<GIT", 1  }, { "<NUM", 0  },
  // player
  { "<MM0", 0  }, { "<HMC", 0  }, { "<SMC", 0  }, { "<UNI", 1  },
  { "<MYB", 1  }, { "<MYD", 1  }, { "<MOV", 2  }, { "<TRA", 4  },
  // camera
  { "<FOB", 2  }, { "<FOM", 1  }, { "<FON", 2  }, { "<FLA", 0  },
  { "<QUA", 1  },
  // npc
  { "<ANP", 3  }, { "<CNP", 3  }, { "<INP", 3  }, { "<MNP", 4  },
  { "<SNP", 4  }, { "<DNA", 1  }, { "<DNP", 1  }, { "<BOA", 1  },
  { "<BSL", 1  },
  // item
  { "<EQ+", 1  }, { "<EQ-", 1  }, { "<IT+", 1  }, { "<IT-", 1  },
  { "<AM+", 2  }, { "<AM-", 1  }, { "<TAM", 3  }, { "<AE+", 0  },
  { "<ML+", 1  }, { "<LI+", 1  },
  // map
  { "<MLP", 0  }, { "<MNA", 0  }, { "<CMP", 3  }, { "<SMP", 2  },
  // flags
  { "<FL+", 1  }, { "<FL-", 1  }, { "<MP+", 1  }, { "<SK+", 1  },
  { "<SK-", 1  },
  // sound and music
  { "<SOU", 1  }, { "<SSS", 1  }, { "<CSS", 0  }, { "<SPS", 0  },
  { "<CPS", 0  }, { "<CMU", 1  }, { "<FMU", 0  }, { "<RMU", 0, },
  // other
  { "<SVP", 0  }, { "<LDP", 0  }, { "<STC", 0  }, { "<CRE", 0  },
  { "<XX1", 1  }, { "<SIL", 1  }, { "<CIL", 0  }, { "<ESC", 0  },
  { "<INI", 0  }, { "<PS+", 2  }, { "<SLP", 0  }, { "<ZAM", 0  },
};

static const int num_opcodes = sizeof(op_table) / sizeof(*op_table);

static const opcode_t op_credits_table[] = {
  // credits pseudo-opcodes
  { "[",    0, 0x80  }, { "+",    1, 0x81  }, { "!",    1, 0x82  }, { "-",    1, 0x83  },
  { "f",    2, 0x84  }, { "j",    1, 0x85  }, { "l",    1, 0x86  }, { "~",    0, 0x87  },
  { "/",    0, 0x88  },
};

static const int num_credits_opcodes = sizeof(op_credits_table) / sizeof(*op_credits_table);

static inline const opcode_t *parse_opcode(const char *p, opcode_t *optab, const int opnum) {
  union {
    char name[4];
    uint32_t name_cmp;
  } pname;

  if (p[0] == '<') {
    pname.name_cmp = *(const uint32_t *)p;
  } else {
    // might be a special credits pseudo op
    pname.name[0] = *p;
    pname.name[1] = pname.name[2] = pname.name[3] = 0;
  }

  for (uint32_t i = 0; i < opnum; ++i) {
    if (pname.name_cmp == optab[i].name_cmp) {
      if (!optab[i].id)
        optab[i].id = i;
      return &optab[i];
    }
  }

  return NULL;
}

static inline uint8_t *tsc_compile_event(const int ev, char *src, uint8_t *code) {
  while (*src && *src != '#') {
    if (*src == '<') {
      const opcode_t *op = parse_opcode(src, op_table, num_opcodes);
      if (!op) {
        fprintf(stderr, "error: tsc_compile: #%04d: unknown opcode '%.4s'\n", ev, src);
        return NULL;
      }

      *code++ = op->id;

      // normal opcode
      if (op->id == 0)
        return code; // <END reached

      src += 4; // <XXX

      if (op->num_args > 0) {
        // numeric args
        uint32_t i, n;
        char buf[5] = { 0 };

        for (i = 0; i < op->num_args; ++i) {
          for (n = 0; n < 4 && isdigit(*src); ++n, ++src)
            buf[n] = *src;
          *(uint16_t *)code = (uint16_t)atoi(buf);
          code += 2;
          if (*src == ':') ++src; // skip ':'
        }

        if (i != op->num_args) {
          fprintf(stderr, "error: tsc_compile: #%04d: '%.4s' expects %u args, got %u\n", ev, op->name, op->num_args, i);
          return NULL;
        }
      }
    } else if (*src != '<') {
      // text string
      *code++ = OP_STR_START; // mark string start
      while (*src && *src != '<' && *src != '#') {
        // leave '\r' but not '\n', since '\r' is what the game checks for
        if (*src != '\n')
          *code++ = *src;
        ++src;
      }
      *code++ = 0; // mark string end
    }
  }

  // unterminated event; just insert terminator
  *code++ = 0x00;

  return code;
}

static inline uint8_t *tsc_compile_credits(char *src, uint8_t *code) {
  // TODO
  *code++ = 0x00;
  return code;
}

tsc_script_t *tsc_compile(char *src, const int in_size, int *out_size) {
  // this is much more than enough space, but whatever
  tsc_script_t *tsc = calloc(1, sizeof(*tsc) + TSC_MAX_EVENTS * sizeof(tsc_event_t) + in_size + 4);
  assert(tsc);

  uint8_t *codeptr;

  // count all the events
  char *p = strchr(src, '#');
  while (p) {
    if (isdigit(p[1])) {
      const int id = atoi(p + 1);
      tsc->ev_map[tsc->num_ev].id = id;
      p += 5; // #XXXX
      while (*p == '\r' || *p == '\n') ++p;
      // save the char position for now
      tsc->ev_map[tsc->num_ev++].ofs = p - src;
    }
    p = strchr(p, '#');
  }

  codeptr = (uint8_t *)tsc + sizeof(*tsc) + tsc->num_ev * sizeof(tsc_event_t);

  // might be credits tsc; compile the only "event", which is credits
  if (tsc->num_ev == 0) {
    fprintf(stderr, "warning: tsc_compile: no events in script!\n");
    fprintf(stderr, "this is normal when compiling credits.tsc\n");

    tsc->num_ev = 1;
    tsc->ev_map[0].ofs = codeptr - (uint8_t *)tsc;
    codeptr = tsc_compile_credits(src, codeptr);
    if (!codeptr) {
      fprintf(stderr, "error: tsc_compile: could not compile credits script\n");
      goto _error;
    }

    *out_size = ALIGN(codeptr - (uint8_t *)tsc, 4);

    return tsc;
  }

  // normal tsc; compile every event
  for (uint32_t i = 0; i < tsc->num_ev; ++i) {
    const uint16_t new_ofs = codeptr - (uint8_t *)tsc;
    codeptr = tsc_compile_event(tsc->ev_map[i].id, src + tsc->ev_map[i].ofs, codeptr);
    if (!codeptr) {
      fprintf(stderr, "error: tsc_compile: could not compile event #%04d\n", tsc->ev_map[i].id);
      goto _error;
    }
    tsc->ev_map[i].ofs = new_ofs;
  }

  *out_size = ALIGN(codeptr - (uint8_t *)tsc, 4);

  return tsc;

_error:
  free(tsc);
  return NULL;
}

void tsc_decode(uint8_t *data, const int size) {
  // replicating arithmetics without change in case of compatibility problems
  const int half = size / 2;
  const int cdec = (data[half] == 0) ? -7 : (data[half] % 0x100) * -1;
  for (int i = 0; i < size; ++i) {
    int tmp = data[i];
    tmp += cdec;
    if (i != half)
      data[i] = tmp % 0x100;
  }
}

int tsc_scan_music(const char *src, uint32_t *songlist, const int limit) {
  if (!src || !src[0])
    return 0;

  int count = 0;

  // very shitty and slow but who cares
  const char *tsc = src;
  const char *cmd = strstr(tsc, "<CMU");
  while (cmd) {
    const char *num = cmd + 4;
    // skip the leading zeroes, god knows what atoi() will do with that
    while (*num && *num == '0') ++num;
    if (!*num) break;
    const uint32_t musnum = atoi(num);
    if (musnum > 0 && musnum < MUSIC_COUNT && find_id(musnum, songlist, count) < 0) {
      if (count < limit)
        songlist[count++] = musnum;
      else
        printf("warning: too many songs for stage\n");
    }
    cmd = strstr(num, "<CMU");
  }

  return count;
}

int tsc_scan_transitions(const char *src, uint32_t *linklist, const uint32_t ignore, const int limit) {
  if (!src || !src[0])
    return 0;

  int count = 0;

  // very shitty and slow but who cares
  const char *tsc = src;
  const char *cmd = strstr(tsc, "<TRA");
  while (cmd) {
    const char *num = cmd + 4;
    // skip the leading zeroes, god knows what atoi() will do with that
    while (*num && *num == '0') ++num;
    if (!*num) break;
    const uint32_t linknum = atoi(num);
    if (linknum && ignore != linknum && find_id(linknum, linklist, count) < 0) {
      if (count < limit)
        linklist[count++] = linknum;
      // if (count > MAX_STAGE_LINKS)
      //   printf("warning: too many potential links from stage %02x\n", stage->id);
    }
    cmd = strstr(num, "<TRA");
  }

  return count;
}