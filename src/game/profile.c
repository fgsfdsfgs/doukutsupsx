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

#define PROFILE_MAGIC "CSPF"

profile_t profile;
int profile_slot = -1;

void profile_reset(void) {
  memset(&profile, 0, sizeof(profile));
  profile_slot = -1;
}

void profile_save(void) {
  memcpy(profile.magic, PROFILE_MAGIC, 4);
  profile.save.game_tick = game_tick;

  // save globals
  memcpy(profile.save.npc_flags, npc_flags, sizeof(profile.save.npc_flags));
  memcpy(profile.save.map_flags, map_flags, sizeof(profile.save.map_flags));
  memcpy(profile.save.skip_flags, skip_flags, sizeof(profile.save.skip_flags));
  memcpy(profile.save.tele_dest, tele_dest, sizeof(profile.save.tele_dest));
  memcpy(profile.save.stage_title, stage_data->title, sizeof(profile.save.stage_title));
  profile.save.tele_dest_num = tele_dest_num;
  profile.save.stage_id = stage_data->id;
  profile.save.stage_bank_id = stage_bank_id;
  profile.save.music_id = org_get_id();

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
  profile.save.player.arm_id = player.arm;
  profile.save.player.unit = player.unit;

  printf("profile_save(): stage=%02x stage_bank=%02x\n", profile.save.stage_id, profile.save.stage_bank_id);
}

bool profile_load(void) {
  if (memcmp(profile.magic, PROFILE_MAGIC, 4))
    return FALSE;

  game_reset();
  game_tick = profile.save.game_tick;

  // load globals
  memcpy(npc_flags, profile.save.npc_flags, sizeof(npc_flags));
  memcpy(map_flags, profile.save.map_flags, sizeof(map_flags));
  memcpy(skip_flags, profile.save.skip_flags, sizeof(skip_flags));
  memcpy(tele_dest, profile.save.tele_dest, sizeof(tele_dest));
  tele_dest_num = profile.save.tele_dest_num;

  // load stage bank
  timer_set_callback(timer_cb_music); // switch to IRQ based music
  gfx_draw_loading();
  stage_load_stage_bank(profile.save.stage_bank_id);
  timer_set_callback(timer_cb_ticker); // switch back to a simple ticker

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
  player.arm = profile.save.player.arm_id;
  player.unit = profile.save.player.unit;

  cam_clear_fade();

  return TRUE;
}
