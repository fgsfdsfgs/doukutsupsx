#include <string.h>

#include "engine/common.h"
#include "engine/graphics.h"
#include "engine/input.h"

#include "game/game.h"
#include "game/stage.h"
#include "game/player.h"
#include "game/npc.h"
#include "game/tsc.h"
#include "game/hud.h"

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

static gfx_texrect_t hud_rc_boss_label = {{ 0, 48, 32, 56 }};
static gfx_texrect_t hud_rc_boss_box1 = {{ 0, 0, 244, 8 }};
static gfx_texrect_t hud_rc_boss_box2 = {{ 0, 16, 244, 24 }};
static gfx_texrect_t hud_rc_boss_life = {{ 0, 24, 0, 32 }};
static gfx_texrect_t hud_rc_boss_flash = {{ 0, 32, 232, 40 }};

static gfx_texrect_t hud_rc_air[2] = {
  {{ 112, 72, 144, 80 }},
  {{ 112, 80, 144, 88 }},
};

static gfx_texrect_t hud_rc_time[3] = {
  {{ 112, 104, 120, 112 }},
  {{ 120, 104, 128, 112 }},
  {{ 128, 104, 160, 112 }},
};

gfx_texrect_t hud_rc_ammo[] = {
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

static int hud_boss_bar_max = 0;
static int hud_boss_bar_flash = 0;
static int hud_boss_bar_count = 0;
static s16 *hud_boss_life = NULL;

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

  gfx_set_texrect(&hud_rc_boss_box1, SURFACE_ID_TEXT_BOX);
  gfx_set_texrect(&hud_rc_boss_box2, SURFACE_ID_TEXT_BOX);
  gfx_set_texrect(&hud_rc_boss_life, SURFACE_ID_TEXT_BOX);
  gfx_set_texrect(&hud_rc_boss_flash, SURFACE_ID_TEXT_BOX);
  gfx_set_texrect(&hud_rc_boss_label, SURFACE_ID_TEXT_BOX);

  gfx_set_texrect(&hud_rc_air[0], SURFACE_ID_TEXT_BOX);
  gfx_set_texrect(&hud_rc_air[1], SURFACE_ID_TEXT_BOX);
 
  gfx_set_texrect(&hud_rc_time[0], SURFACE_ID_TEXT_BOX);
  gfx_set_texrect(&hud_rc_time[1], SURFACE_ID_TEXT_BOX);
  gfx_set_texrect(&hud_rc_time[2], SURFACE_ID_TEXT_BOX);

  hud_clear();
}

void hud_clear(void) {
  hud_boss_life = NULL;
  hud_boss_bar_count = 0;
  map_name_time = 0;
  map_name_xofs = 0;
}

void hud_update(void) {
  if (*hud_boss_life < 1)
    hud_boss_life = NULL;
}

void hud_draw_number(int val, int x, int y) {
  // drawing right to left
  do {
    const int d = val % 10;
    val /= 10;
    gfx_draw_texrect_8x8(&hud_rc_digit[d], GFX_LAYER_FRONT, x, y);
    x -= 8;
  } while(val);
}

static void hud_draw_time_number(int val, int x, int y) {
  char buf[16];
  // FIXME: this is probably slow as fuck, but I can't be arsed
  sprintf(buf, "%02d", val);
  buf[15] = 0;
  for (char *p = buf; *p; ++p) {
    gfx_draw_texrect_8x8(&hud_rc_digit[*p - '0'], GFX_LAYER_FRONT, x, y);
    x += 8;
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
    gfx_draw_texrect(&hud_rc_ammo[2], GFX_LAYER_FRONT, player.arms_x + 48, 16); // "--"
    gfx_draw_texrect(&hud_rc_ammo[2], GFX_LAYER_FRONT, player.arms_x + 48, 24); // "--"
  }

  if (player.shock / 2 % 2)
    return;

  int lv = player.arms[arm_id].level;
  gfx_draw_texrect_8x8(&hud_rc_ammo[0], GFX_LAYER_FRONT, player.arms_x + 32, 24); // "/"
  gfx_draw_texrect(&hud_rc_ammo[1], GFX_LAYER_FRONT, player.arms_x, 32); // "Lv"
  hud_draw_number(lv, player.arms_x + 16, 32);

  if (lv) --lv;

  const int exp_now = player.arms[arm_id].exp;
  const int exp_max = plr_arms_exptab[arm_id][lv];

  gfx_draw_texrect(&hud_rc_ammo[3], GFX_LAYER_FRONT, player.arms_x + 24, 32);

  if (lv == 2 && exp_max == exp_now) {
    // draw ==MAX==
    gfx_draw_texrect(&hud_rc_ammo[5], GFX_LAYER_FRONT, player.arms_x + 24, 32);
  } else {
    // draw bar
    if (exp_max)
      hud_rc_ammo[4].r.right = (exp_now * 40) / exp_max;
    else
      hud_rc_ammo[4].r.right = 0;
    gfx_draw_texrect(&hud_rc_ammo[4], GFX_LAYER_FRONT, player.arms_x + 24, 32);
  }

  // draw flashing bar
  if (player.exp_wait && ((player.exp_flash++ / 2) % 2))
    gfx_draw_texrect(&hud_rc_ammo[6], GFX_LAYER_FRONT, player.arms_x + 24, 32);
}

static inline void hud_draw_air(void) {
  if (player.equip & EQUIP_AIR_TANK)
    return;

  const int x = (VID_WIDTH / 2) - 32;
  const int y = (VID_HEIGHT / 2) - 16;

  if (player.air_count) {
    if (player.air_count % 6 < 4)
      hud_draw_number(player.air / 10, x + 56, y);
    gfx_draw_texrect(&hud_rc_air[player.air % 30 <= 10], GFX_LAYER_FRONT, x, y);
  }
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

  gfx_draw_texrect(&hud_rc_life_label, GFX_LAYER_FRONT, 16, 40);
  gfx_draw_texrect(&hud_rc_life_flash, GFX_LAYER_FRONT, 40, 40);
  gfx_draw_texrect(&hud_rc_life, GFX_LAYER_FRONT, 40, 40);
  hud_draw_number(player.life_bar, 32, 40);
}

static inline void hud_draw_boss_life(void) {
  const int life = *hud_boss_life;

  if (hud_boss_bar_flash > life) {
    if (++hud_boss_bar_count > 30)
      --hud_boss_bar_flash;
  } else {
    hud_boss_bar_count = 0;
  }

  hud_rc_boss_life.r.w = (life * 198) / hud_boss_bar_max;
  hud_rc_boss_flash.r.w = (hud_boss_bar_flash * 198) / hud_boss_bar_max;

  gfx_draw_texrect(&hud_rc_boss_box1, GFX_LAYER_FRONT, (VID_WIDTH / 2) - 120, VID_HEIGHT - 20 - 8);
  gfx_draw_texrect(&hud_rc_boss_box2, GFX_LAYER_FRONT, (VID_WIDTH / 2) - 120, VID_HEIGHT - 12 - 8);
  gfx_draw_texrect(&hud_rc_boss_flash, GFX_LAYER_FRONT, (VID_WIDTH / 2) - 80, VID_HEIGHT - 16 - 8);
  gfx_draw_texrect(&hud_rc_boss_life, GFX_LAYER_FRONT, (VID_WIDTH / 2) - 80, VID_HEIGHT - 16 - 8);
  gfx_draw_texrect(&hud_rc_boss_label, GFX_LAYER_FRONT, (VID_WIDTH / 2) - 112, VID_HEIGHT - 16 - 8);
}

static inline void hud_draw_debug(void) {
  // char line[80];
}

void hud_draw(void) {
  if (player.equip & EQUIP_NIKUMARU_COUNTER)
    hud_draw_time(16, 48);
  else
    game_stopwatch = 0;

  if (game_flags & GFLAG_INPUT_ENABLED) {
    hud_draw_life();
    hud_draw_ammo();
    hud_draw_air();
    hud_draw_arms();
  }

  if (hud_boss_life)
    hud_draw_boss_life();

  hud_draw_debug();
}

void hud_draw_map_name(void) {
  if (map_name_time) {
    const int x = (VID_WIDTH / 2) + map_name_xofs;
    const int y = (VID_HEIGHT / 2) - 48;
    const u8 shadow[3] = { 0x08, 0x00, 0x11 };
    gfx_draw_string_rgb(stage_data->title, shadow, GFX_LAYER_FRONT, x + 1, y + 1);
    gfx_draw_string(stage_data->title, GFX_LAYER_FRONT, x, y);
    --map_name_time;
  }
}

void hud_draw_time(const int x, const int y) {
  int blink = 0;
  if (game_flags & GFLAG_INPUT_ENABLED) {
    // assume this is called every frame
    if (game_stopwatch < 100 * 60 * 50)
      ++game_stopwatch;
    // blink icon to indicate passage of time
    blink = (game_stopwatch % 30 <= 10);
  }

  // draw clock icon
  gfx_draw_texrect_8x8(&hud_rc_time[blink], GFX_LAYER_FRONT, x, y);

  // draw time
  const int total_sec = game_stopwatch / 50;
  const int min = total_sec / 60;
  const int sec = total_sec % 60;
  const int sub = (game_stopwatch / 5) % 10;
  hud_draw_number(min, x + 24, y); // right aligned, no zero padding
  hud_draw_time_number(sec, x + 35, y); // zero padded
  gfx_draw_texrect_8x8(&hud_rc_digit[sub], GFX_LAYER_FRONT, x + 56, y);
  gfx_draw_texrect(&hud_rc_time[2], GFX_LAYER_FRONT, x + 30, y);
}

void hud_show_map_name(void) {
  map_name_time = 160;
  map_name_xofs = -GFX_FONT_WIDTH * strlen(stage_data->title) / 2;
}

void hud_init_boss_life(npc_t *npc) {
  hud_boss_life = &npc->life;
  hud_boss_bar_max = npc->life;
  hud_boss_bar_flash = npc->life;
}
