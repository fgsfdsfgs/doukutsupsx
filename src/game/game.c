#include <string.h>

#include "engine/common.h"
#include "engine/input.h"
#include "engine/org.h"

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
#include "game/menu.h"
#include "game/game.h"

u32 game_flags = GFLAG_INPUT_ENABLED;
u32 game_tick = 0;

u8 skip_flags[GAME_MAX_SKIPFLAGS];
u8 map_flags[GAME_MAX_MAPFLAGS];

// teleport destinations
tele_dest_t tele_dest[GAME_MAX_MAPFLAGS];
int tele_dest_num;

void game_init(void) {
  stage_init();
  tsc_init();
  npc_init(NPC_MAIN_TABLE);
  plr_init();
  bullet_init();
  caret_init();
  hud_init();
  cam_init();
  menu_init();

  // hide screen for now
  cam_complete_fade();
  cam_target_player(16);

  game_tick = 0;
}

void game_start(void) {
  // skip intro cutscene
  game_set_skipflag(5);
  // load start point
  stage_transition(13, 200, 10, 8);
  // stage_transition(62, 90, 80, 9);
  // stage_transition(56, 90, 80, 9);
  // stage_transition(2, 90, 5, 6);
}

void game_reset(void) {
  stage_reset();
  tsc_reset();
  npc_reset();
  plr_reset();
  dmgnum_init();
  cam_reset();
  hud_clear();
  caret_init();
  bullet_init();
  org_free();

  memset(skip_flags, 0, sizeof(skip_flags));
  memset(map_flags, 0, sizeof(map_flags));
  memset(tele_dest, 0, sizeof(tele_dest));
  tele_dest_num = 0;

  // hide screen for now
  cam_complete_fade();
  cam_target_player(16);

  game_flags = GFLAG_INPUT_ENABLED;
  game_tick = 0;
}

static inline void game_draw_common(void) {
  // stage draws into back and front
  stage_draw(camera.x, camera.y);
  // these are all on the back layer
  npc_draw(camera.x, camera.y);
  bullet_draw(camera.x, camera.y);
  plr_draw(camera.x, camera.y);
  // these are on the front layer
  cam_draw_flash();
  caret_draw(camera.x, camera.y);
  dmgnum_draw(camera.x, camera.y);
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

  // if there's a menu open, update that and bail
  const int cur_menu = menu_active();
  if (cur_menu) {
    menu_act();
    game_draw_common();
    menu_draw();
    if (cur_menu != MENU_PAUSE) {
      tsc_update();
      tsc_draw();
    }
    return;
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
  game_draw_common();

  // the only reason this exists is that the boss life counter
  // has to be checked *before* tsc_update()
  hud_update();

  if (!(game_flags & 4)) {
    if (input_trig & IN_INVENTORY) {
      menu_open(MENU_INVENTORY);
      return;
    } else if ((input_trig & IN_MAP) && (player.equip & EQUIP_MAP)) {
      menu_open(MENU_MAP);
      return;
    } else if (input_trig & IN_PAUSE) {
      menu_open(MENU_PAUSE);
      return;
    }
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

tele_dest_t *game_find_tele_dest(const u16 stage_id) {
  for (int i = 0; i < tele_dest_num; ++i) {
    if (stage_id == tele_dest[i].stage_num)
      return &tele_dest[i];
  }
  return NULL;
}

tele_dest_t *game_add_tele_dest(const u16 stage_id, const u16 event_num) {
  tele_dest_t *dest = game_find_tele_dest(stage_id);
  if (!dest) dest = &tele_dest[tele_dest_num++];
  dest->stage_num = stage_id;
  dest->event_num = event_num;
  return dest;
}
