#include "engine/common.h"
#include "engine/input.h"

#include "game/npc.h"
#include "game/player.h"
#include "game/bullet.h"
#include "game/caret.h"
#include "game/hit.h"
#include "game/camera.h"
#include "game/stage.h"
#include "game/tsc.h"
#include "game/dmgnum.h"
#include "game/hud.h"
#include "game/game.h"

u32 game_flags = GFLAG_INPUT_ENABLED;
u32 game_tick = 0;

u8 skip_flags[GAME_MAX_SKIPFLAGS];
u8 map_flags[GAME_MAX_MAPFLAGS];
u8 tele_flags[GAME_MAX_MAPFLAGS];

void game_init(void) {
  stage_init();
  tsc_init();
  npc_init(NPC_MAIN_TABLE);
  plr_init();
  bullet_init();
  caret_init();
  hud_init();
  cam_init();

  // hide screen for now
  cam_complete_fade();

  cam_target_player(16);
}

void game_start(void) {
  // skip intro cutscene
  game_set_skipflag(5);
  // load start point
  stage_transition(13, 200, 10, 8);
}

void game_frame(void) {
  const bool input_enabled = (game_flags & GFLAG_INPUT_ENABLED);

  u16 btns_held, btns_trig;
  if (input_enabled) {
    btns_held = input_held;
    btns_trig = input_trig;
  } else {
    btns_held = btns_trig = 0;
  }

  // call `act` on all entities (except bullets and particles for some reason)
  plr_act(btns_held, btns_trig);
  npc_act();
  // boss_act();
  stage_update();

  // call `hit` on all entities
  hit_player();
  hit_npc_map();
  hit_boss_map();
  hit_bullet_map();
  hit_npc_bullet();
  hit_boss_bullet();

  if (input_enabled)
    plr_arm_shoot();

  // bullets and particles update after other shit
  bullet_act();
  caret_act();
  dmgnum_act();

  // move camera and update fade/quake effects
  cam_update();

  // animate the player
  plr_animate(btns_held);

  // call `draw` on all entities
  stage_draw(camera.x, camera.y);
  npc_draw(camera.x, camera.y);
  bullet_draw(camera.x, camera.y);
  plr_draw(camera.x, camera.y);
  // these are on the front layer
  caret_draw(camera.x, camera.y);
  dmgnum_draw(camera.x, camera.y);

  if (!(game_flags & 4)) {
    // TODO: inventory and map
  }

  if (input_enabled) {
    if (input_trig & IN_SWAP_L)
      plr_arm_swap_to_prev();
    else if (input_trig & IN_SWAP_R)
      plr_arm_swap_to_next();
  }

  tsc_update();

  hud_draw();
  cam_draw_fade();
  tsc_draw();

  ++game_tick;
}
