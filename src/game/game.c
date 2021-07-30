#include "engine/common.h"
#include "engine/input.h"

#include "game/npc.h"
#include "game/player.h"
#include "game/hit.h"
#include "game/camera.h"
#include "game/stage.h"
#include "game/game.h"

u32 game_flags = 2;
u32 game_tick = 0;

void game_init(void) {
  stage_init();
  npc_init(NPC_MAIN_TABLE);
  plr_init();
  cam_set_target(&player.x, &player.y, 16);

  // load start cave
  stage_transition(12, 200, 37, 11);
}

void game_update(void) {
  // call `act` on all entities
  plr_act(input_held, input_trig);
  npc_act();
  // boss_act();

  // call `hit` on all entities
  hit_player(input_held);
  hit_npc();

  // animate the player
  plr_animate(input_held);

  // move camera
  cam_update();

  ++game_tick;
}

void game_draw(void) {
  // call `draw` on all entities
  stage_draw(camera.x, camera.y);
  npc_draw(camera.x, camera.y);
  plr_draw(camera.x, camera.y);
}
