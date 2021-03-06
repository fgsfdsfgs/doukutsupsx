#include <string.h>

#include "engine/common.h"
#include "engine/timer.h"
#include "engine/org.h"

#include "game/game.h"
#include "game/player.h"
#include "game/npc.h"
#include "game/camera.h"
#include "game/menu.h"
#include "game/profile.h"

#define PROFILE_MAGIC "CSP\x02"

profile_t profile;
int profile_slot = -1;
u32 profile_stopwatch = 0;

void profile_reset(void) {
  memset(&profile, 0, sizeof(profile));
  profile_slot = -1;
  profile_stopwatch = 0;
}

void profile_save(void) {
  memcpy(profile.magic, PROFILE_MAGIC, 4);

  // save config
  profile.config.vol_sfx = snd_sfx_volume;
  profile.config.vol_music = org_get_master_volume();
  memcpy(profile.config.binds, input_binds, sizeof(profile.config.binds));

  // save globals
  profile.save.game_tick = game_tick;
  profile.save.clock_tick = profile_stopwatch;
  memcpy(profile.save.npc_flags, npc_flags, sizeof(profile.save.npc_flags));
  memcpy(profile.save.map_flags, map_flags, sizeof(profile.save.map_flags));
  memcpy(profile.save.skip_flags, skip_flags, sizeof(profile.save.skip_flags));
  memcpy(profile.save.tele_dest, tele_dest, sizeof(profile.save.tele_dest));
  profile.save.tele_dest_num = tele_dest_num;
  profile.save.stage_id = stage_data->id;
  profile.save.stage_bank_id = stage_bank_id;
  profile.save.music_id = org_get_id();

  if (profile_stopwatch) {
    // this is a post-game save with nikumaru timer on, save the record
    const int total_seconds = profile_stopwatch / 50;
    const int min = total_seconds / 60;
    const int sec = total_seconds % 60;
    const int sub = (profile_stopwatch / 5) % 10;
    sprintf(profile.save.stage_title, "= %d'%02d\"%d =", min, sec, sub);
  } else if (stage_data->id == STAGE_CREDITS_ID) {
    // this is a post-game save without nikumaru timer on, mark it as such
    strcpy(profile.save.stage_title, "= End =");
  } else {
    memcpy(profile.save.stage_title, stage_data->title, sizeof(profile.save.stage_title));
  }

  // save player
  memcpy(profile.save.player.arms, player.arms, sizeof(profile.save.player.arms));
  memcpy(profile.save.player.items, player.items, sizeof(profile.save.player.items));
  profile.save.player.x = player.x;
  profile.save.player.y = player.y;
  profile.save.player.dir = player.dir;
  profile.save.player.life = player.life;
  profile.save.player.max_life = player.max_life;
  profile.save.player.equip = player.equip;
  profile.save.player.star = player.star;
  profile.save.player.arm = player.arm;
  profile.save.player.num_arms = player.num_arms;
  profile.save.player.num_items = player.num_items;
  profile.save.player.unit = player.unit;

  printf("profile_save(): stage=%02x stage_bank=%02x\n", profile.save.stage_id, profile.save.stage_bank_id);
}

bool profile_load(const bool load_skipflags) {
  if (memcmp(profile.magic, PROFILE_MAGIC, 4))
    return FALSE;

  // don't reset skip flags, they're either going to be overwritten or we need to keep them as is
  game_reset(FALSE);

  // make sure camera is 100% faded out while we're loading shit
  cam_complete_fade();

  // load config
  memcpy(input_binds, profile.config.binds, sizeof(input_binds));
  snd_set_sfx_volume(profile.config.vol_sfx);
  org_set_master_volume(profile.config.vol_music);

  // load globals
  game_tick = profile.save.game_tick;
  game_stopwatch = profile_stopwatch = profile.save.clock_tick;
  tele_dest_num = profile.save.tele_dest_num;
  memcpy(npc_flags, profile.save.npc_flags, sizeof(npc_flags));
  memcpy(map_flags, profile.save.map_flags, sizeof(map_flags));
  memcpy(tele_dest, profile.save.tele_dest, sizeof(tele_dest));

  // only overwrite skip_flags if loading from main menu
  if (load_skipflags)
    memcpy(skip_flags, profile.save.skip_flags, sizeof(skip_flags));

  // load stage bank
  gfx_draw_loading();
  stage_load_stage_bank(profile.save.stage_bank_id);

  // after loading the stage bank we can get into the actual stage
  stage_transition(profile.save.stage_id, 0, 0, 1);
  stage_change_music(profile.save.music_id);

  // load player
  memcpy(player.arms, profile.save.player.arms, sizeof(player.arms));
  memcpy(player.items, profile.save.player.items, sizeof(player.items));
  player.x = profile.save.player.x;
  player.y = profile.save.player.y;
  player.dir = profile.save.player.dir;
  player.life = profile.save.player.life;
  player.max_life = profile.save.player.max_life;
  player.equip = profile.save.player.equip;
  player.star = profile.save.player.star;
  player.arm = profile.save.player.arm;
  player.num_arms = profile.save.player.num_arms;
  player.num_items = profile.save.player.num_items;
  player.unit = profile.save.player.unit;

  // recenter whimsical star projectiles
  plr_star_reset();

  // recenter camera since player position changed
  cam_center_on_player();

  // show camera
  cam_clear_fade();

  return TRUE;
}
