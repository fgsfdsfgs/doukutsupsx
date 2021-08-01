#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include "common/common.h"
#include "common/stage.h"
#include "common/tsc.h"

static stage_list_t stages[MAX_STAGES];
static int numstages = 0;

static void cleanup(void) {
  for (int i = 0; i < numstages; ++i)
    free(stages[i].stage);
}

int main(int argc, char **argv) {
  if (argc < 3) {
    printf("usage: stagepack <stage_list> <data_path> <out_dir>\n");
    return -1;
  }

  const char *listname = argv[1];
  const char *datapath = argv[2];
  const char *outpath = argv[3];

  // init stage list
  for (int i = 0; i < MAX_STAGES; ++i)
    stages[i].numlinks = -1;

  FILE *f = fopen(listname, "r");
  if (!f) {
    fprintf(stderr, "error: could not open stage list file '%s'\n", listname);
    return -2;
  }

  numstages = read_stagelist(stages, f);
  printf("\nparsed %d stages from '%s'\n", numstages, listname);

  fclose(f);

  char path[2048];
  char tpath[2048];

  // load head.tsc
  snprintf(path, sizeof(path), "%s/Head.tsc", datapath);
  if (!stage_load_tsc_head(path)) {
    fprintf(stderr, "error: could not load '%s'\n", path);
    return -3;
  }

  atexit(cleanup);

  for (int i = 0; i < numstages; ++i) {
    // skip NULL stages
    if (!stages[i].name[0]) continue;

    snprintf(path, sizeof(path), "%s/Stage/%s", datapath, stages[i].name);
    if (stages[i].tilesheet[0])
      snprintf(tpath, sizeof(tpath), "%s/Stage/%s", datapath, stages[i].tilesheet);
    else
      tpath[0] = '\0';

    // load the map data and tsc
    char *tsc_src = NULL;
    stage_t *stage = stage_load(i, path, tpath, &tsc_src);
    stages[i].stage = stage;
    if (!stages[i].stage) {
      fprintf(stderr, "error: could not load stage '%s'\n", path);
      return -4;
    }

    printf("loaded stage '%8.8s' (id %02x, size %03ux%03u)\n", stages[i].name, stage->id, stage->width, stage->height);

    // fill in the rest of the fields
    stage->boss_type = stages[i].bossnum;
    stage->bk_type = stages[i].bktype;
    strncpy(stage->title, stages[i].title, sizeof(stage->title) - 1);

    //scan for songs used by the stage
    stages[i].numsongs = tsc_scan_music(tsc_src, stages[i].songs, MAX_STAGE_SONGS);

    // scan for transitions if they weren't specified manually with a BANK directive
    if (stages[i].numlinks < 0)
      stages[i].numlinks = tsc_scan_transitions(tsc_src, stages[i].links, stage->id, MAX_STAGE_LINKS);
  
    free(tsc_src);
  }

  puts("");

  // save out banks for each stage
  // the game will know when to read a new bank from CD
  for (int i = 0; i < numstages; ++i) {
    // skip NULL stages and stages that are already in a bank
    if (!stages[i].name[0] || stages[i].packed)
      continue;

    snprintf(tpath, sizeof(tpath), "%s/%01x", outpath, stages[i].stage->id >> 4);
    plat_mkdir(tpath, 0755);

    snprintf(path, sizeof(path), "%s/stage%02x.stg", tpath, stages[i].stage->id);
    FILE *f = fopen(path, "wb");
    if (!f) {
      fprintf(stderr, "error: could not open '%s' for writing\n", path);
      return -5;
    }

    const uint32_t num = stage_write_bank(&stages[i], stages, datapath, f);
    if (!num) {
      fclose(f);
      fprintf(stderr, "error: could not write bank for stage '%s' (%u)\n", stages[i].name, stages[i].stage->id);
      return -6;
    }

    fclose(f);

    printf("wrote %u stage(s) into bank '%s'\n", num, path);
  }

  return 0;
}
