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
  stage_transition(12, 200, 37, 11);
}

void game_update(void) {
  // call `act` on all entities (except bullets and particles for some reason)
  plr_act(input_held, input_trig);
  npc_act();
  // boss_act();

  // call `hit` on all entities
  hit_player(input_held);
  hit_npc_map();
  // hit_boss_map();
  hit_bullet_map();
  hit_npc_bullet();
  // hit_boss_bullet();

  // bullets and particles update after other shit
  // if (game_flags & GFLAG_INPUT_ENABLED)
  //   plr_shoot(input_held, input_trig);
  bullet_act();

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
  bullet_draw(camera.x, camera.y);
  plr_draw(camera.x, camera.y);
}
