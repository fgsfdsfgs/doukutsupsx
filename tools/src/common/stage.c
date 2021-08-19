#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <ctype.h>

#include "common.h"
#include "vram.h"
#include "surface.h"
#include "stage.h"
#include "musiclist.h"
#include "tsc.h"

#define MAX_TOKEN 256
#define NUM_LIST_ARGS 10
#define NUM_STAGE_ARGS 8
#define NUM_BANK_ARGS 9
#define NUM_RECT_ARGS 4

#define PXM_MAGIC "PXM\x10"
#define PXE_MAGIC "PXE\x00"

static uint8_t tsc_head[TSC_MAX_SIZE + 1]; // space for a NUL just in case
static int tsc_head_size = 0;

static const struct { const char *name; int id; } bklist[] = {
  { "BACKGROUND_TYPE_STATIONARY", 0 },   // Doesn't move at all
  { "BACKGROUND_TYPE_MOVE_DISTANT", 1 }, // Moves at half the speed of the foreground
  { "BACKGROUND_TYPE_MOVE_NEAR", 2 },    // Moves at the same speed as the foreground
  { "BACKGROUND_TYPE_WATER", 3 },        // No background - draws a water foreground layer instead
  { "BACKGROUND_TYPE_BLACK", 4 },        // No background - just black
  { "BACKGROUND_TYPE_AUTOSCROLL", 5 },   // Constantly scrolls to the left (used by Ironhead)
  { "BACKGROUND_TYPE_CLOUDS_WINDY", 6 }, // Fancy parallax scrolling, items are blown to the left (used by bkMoon)
  { "BACKGROUND_TYPE_CLOUDS", 7 }        // Fancy parallax scrolling (used by bkFog)
};

static inline int parse_bktype(const char *token) {
  for (unsigned i = 0; i < sizeof(bklist) / sizeof(*bklist); ++i) {
    if (!strcasecmp(token, bklist[i].name))
      return bklist[i].id;
  }
  return -1;
}

static inline bool parse_stage_entry(char **args, stage_list_t *list, const int id) {
  char empty = '\0';

  for (int i = 0; i < NUM_STAGE_ARGS; ++i) {
    args[i] = trim_whitespace(args[i]);
    if (!strcmp(args[i], "0"))
      args[i] = &empty;
  }

  const char *tilesheet = args[0];
  strncpy(list->tilesheet, tilesheet, sizeof(list->tilesheet) - 1);

  const char *mapname = args[1];
  if (strlen(mapname) > MAX_RES_NAME - 1) {
    fprintf(stderr, "warning: map name '%s' is longer than %d chars\n", mapname, MAX_RES_NAME - 1);
    return false;
  }
  strncpy(list->name, mapname, sizeof(list->name) - 1);

  const int bktype = parse_bktype(args[2]);
  if (bktype < 0) {
    fprintf(stderr, "warning: unknown bg type '%s'\n", args[2]);
    return false;
  }
  list->bktype = bktype;

  const char *bksheet = args[3];
  strncpy(list->bksheet, bksheet, sizeof(list->bksheet) - 1);

  const char *npcsheet = args[4];
  strncpy(list->npcsheet, npcsheet, sizeof(list->npcsheet) - 1);

  const char *bosssheet = args[5];
  strncpy(list->bosssheet, bosssheet, sizeof(list->bosssheet) - 1);

  const int bossnum = atoi(args[6]);
  list->bossnum = bossnum;

  const char *title = args[7];
  if (strlen(title) > MAX_STAGE_TITLE - 1) {
    fprintf(stderr, "warning: map title '%s' is longer than %d chars\n", mapname, MAX_STAGE_TITLE - 1);
    return false;
  }
  strncpy(list->title, title, sizeof(list->title) - 1);

  // HACK: add extra title sheet to the title stage
  if (id == 72 || !strcmp(list->title, "u"))
    strncpy(list->titlesheet, "Title", sizeof(list->titlesheet) - 1);
  else
    list->titlesheet[0] = 0;

  return true;
}

static inline bool parse_bank_entry(char **args, stage_list_t *list, const int listlen) {
  for (int i = 0; i < NUM_BANK_ARGS; ++i) {
    args[i] = trim_whitespace(args[i]);
    if (!strcmp(args[i], "0") || !args[i][0])
      args[i] = NULL;
  }

  const char *rootname = args[0];
  if (!rootname || !rootname[0]) {
    fprintf(stderr, "warning: bank root must not be empty\n");
    return false;
  }

  stage_list_t *root = NULL;
  for (int i = 0; i < listlen; ++i) {
    if (!strcasecmp(list[i].name, rootname)) {
      root = &list[i];
      break;
    }
  }
  if (!root) {
    fprintf(stderr, "warning: unknown bank root stage '%s'\n", rootname);
    return false;
  }

  root->numlinks = 0;
  for (int a = 1; a < NUM_BANK_ARGS; ++a) {
    for (int i = 0; i < listlen; ++i) {
      if (args[a] && !strcasecmp(list[i].name, args[a])) {
        root->links[root->numlinks++] = i;
        break;
      }
    }
  }

  return true;
}

static inline bool parse_vram_surf_rect(char **args) {
  if (!isdigit(args[0][7])) {
    fprintf(stderr, "warning: SURFSET has to have a page numer at the end\n");
    return false;
  }
  const int pg = args[0][7] - '0';
  if (pg > VRAM_NUM_PAGES) {
    fprintf(stderr, "warning: SURFSET page number too high\n");
    return false;
  }
  const vram_rect_t r = { atoi(args[1]), atoi(args[2]), atoi(args[3]), atoi(args[4]) };
  vram_restrict_surf_rect(pg, &r);
  printf("surf page %d limited by rect: (%d, %d, %d, %d)\n", pg, r.x, r.y, r.w, r.h);
}

static inline bool parse_vram_clut_rect(char **args) {
  if (!isdigit(args[0][7])) {
    fprintf(stderr, "warning: CLUTSET has to have a page numer at the end\n");
    return false;
  }
  const int pg = args[0][7] - '0';
  if (pg > VRAM_NUM_CLUT_PAGES) {
    fprintf(stderr, "warning: CLUTSET page number too high\n");
    return false;
  }
  const vram_rect_t r = { atoi(args[1]), atoi(args[2]), atoi(args[3]), atoi(args[4]) };
  vram_restrict_clut_rect(pg, &r);
  printf("clut page %d limited by rect: (%d, %d, %d, %d)\n", pg, r.x, r.y, r.w, r.h);
}

int read_stagelist(stage_list_t *list, FILE *f) {
  char line[2048];
  char *token[1 + NUM_LIST_ARGS] = { NULL };
  char *p;
  int num = 0;

  while (true) {
    p = fgets(line, sizeof(line) - 1, f);
    if (!p) break;

    p = skip_whitespace(p);
    if (!*p || *p == '#') continue;

    int i = 0;
    token[0] = p = strtok(p, ",\r\n");
    for (i = 1; i < NUM_LIST_ARGS && p; ++i)
      token[i] = p = strtok(NULL, ",\r\n");
    if (i < NUM_RECT_ARGS || i > NUM_LIST_ARGS) {
      fprintf(stderr, "warning: ignoring malformed directive:\n%s\n", line);
      continue;
    }

    if (!strncasecmp(token[0], "STAGE", 5) && i >= NUM_STAGE_ARGS && num < MAX_STAGES)
      num += (int)parse_stage_entry(&token[1], &list[num], num);
    else if (!strncasecmp(token[0], "BANK", 4) && i >= NUM_BANK_ARGS)
      parse_bank_entry(&token[1], list, num);
    else if (!strncasecmp(token[0], "SURFSET", 7) && i >= NUM_RECT_ARGS)
      parse_vram_surf_rect(token);
    else if (!strncasecmp(token[0], "CLUTSET", 7) && i >= NUM_RECT_ARGS)
      parse_vram_clut_rect(token);
  }

  return num;
}

static uint8_t *stage_load_pxa(const char *path) {
  FILE *f = fopen(path, "rb");
  if (!f) return false;

  uint8_t *pxa = calloc(1, MAX_TILESET_SIZE * MAX_TILESET_SIZE);
  assert(pxa);

  if (fread(pxa, MAX_TILESET_SIZE * MAX_TILESET_SIZE, 1, f) != 1) {
    free(pxa);
    pxa = NULL;
  }

  fclose(f);

  return pxa;
}

static uint8_t *stage_load_pxm(const char *path, uint32_t *out_w, uint32_t *out_h) {
  struct {
    char magic[4]; // last character doesn't actually matter
    int16_t w;
    int16_t h;
  } __attribute__((packed)) header;

  FILE *f = fopen(path, "rb");
  if (!f) return NULL;

  fread(&header, sizeof(header), 1, f);

  if (memcmp(header.magic, PXM_MAGIC, sizeof(header.magic) - 1)) {
    fprintf(stderr, "error: '%s' has wrong PXM magic\n", path);
    fclose(f);
    return NULL;
  }

  if (header.w > MAX_STAGE_SIZE || header.h > MAX_STAGE_SIZE) {
    fprintf(stderr, "error: map '%s' is too large (%dx%d)\n", path, header.w, header.h);
    fclose(f);
    return NULL;
  }

  uint8_t *pxm = calloc(1, header.w * header.h);
  assert(pxm);

  if (fread(pxm, header.w * header.h, 1, f) != 1) {
    free(pxm);
    pxm = NULL;
  }

  fclose(f);

  *out_w = header.w;
  *out_h = header.h;

  return pxm;
}

static stage_event_t *stage_load_pxe(const char *path, int *out_count) {
  struct {
    char magic[4]; // last character doesn't actually matter
    uint32_t ev_count;
  } __attribute__((packed)) header;

  FILE *f = fopen(path, "rb");
  if (!f) return NULL;

  fread(&header, sizeof(header), 1, f);

  if (memcmp(header.magic, PXE_MAGIC, sizeof(header.magic) - 1)) {
    fprintf(stderr, "error: '%s' has wrong PXE magic\n", path);
    fclose(f);
    return NULL;
  }

  if (header.ev_count == 0) {
    *out_count = 0; // signal success anyway
    return NULL;
  }

  stage_event_t *pxe = calloc(1, header.ev_count * sizeof(*pxe));
  assert(pxe);

  if (fread(pxe, header.ev_count * sizeof(*pxe), 1, f) != 1) {
    free(pxe);
    pxe = NULL;
  } else {
    *out_count = header.ev_count;
  }

  fclose(f);

  return pxe;
}

static uint8_t *stage_load_tsc(const char *path, int *out_size, char **out_tscsrc) {
  FILE *f = fopen(path, "rb");
  if (!f) {
    *out_size = 0; // no tsc
    return NULL;
  }

  fseek(f, 0, SEEK_END);
  const int filelen = ftell(f);
  fseek(f, 0, SEEK_SET);

  char *tscdata = NULL;
  tsc_script_t *tsc = NULL;

  if (filelen > TSC_MAX_SIZE) {
    fprintf(stderr, "warning: TSC '%s' is too large (%d), ignoring\n", path, (int)filelen);
  } else if (filelen > 0) {
    int total = tsc_head_size + filelen + 1; // +terminating NUL
    tscdata = calloc(1, total);
    assert(tscdata);
    // copy in head.tsc
    memcpy(tscdata, tsc_head, tsc_head_size);
    // read the rest
    if (fread(tscdata + tsc_head_size, filelen, 1, f) == 1) {
      // head is already decoded
      tsc_decode(tscdata + tsc_head_size, filelen);
      // compile it
      tsc = tsc_compile(tscdata, total, &total);
      if (tsc) *out_size = total;
    }
  }

  if (tsc)
    *out_tscsrc = tscdata;
  else
    free(tscdata);

  fclose(f);

  return (uint8_t *)tsc;
}

stage_t *stage_load(const uint32_t id, const char *prefix, const char *tileprefix, char **out_tscsrc) {
  const char *sname = strrchr(prefix, '/') + 1;
  if ((uintptr_t)sname == 1)
    sname = strrchr(prefix, '\\') + 1;
  if ((uintptr_t)sname == 1)
    sname = prefix;
  if (!sname || !sname[0]) {
    fprintf(stderr, "error: empty name\n");
    return NULL;
  }

  char path[2048];
  uint8_t *pxa = NULL;
  uint8_t *pxm = NULL;
  stage_event_t *pxe = NULL;
  uint8_t *tsc = NULL;
  stage_t *stage = NULL;

  // load PXA
  if (tileprefix && tileprefix[0]) {
    snprintf(path, sizeof(path), "%s.pxa", tileprefix);
    pxa = stage_load_pxa(path);
    if (!pxa) {
      fprintf(stderr, "error: could not load atrb data from '%s'\n", path);
      goto _end;
    }
  }

  // load PXM
  uint32_t stage_w = 0;
  uint32_t stage_h = 0;
  snprintf(path, sizeof(path), "%s.pxm", prefix);
  pxm = stage_load_pxm(path, &stage_w, &stage_h);
  if (!pxm) {
    fprintf(stderr, "error: could not load map data from '%s'\n", path);
    goto _end;
  }

  // load PXE
  int num_ev = -1;
  snprintf(path, sizeof(path), "%s.pxe", prefix);
  pxe = stage_load_pxe(path, &num_ev);
  if (num_ev < 0) {
    fprintf(stderr, "error: could not load event data from '%s'\n", path);
    goto _end;
  }

  // load TSC
  int tsc_size = -1;
  char *tsc_src = NULL;
  snprintf(path, sizeof(path), "%s.tsc", prefix);
  tsc = stage_load_tsc(path, &tsc_size, &tsc_src);
  if (tsc_size < 0) {
    fprintf(stderr, "error: malformed tsc '%s'\n", path);
    goto _end;
  }

  const uint32_t ev_size = num_ev * sizeof(stage_event_t);
  // align the start of event data block to 4 bytes to avoid unaligned access
  const uint32_t ev_offset = ALIGN(stage_w * stage_h, 4);
  // same with the TSC
  const uint32_t tsc_offset = ALIGN(ev_offset + ev_size, 4);
  // total map size (extra bytes on top of the TSC for NUL termination)
  const uint32_t total = tsc_offset + ALIGN(tsc_size + 1, 4) + sizeof(*stage);

  stage = calloc(1, total);
  assert(stage);
  stage->id = id;
  stage->width = stage_w;
  stage->height = stage_h;
  stage->ev_count = num_ev;
  stage->ev_offset = ev_offset;
  stage->tsc_offset = tsc_offset;
  stage->tsc_size = ALIGN(tsc_size + 1, 4);
  if (pxa) memcpy(&stage->atrb[0][0], pxa, sizeof(stage->atrb));
  memcpy(&stage->map_data[0], pxm, stage_w * stage_h);
  if (num_ev) memcpy(&stage->map_data[ev_offset], pxe, sizeof(stage_event_t) * num_ev);
  if (tsc_size) memcpy(&stage->map_data[tsc_offset], tsc, tsc_size);
  if (out_tscsrc) *out_tscsrc = tsc_src;

_end:
  free(pxa);
  free(pxm);
  free(pxe);
  free(tsc);
  return stage;
}

bool stage_load_tsc_head(const char *path) {
  FILE *f = fopen(path, "rb");
  if (!f) return NULL;

  fseek(f, 0, SEEK_END);
  const intptr_t filelen = ftell(f);
  fseek(f, 0, SEEK_SET);

  bool ret = false;

  if (filelen > (intptr_t)sizeof(tsc_head)) {
    fprintf(stderr, "error: head TSC '%s' is too large (%d)\n", path, (int)filelen);
  } else if (filelen > 0) {
    // read the head
    if (fread(tsc_head, filelen, 1, f) == 1) {
      tsc_head_size = filelen;
      tsc_decode(tsc_head, tsc_head_size);
      ret = true;
    }
  }

  fclose(f);

  printf("\nhead tsc loaded from '%s': %d bytes\n", path, tsc_head_size);

  return ret;
}

static inline bool stages_linkable(const stage_list_t *a, const stage_list_t *b) {
  return
    (!a->tilesheet[0] || !b->tilesheet[0] || !strcasecmp(a->tilesheet, b->tilesheet)) &&
    (!a->npcsheet[0]  || !b->npcsheet[0]  || !strcasecmp(a->npcsheet,  b->npcsheet))  &&
    (!a->bosssheet[0] || !b->bosssheet[0] || !strcasecmp(a->bosssheet, b->bosssheet)) &&
    (!a->bksheet[0]   || !b->bksheet[0]   || !strcasecmp(a->bksheet,   b->bksheet));
}

uint32_t stage_write_bank(const stage_list_t *root, const stage_list_t *stlist, const char *datapath, FILE *f) {
  // compile actual stage list, merging song and surface lists
  // TODO: maybe add adjustable recursion levels

  uint32_t ret = 0;

  uint32_t num_stages = 1;
  const stage_list_t *stage_list[1 + root->numlinks];
  uint32_t stage_sizes[1 + root->numlinks];

  uint32_t num_songs = 0;
  uint32_t song_list[MAX_STAGE_SONGS * num_stages];

  uint32_t num_surf = 0;
  const char *surf_list[(1 + root->numlinks) * 5]; // (npc, boss, tile, bk, title) * num
  uint32_t surf_id_list[(1 + root->numlinks) * 5];
  int bg_width = 0;
  int bg_height = 0;

  stage_list[0] = root;
  for (int i = 0; i < root->numlinks && num_stages < MAX_STAGE_LINKS; ++i) {
    // only link together stages that share all the graphics
    if (stages_linkable(root, &stlist[root->links[i]]))
      stage_list[num_stages++] = &stlist[root->links[i]];
  }

  // merge resource lists for all stages
  for (uint32_t i = 0; i < num_stages; ++i) {
    const stage_list_t *list = stage_list[i];
    // merge the song list
    for (int s = 0; s < list->numsongs; ++s) {
      if (find_id(list->songs[s], song_list, num_songs) < 0)
        song_list[num_songs++] = list->songs[s];
    }
    // merge the surface list
    const char *tmpsurf[] = { list->npcsheet, list->bosssheet, list->tilesheet, list->bksheet, list->titlesheet };
    const uint32_t tmpid[] = {
      SURFACE_ID_LEVEL_SPRITESET_1, SURFACE_ID_LEVEL_SPRITESET_2,
      SURFACE_ID_LEVEL_TILESET, SURFACE_ID_LEVEL_BACKGROUND,
      SURFACE_ID_TITLE,
    };
    for (uint32_t s = 0; s < sizeof(tmpsurf) / sizeof(*tmpsurf); ++s) {
      if (tmpsurf[s][0] && find_id(tmpid[s], surf_id_list, num_surf) < 0) {
        surf_id_list[num_surf] = tmpid[s];
        surf_list[num_surf++] = tmpsurf[s];
      }
    }
  }

  // convert surfaces
  char path[2048];
  const char *prefix;
  struct bitmap bmp;
  for (uint32_t i = 0; i < num_surf; ++i) {
    if (surf_id_list[i] == SURFACE_ID_LEVEL_SPRITESET_1 || surf_id_list[i] == SURFACE_ID_LEVEL_SPRITESET_2)
      prefix = "Npc/Npc";
    else if (surf_id_list[i] == SURFACE_ID_LEVEL_BACKGROUND || surf_id_list[i] == SURFACE_ID_TITLE)
      prefix = "";
    else
      prefix = "Stage/Prt";

    snprintf(path, sizeof(path), "%s/%s%s.pbm", datapath, prefix, surf_list[i]);
    bmp.data = NULL;
    bmp.leftpad = 0;

    FILE *bmpf = fopen(path, "rb");
    if (!bmpf) {
      fprintf(stderr, "error: could not open bitmap '%s'\n", path);
      goto _end;
    }

    const int rc = read_bmp(&bmp, bmpf);
    fclose(bmpf);
    if (rc < 0) {
      fprintf(stderr, "error: could not read bitmap '%s': %d\n", path, rc);
      goto _end;
    }

    if (convert_surface(surf_id_list[i], &bmp) < 0) {
      fprintf(stderr, "error: could not convert surface '%s'\n", path);
      goto _end;
    }

    // save background size
    if (surf_id_list[i] == SURFACE_ID_LEVEL_BACKGROUND) {
      bg_width = bmp.width;
      bg_height = bmp.height;
    }

    free(bmp.data);
    bmp.data = NULL;
  }

  const uint32_t hdr_size = sizeof(stagebank_t) + sizeof(stage_song_t) * num_songs;
  stagebank_t *hdr = calloc(1, hdr_size);
  assert(hdr);
  hdr->numsongs = num_songs;
  hdr->numstages = num_stages;
  hdr->bk_width = bg_width;
  hdr->bk_height = bg_height;

  // write temporary header
  fwrite(hdr, hdr_size, 1, f);

  // calculate stage offsets and sizes
  uint32_t ofs = ftell(f);
  for (uint32_t i = 0; i < num_stages; ++i) {
    hdr->stageofs[i] = ofs;
    stage_sizes[i] = (uint32_t)stage_list[i]->stage->tsc_offset + (uint32_t)stage_list[i]->stage->tsc_size + sizeof(stage_t);
    ofs += ALIGN(stage_sizes[i], 4);
  }

  // write stages
  for (uint32_t i = 0; i < num_stages; ++i) {
    const uint32_t pad = 0;
    const int padsize = ALIGN(stage_sizes[i], 4) - stage_sizes[i];
    fwrite(stage_list[i]->stage, stage_sizes[i], 1, f);
    if (padsize) fwrite(&pad, padsize, 1, f);
  }

  // sanity check
  assert(ftell(f) == ofs);

  // write songs
  uint32_t fsize = 0;
  uint8_t *fdata = NULL;
  for (uint32_t i = 0; i < num_songs; ++i) {
    hdr->songs[i].music_id = song_list[i];
    hdr->songs[i].bank_ofs = ftell(f);
    // read bank
    snprintf(path, sizeof(path), "%s/sfxbnk/%s.sfx", datapath, music_names[song_list[i]]);
    strlwr(path + strlen(datapath) + 1 + 4);
    fdata = read_file(path, &fsize);
    if (!fdata) {
      fprintf(stderr, "error: could not read '%s'\n", path);
      goto _end;
    }
    // write bank
    fwrite(fdata, fsize, 1, f);
    free(fdata);
    // read org
    snprintf(path, sizeof(path), "%s/org/%s.org", datapath, music_names[song_list[i]]);
    strlwr(path + strlen(datapath) + 1 + 4);
    fdata = read_file(path, &fsize);
    if (!fdata) {
      fprintf(stderr, "error: could not read '%s'\n", path);
      goto _end;
    }
    // write bank
    fwrite(fdata, fsize, 1, f);
    free(fdata);
  }

  // write surface bank
  hdr->surfofs = ftell(f);
  vram_write_surf_bank(f);
  // snprintf(path, 2048, "out/stage/%02x.png", root->stage->id & 0xFF);
  // vram_export_png(path);

  // rewrite updated header
  fseek(f, 0, SEEK_SET);
  fwrite(hdr, hdr_size, 1, f);

  ret = num_stages;

_end:
  if (bmp.data) free(bmp.data);
  if (hdr) free(hdr);
  vram_reset();

  return ret;
}
