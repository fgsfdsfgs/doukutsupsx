#include <string.h>

#include "engine/common.h"
#include "engine/graphics.h"
#include "engine/input.h"

#include "game/game.h"
#include "game/stage.h"
#include "game/player.h"
#include "game/tsc.h"

gfx_texrect_t hud_rc_digit[] = {
  // white numbers
  {{ 0,  56, 8,  64 }},
  {{ 8,  56, 16, 64 }},
  {{ 16, 56, 24, 64 }},
  {{ 24, 56, 32, 64 }},
  {{ 32, 56, 40, 64 }},
  {{ 40, 56, 48, 64 }},
  {{ 48, 56, 56, 64 }},
  {{ 56, 56, 64, 64 }},
  {{ 64, 56, 72, 64 }},
  {{ 72, 56, 80, 64 }},
  // red numbers
  {{ 0,  64, 8,  72 }},
  {{ 8,  64, 16, 72 }},
  {{ 16, 64, 24, 72 }},
  {{ 24, 64, 32, 72 }},
  {{ 32, 64, 40, 72 }},
  {{ 40, 64, 48, 72 }},
  {{ 48, 64, 56, 72 }},
  {{ 56, 64, 64, 72 }},
  {{ 64, 64, 72, 72 }},
  {{ 72, 64, 80, 72 }},
  // plus
  {{ 32, 48, 40, 56 }},
  // minus
  {{ 40, 48, 48, 56 }},
};

gfx_texrect_t hud_rc_item[PLR_MAX_ITEMS];
gfx_texrect_t hud_rc_arms[PLR_MAX_ARMS];

static gfx_texrect_t hud_rc_life_label = {{ 0, 40, 64, 48 }};
static gfx_texrect_t hud_rc_life = {{ 0, 24, 232, 32 }};
static gfx_texrect_t hud_rc_life_flash = {{ 0, 32, 232, 40 }};

static gfx_texrect_t hud_rc_air[2] = {
  {{ 112, 72, 144, 80 }},
  {{ 112, 80, 144, 88 }},
};

static gfx_texrect_t hud_rc_time[3] = {
  {{ 112, 104, 120, 112 }},
  {{ 120, 104, 128, 112 }},
  {{ 128, 104, 160, 112 }},
};

static gfx_texrect_t hud_rc_ammo[] = {
  {{ 72, 48, 80, 56 }}, // slash
  {{ 80, 80, 96, 88 }}, // Lv
  {{ 80, 48, 96, 56 }}, // "--"
  {{ 0,  72, 40, 80 }}, // ExpBox
  {{ 0,  80, 0,  88 }}, // ExpVal
  {{ 40, 72, 80, 80 }}, // ExpMax
  {{ 40, 80, 80, 88 }}, // ExpFlash
};

static int map_name_time = 0;
static int map_name_xofs = 0;

void hud_init(void) {
  for (u32 i = 0; i < sizeof(hud_rc_digit) / sizeof(*hud_rc_digit); ++i)
    gfx_set_texrect(&hud_rc_digit[i], SURFACE_ID_TEXT_BOX);

  for (u32 i = 0; i < sizeof(hud_rc_arms) / sizeof(*hud_rc_arms); ++i) {
    hud_rc_arms[i].r.left = i * 16;
    hud_rc_arms[i].r.right = hud_rc_arms[i].r.left + 16;
    hud_rc_arms[i].r.top = 0;
    hud_rc_arms[i].r.bottom = 16;
    gfx_set_texrect(&hud_rc_arms[i], SURFACE_ID_ARMS_IMAGE);
  }

  for (u32 i = 0; i < PLR_MAX_ITEMS; ++i) {
    hud_rc_item[i].r.left = (i % 8) * 32;
    hud_rc_item[i].r.top = (i / 8) * 16;
    hud_rc_item[i].r.right = hud_rc_item[i].r.left + 32;
    hud_rc_item[i].r.bottom = hud_rc_item[i].r.top + 16;
    gfx_set_texrect(&hud_rc_item[i], SURFACE_ID_ITEM_IMAGE);
  }

  for (u32 i = 0; i < sizeof(hud_rc_ammo) / sizeof(*hud_rc_ammo); ++i)
    gfx_set_texrect(&hud_rc_ammo[i], SURFACE_ID_TEXT_BOX);

  gfx_set_texrect(&hud_rc_life, SURFACE_ID_TEXT_BOX);
  gfx_set_texrect(&hud_rc_life_flash, SURFACE_ID_TEXT_BOX);
  gfx_set_texrect(&hud_rc_life_label, SURFACE_ID_TEXT_BOX);

  gfx_set_texrect(&hud_rc_air[0], SURFACE_ID_TEXT_BOX);
  gfx_set_texrect(&hud_rc_air[1], SURFACE_ID_TEXT_BOX);
 
  gfx_set_texrect(&hud_rc_time[0], SURFACE_ID_TEXT_BOX);
  gfx_set_texrect(&hud_rc_time[1], SURFACE_ID_TEXT_BOX); 
  gfx_set_texrect(&hud_rc_time[2], SURFACE_ID_TEXT_BOX); 
}

static const int numdiv[4] = { 1000, 100, 10, 1 };

static inline void hud_draw_number(int val, int x, int y) {
  // drawing right to left
  while (val) {
    const int d = val % 10;
    val /= 10;
    gfx_draw_texrect_8x8(&hud_rc_digit[d], GFX_LAYER_FRONT, x, y);
    x -= 8;
  }
}

static inline void hud_draw_ammo(void) {
  if (player.arms_x > 16)
    player.arms_x -= 2;
  if (player.arms_x < 16)
    player.arms_x += 2;

  const int arm_id = player.arm;

  if (player.arms[arm_id].max_ammo) {
    hud_draw_number(player.arms[arm_id].ammo, player.arms_x + 32 + 24, 16);
    hud_draw_number(player.arms[arm_id].max_ammo, player.arms_x + 32 + 24, 24);
  } else {
    gfx_draw_texrect(&hud_rc_ammo[2], GFX_LAYER_FRONT, player.arms_x + 48 - 8, 16 - 8); // "--"
    gfx_draw_texrect(&hud_rc_ammo[2], GFX_LAYER_FRONT, player.arms_x + 48 - 8, 24 - 8); // "--"
  }

  if (player.shock / 2 % 2)
    return;

  int lv = player.arms[arm_id].level;
  gfx_draw_texrect_8x8(&hud_rc_ammo[0], GFX_LAYER_FRONT, player.arms_x + 32, 24); // "/"
  gfx_draw_texrect(&hud_rc_ammo[1], GFX_LAYER_FRONT, player.arms_x - 8, 32 - 8); // "Lv"
  hud_draw_number(lv, player.arms_x + 16, 32);

  if (lv) --lv;

  const int exp_now = player.arms[arm_id].exp;
  const int exp_max = plr_arms_exptab[arm_id][lv];

  gfx_draw_texrect(&hud_rc_ammo[3], GFX_LAYER_FRONT, player.arms_x + 24 - 8, 32 - 8);

  if (lv == 2 && exp_max == exp_now) {
    // draw ==MAX==
    gfx_draw_texrect(&hud_rc_ammo[5], GFX_LAYER_FRONT, player.arms_x + 24 - 8, 32 - 8);
  } else {
    // draw bar
    if (exp_max)
      hud_rc_ammo[4].r.right = (exp_now * 40) / exp_max;
    else
      hud_rc_ammo[4].r.right = 0;
    gfx_draw_texrect(&hud_rc_ammo[4], GFX_LAYER_FRONT, player.arms_x + 24 - 8, 32 - 8);
  }

  // draw flashing bar
  if (player.exp_wait && ((player.exp_flash++ / 2) % 2))
    gfx_draw_texrect(&hud_rc_ammo[6], GFX_LAYER_FRONT, player.arms_x + 24 - 8, 32 - 8);
}

static inline void hud_draw_air(void) {
  if (player.equip & EQUIP_AIR_TANK)
    return;

  const int x = (VID_WIDTH / 2) - 40;
  const int y = (VID_HEIGHT / 2) - 16;

  if (player.air_count) {
    if (player.air_count % 6 < 4)
      hud_draw_number(player.air / 10, x + 48 + 8, y + 8);
    gfx_draw_texrect(&hud_rc_air[player.air % 30 <= 10], GFX_LAYER_FRONT, x, y);
  }
}

static inline void hud_draw_time(void) {

}

static inline void hud_draw_arms(void) {
  if (player.arm == 0)
    return;

  // this is one of the places where the original system wins
  int owned[PLR_MAX_ARMS];
  int numowned = 0;
  int selidx = 0;
  for (int i = 0; i < plr_arms_order_num; ++i) {
    const int arm_id = plr_arms_order[i];
    if (player.arms[arm_id].owned) {
      if (player.arm == arm_id)
        selidx = numowned;
      owned[numowned++] = arm_id;
    }
  }

  for (int i = 0; i < numowned; ++i) {
    const int arm_id = owned[i];
    int x = ((i - selidx) * 16) + player.arms_x;
    if (x < 8)
      x += 48 + (numowned * 16);
    else if (x >= 24)
      x += 48;
    if (x >= 72 + ((numowned - 1) * 16))
      x -= 48 + (numowned * 16);
    if (x < 72 && x >= 24)
      x -= 48;
    gfx_draw_texrect_16x16(&hud_rc_arms[arm_id], GFX_LAYER_FRONT, x, 16);
  }
}

static inline void hud_draw_life(void) {
  if (player.shock / 2 % 2)
    return;

  if (player.life_bar < player.life)
    player.life_bar = player.life;

  if (player.life_bar > player.life) {
    if (++player.life_count > 30)
      --player.life_bar;
  } else {
    player.life_count = 0;
  }

  hud_rc_life.r.w = ((player.life * 40) / player.max_life) - 1;
  hud_rc_life_flash.r.w = ((player.life_bar * 40) / player.max_life) - 1;

  gfx_draw_texrect(&hud_rc_life_label, GFX_LAYER_FRONT, 16 - 8, 40 - 8);
  gfx_draw_texrect(&hud_rc_life_flash, GFX_LAYER_FRONT, 40 - 8, 40 - 8);
  gfx_draw_texrect(&hud_rc_life, GFX_LAYER_FRONT, 40 - 8, 40 - 8);
  hud_draw_number(player.life_bar, 32, 40);
}

static inline void hud_draw_map_name(void) {
  const int x = (VID_WIDTH / 2) + map_name_xofs;
  const int y = (VID_HEIGHT / 2) - 40;
  const u8 shadow[3] = { 0x08, 0x00, 0x11 };
  gfx_draw_string_rgb(stage_data->title, shadow, GFX_LAYER_FRONT, x + 1, y + 1);
  gfx_draw_string(stage_data->title, GFX_LAYER_FRONT, x, y);
}

static inline void hud_draw_debug(void) {
  /*
  char line[80];
  snprintf(line, sizeof(line), "GFLAGS:   %08x", game_flags);
  gfx_draw_string(line, GFX_LAYER_FRONT, 8, 48);
  snprintf(line, sizeof(line), "TSC MODE: %02x", tsc_state.mode);
  gfx_draw_string(line, GFX_LAYER_FRONT, 8, 60);
  snprintf(line, sizeof(line), "IN TRIG:  %02x", input_trig);
  gfx_draw_string(line, GFX_LAYER_FRONT, 8, 72);
  snprintf(line, sizeof(line), "IN HELD:  %02x", input_held);
  gfx_draw_string(line, GFX_LAYER_FRONT, 8, 84);
  */
}

void hud_draw(void) {
  hud_draw_time();

  if (game_flags & GFLAG_INPUT_ENABLED) {
    hud_draw_life();
    hud_draw_ammo();
    hud_draw_air();
    hud_draw_arms();
  }

  if (map_name_time) {
    hud_draw_map_name();
    --map_name_time;
  }

  hud_draw_debug();
}

void hud_show_map_name(void) {
  map_name_time = 160;
  map_name_xofs = -GFX_FONT_WIDTH * strlen(stage_data->title) / 2;
}
