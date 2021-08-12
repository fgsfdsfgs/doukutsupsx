#include <string.h>

#include "engine/common.h"
#include "engine/timer.h"
#include "engine/org.h"

#include "game/game.h"
#include "game/player.h"
#include "game/npc.h"
#include "game/camera.h"
#include "game/profile.h"

#define PROFILE_MAGIC "CSPF"

profile_t profile;

void profile_reset(void) {
  memset(&profile, 0, sizeof(profile));
}

static void profile_save(void) {
  memcpy(profile.magic, PROFILE_MAGIC, 4);
  profile.game_tick = game_tick;

  // save globals
  memcpy(profile.npc_flags, npc_flags, sizeof(profile.npc_flags));
  memcpy(profile.map_flags, map_flags, sizeof(profile.map_flags));
  memcpy(profile.skip_flags, skip_flags, sizeof(profile.skip_flags));
  memcpy(profile.tele_dest, tele_dest, sizeof(profile.tele_dest));
  profile.tele_dest_num = tele_dest_num;
  profile.stage_id = stage_data->id;
  profile.stage_bank_id = stage_bank_id;
  profile.music_id = org_get_id();

  // save player
  memcpy(profile.player.arms, player.arms, sizeof(profile.player.arms));
  memcpy(profile.player.items, player.items, sizeof(profile.player.items));
  profile.player.x = player.x;
  profile.player.y = player.y;
  profile.player.dir = player.dir;
  profile.player.life = player.life;
  profile.player.max_life = player.max_life;
  profile.player.equip = player.equip;
  profile.player.star = player.star;
  profile.player.arm_id = player.arm;
  profile.player.unit = player.unit;

  printf("profile_save(): stage=%02x stage_bank=%02x\n", profile.stage_id, profile.stage_bank_id);
}

static bool profile_load(void) {
  if (memcmp(profile.magic, PROFILE_MAGIC, 4))
    return FALSE;

  game_reset();
  game_tick = profile.game_tick;

  // load globals
  memcpy(npc_flags, profile.npc_flags, sizeof(npc_flags));
  memcpy(map_flags, profile.map_flags, sizeof(map_flags));
  memcpy(skip_flags, profile.skip_flags, sizeof(skip_flags));
  memcpy(tele_dest, profile.tele_dest, sizeof(tele_dest));
  tele_dest_num = profile.tele_dest_num;

  // load stage bank
  timer_set_callback(timer_cb_music); // switch to IRQ based music
  gfx_draw_loading();
  stage_load_stage_bank(profile.stage_bank_id);
  timer_set_callback(timer_cb_ticker); // switch back to a simple ticker

  // after loading the stage bank we can get into the actual stage
  stage_transition(profile.stage_id, 0, 0, 1);
  stage_change_music(profile.music_id);

  // load player
  memcpy(player.arms, profile.player.arms, sizeof(player.arms));
  memcpy(player.items, profile.player.items, sizeof(player.items));
  player.x = profile.player.x;
  player.y = profile.player.y;
  player.dir = profile.player.dir;
  player.life = profile.player.life;
  player.max_life = profile.player.max_life;
  player.equip = profile.player.equip;
  player.star = profile.player.star;
  player.arm = profile.player.arm_id;
  player.unit = profile.player.unit;

  cam_clear_fade();

  return TRUE;
}

bool profile_write(void) {
  profile_save();
  // TODO: memcard stuff
  return TRUE;
}

bool profile_read(void) {
  // TODO: memcard stuff
  return profile_load();
}
