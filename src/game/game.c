#include "engine/common.h"
#include "engine/input.h"

#include "game/npc.h"
#include "game/player.h"
#include "game/bullet.h"
#include "game/hit.h"
#include "game/camera.h"
#include "game/stage.h"
#include "game/game.h"

u32 game_flags = GFLAG_INPUT_ENABLED;
u32 game_tick = 0;

void game_init(void) {
  stage_init();
  npc_init(NPC_MAIN_TABLE);
  plr_init();
  bullet_init();
  cam_set_target(&player.x, &player.y, 16);

  // load start cave
  stage_transition(2, 200, 5, 6);
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
  // hit_boss_map();
  hit_bullet_map();
  hit_npc_bullet();
  // hit_boss_bullet();

  // bullets and particles update after other shit
  if (input_enabled)
    plr_arm_shoot();
  bullet_act();

  // animate the player
  plr_animate(btns_held);

  // move camera
  cam_update();

  // call `draw` on all entities
  stage_draw(camera.x, camera.y);
  npc_draw(camera.x, camera.y);
  bullet_draw(camera.x, camera.y);
  plr_draw(camera.x, camera.y);

  if (!(game_flags & 4)) {
    // TODO: inventory and map
  }

  if (input_enabled) {
    if (input_trig & IN_SWAP_L)
      plr_arm_swap_to_prev();
    else if (input_trig & IN_SWAP_R)
      plr_arm_swap_to_next();
  }

  // TODO: text script here

  // TODO: hud drawing here

  ++game_tick;
}
