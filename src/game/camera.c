#include "engine/common.h"

#include "game/stage.h"
#include "game/player.h"
#include "game/npc.h"
#include "game/camera.h"

cam_t camera;

static inline void cam_bound(void) {
  const int map_w = stage_data->width;
  const int map_h = stage_data->height;

  if (TO_INT(camera.x) < 0)
    camera.x = 0;
  if (TO_INT(camera.y) < 0)
    camera.y = 0;

  const int x_max = TO_FIX(((map_w - 1) * TILE_SIZE) - VID_WIDTH);
  const int y_max = TO_FIX(((map_h - 1) * TILE_SIZE) - VID_HEIGHT);

  if (camera.x > x_max)
    camera.x = x_max;
  if (camera.y > y_max)
    camera.y = y_max;
}

void cam_update(void) {
  camera.x += (*camera.tgt_x - (TO_FIX(VID_WIDTH) / 2) - camera.x) / camera.wait;
  camera.y += (*camera.tgt_y - (TO_FIX(VID_HEIGHT) / 2) - camera.y) / camera.wait;

  cam_bound();

  // Quake
  /*
  if (camera.quake2) {
    camera.x += (m_rand(-5, 5) * 0x200);
    camera.y += (m_rand(-3, 3) * 0x200);
    --camera.quake2;
  } else if (camera.quake) {
    camera.x += (m_rand(-1, 1) * 0x200);
    camera.y += (m_rand(-1, 1) * 0x200);
  }
  */
}

void cam_center_on_player(void) {
  camera.x = player.x - TO_FIX(VID_WIDTH / 2);
  camera.y = player.y - TO_FIX(VID_HEIGHT / 2);
  cam_bound();
}

void cam_target_player(const int delay) {
  camera.tgt_x = &player.tgt_x;
  camera.tgt_y = &player.tgt_y;
  camera.wait = delay;
}

void cam_target_npc(const int event_num, const int delay) {
  npc_t *npc = npc_find_by_event_num(event_num);
  if (npc) {
    camera.tgt_x = &npc->tgt_x;
    camera.tgt_y = &npc->tgt_y;
    camera.wait = delay;
  }
}

void cam_set_pos(const int fx, const int fy) {
  camera.x = fx;
  camera.y = fy;

  cam_bound();

  camera.quake = 0;
  camera.quake2 = 0;
}

void cam_set_target(s32 *tx, s32 *ty, const int wait) {
  if (wait >= 0)
    camera.wait = wait;
  camera.tgt_x = tx;
  camera.tgt_y = ty;
}
