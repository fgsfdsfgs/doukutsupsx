#include "engine/common.h"
#include "engine/graphics.h"
#include "engine/sound.h"
#include "engine/input.h"

#include "game/player.h"
#include "game/stage.h"
#include "game/game.h"
#include "game/hud.h"
#include "game/tsc.h"
#include "game/menu.h"

static int menu_id = MENU_NONE;

// triangle, circle, cross, square
static gfx_texrect_t rc_padbuttons[4];

static gfx_texrect_t rc_pause = {{ 0, 132, 126, 144 }};

static gfx_texrect_t rc_menubox[3] = {
  {{ 0, 0,  244, 8  }},
  {{ 0, 8,  244, 16 }},
  {{ 0, 16, 244, 24 }},
};

static gfx_texrect_t rc_cursor_arms[2] = {
  {{ 0,  88, 40, 128 }},
  {{ 40, 88, 80, 128 }},
};
static gfx_texrect_t rc_cursor_items[2] = {
  {{ 80, 88,  112, 104 }},
  {{ 80, 104, 112, 120 }},
};

static gfx_texrect_t rc_title[2] = {
  {{ 80, 48, 144, 56 }},
  {{ 80, 56, 144, 64 }},
};

void menu_init(void) {
  menu_id = MENU_NONE;

  for (int i = 0; i < 4; ++i) {
    rc_padbuttons[i].r.left = 196 + i * 12;
    rc_padbuttons[i].r.top = 132;
    rc_padbuttons[i].r.right = rc_padbuttons[i].r.left + 12;
    rc_padbuttons[i].r.bottom = rc_padbuttons[i].r.top + 12;
    gfx_set_texrect(&rc_padbuttons[i], SURFACE_ID_TEXT_BOX);
  }

  for (int i = 0; i < 3; ++i)
    gfx_set_texrect(&rc_menubox[i], SURFACE_ID_TEXT_BOX);

  for (int i = 0; i < 2; ++i) {
    gfx_set_texrect(&rc_cursor_arms[i], SURFACE_ID_TEXT_BOX);
    gfx_set_texrect(&rc_cursor_items[i], SURFACE_ID_TEXT_BOX);
    gfx_set_texrect(&rc_title[i], SURFACE_ID_TEXT_BOX);
  }

  gfx_set_texrect(&rc_pause, SURFACE_ID_TEXT_BOX);
}

/* default func */

static void menu_null(void) {
  // nada
}

/* pause */

static void menu_pause_act(void) {
  if (input_trig & IN_OK) {
    menu_id = MENU_NONE;
    game_reset();
    game_start();
  } else if (input_trig & (IN_CANCEL | IN_PAUSE)) {
    menu_id = MENU_NONE;
  }
}

static void menu_pause_draw(void) {
  // clear screen
  gfx_draw_fillrect(gfx_clear_rgb, GFX_LAYER_FRONT, 0, 0, VID_WIDTH, VID_HEIGHT);
  // draw the prompt
  const int x = (VID_WIDTH - rc_pause.r.w) / 2 - 8;
  const int y = (VID_HEIGHT - rc_pause.r.h) / 2 - 8;
  gfx_draw_texrect(&rc_pause, GFX_LAYER_FRONT, x, y);
  // TODO: draw the buttons bound to OK/CANCEL
  gfx_draw_texrect(&rc_padbuttons[1], GFX_LAYER_FRONT, x, y - 1);
  gfx_draw_texrect(&rc_padbuttons[2], GFX_LAYER_FRONT, x + 78, y - 1);
}

/* inventory */

#define INV_BOX_LEFT TEXT_BOX_LEFT
#define INV_BOX_TOP  ((VID_HEIGHT / 2) - 116)
#define INV_CUR_LEFT (INV_BOX_LEFT + 10)
#define INV_CUR_TOP  (INV_BOX_TOP + 16)
#define INV_ITEM_TOP (INV_CUR_TOP + 52)

static struct {
  s32 arms_owned[PLR_MAX_ARMS];
  s32 num_arms;
  s32 arm_idx;
  s32 items_owned[PLR_MAX_ITEMS];
  s32 num_items;
  s32 item_idx;
  s32 title_y;
  u32 flash;
  bool in_items;
} inventory;

static void menu_inventory_open(void) {
  tsc_switch_script(TSC_SCRIPT_ARMS_ITEM);

  // this is one of the places where the original system wins

  inventory.in_items = FALSE;
  inventory.title_y = (VID_HEIGHT / 2) - 104;

  inventory.num_arms = 0;
  inventory.arm_idx = 0;
  inventory.arms_owned[0] = 0;
  for (int i = 0; i < plr_arms_order_num; ++i) {
    const int arm_id = plr_arms_order[i];
    if (player.arms[arm_id].owned) {
      if (player.arm == arm_id)
        inventory.arm_idx = inventory.num_arms;
      inventory.arms_owned[inventory.num_arms++] = arm_id;
    }
  }

  inventory.num_items = 0;
  inventory.item_idx = 0;
  inventory.items_owned[0] = 0;
  for (int i = 0; i < PLR_MAX_ITEMS; ++i) {
    if (player.items[i])
      inventory.items_owned[inventory.num_items++] = i;
  }

  if (inventory.num_arms)
    tsc_start_event(1000 + inventory.arms_owned[inventory.arm_idx]);
  else
    tsc_start_event(5000 + inventory.items_owned[inventory.item_idx]);
}

static inline void menu_inventory_controls(void) {
  if (!inventory.num_items && !inventory.num_arms)
    return; // empty

  bool changed = FALSE;

  if (inventory.in_items) {
    // in items menu

    if (input_trig & IN_LEFT) {
      if (inventory.item_idx % 6)
        --inventory.item_idx;
      else
        inventory.item_idx += 5; // loop around to end of row
      changed = TRUE;
    } else if (input_trig & IN_RIGHT) {
      if (inventory.item_idx == inventory.num_items - 1)
        inventory.item_idx = (inventory.item_idx / 6) * 6; // loop around to end of row
      else if (inventory.item_idx % 6 == 5)
        inventory.item_idx -= 5; // loop around to end of row
      else
        ++inventory.item_idx;
      changed = TRUE;
    }

    if (input_trig & IN_UP) {
      if (inventory.item_idx < 6)
        inventory.in_items = FALSE; // cursor was on row 1, swap to arms menu
      else
        inventory.item_idx -= 6; // move one row up
      changed = TRUE;
    } else if (input_trig & IN_DOWN) {
      if (inventory.item_idx / 6 == (inventory.num_items - 1) / 6)
        inventory.in_items = FALSE; // cursor was on last row, swap to arms menu
      else
        inventory.item_idx += 6; // move one row down
      changed = TRUE;
    }

    // keep index in range
    if (inventory.item_idx >= inventory.num_items)
      inventory.item_idx = inventory.num_items - 1;

    // select item if OK is pressed and we're still in items
    if ((inventory.in_items) && (input_trig & IN_OK))
      tsc_start_event(6000 + inventory.items_owned[inventory.item_idx]);
  } else {
    // in arms menu

    if (input_trig & IN_LEFT) {
      --inventory.arm_idx;
      changed = TRUE;
    } else if (input_trig & IN_RIGHT) {
      ++inventory.arm_idx;
      changed = TRUE;
    }

    if (input_trig & (IN_UP | IN_DOWN)) {
      // if there are items, change to items menu
      if (inventory.num_items)
        inventory.in_items = TRUE;
      changed = TRUE;
    }

    // keep index in range
    if (inventory.arm_idx < 0)
      inventory.arm_idx = inventory.num_arms - 1;
    if (inventory.arm_idx >= inventory.num_arms)
      inventory.arm_idx = 0;
  }

  if (changed) {
    if (inventory.in_items) {
      // change item and display new item's name
      snd_play_sound(CHAN_MISC, 1, SOUND_MODE_PLAY);
      tsc_start_event(5000 + inventory.items_owned[inventory.item_idx]);
    } else {
      // change weapon and display new weapon's name
      snd_play_sound(CHAN_MISC, 4, SOUND_MODE_PLAY);
      player.arm = inventory.arms_owned[inventory.arm_idx];
      tsc_start_event(1000 + inventory.arms_owned[inventory.arm_idx]);
    }
  }
}

static void menu_inventory_act(void) {
  if (game_flags & GFLAG_INPUT_ENABLED)
    menu_inventory_controls();

  // check for exit buttons
  if (inventory.in_items) {
    if ((game_flags & GFLAG_INPUT_ENABLED) && (input_trig & (IN_INVENTORY | IN_CANCEL | IN_PAUSE))) {
      tsc_stop_event();
      tsc_switch_script(TSC_SCRIPT_STAGE);
      menu_id = MENU_NONE;
    }
  } else {
    if (input_trig & (IN_INVENTORY | IN_CANCEL | IN_PAUSE | IN_OK)) {
      tsc_stop_event();
      tsc_switch_script(TSC_SCRIPT_STAGE);
      menu_id = MENU_NONE;
    }
  }
}

static void menu_inventory_draw(void) {
  int i;

  // draw box
  gfx_draw_texrect(&rc_menubox[0], GFX_LAYER_FRONT, INV_BOX_LEFT, INV_BOX_TOP);
  for (i = 1; i < 18; ++i)
    gfx_draw_texrect(&rc_menubox[1], GFX_LAYER_FRONT, INV_BOX_LEFT, INV_BOX_TOP + (i * 8));
  gfx_draw_texrect(&rc_menubox[2], GFX_LAYER_FRONT, INV_BOX_LEFT, INV_BOX_TOP + (i * 8));

  // update title offset
  if (inventory.title_y > (VID_HEIGHT / 2) - 112)
    --inventory.title_y;
  // draw titles
  gfx_draw_texrect(&rc_title[0], GFX_LAYER_FRONT, INV_CUR_LEFT, inventory.title_y);
  gfx_draw_texrect(&rc_title[1], GFX_LAYER_FRONT, INV_CUR_LEFT, inventory.title_y + 52);

  ++inventory.flash;

  // draw arms cursor
  const int flash_idx = ((inventory.flash / 2) % 2);
  int cur_idx = (inventory.in_items) ? 1 : flash_idx;
  gfx_draw_texrect(&rc_cursor_arms[cur_idx], GFX_LAYER_FRONT, (inventory.arm_idx * 40) + INV_CUR_LEFT, INV_CUR_TOP);

  // draw arms
  for (i = 0; i < inventory.num_arms; ++i) {
    const int arm_id = inventory.arms_owned[i];
    if (arm_id == 0) break; // probably nothing else owned

    // draw icon, slash and Lv
    gfx_draw_texrect(&hud_rc_arms[arm_id], GFX_LAYER_FRONT, (i * 40) + INV_CUR_LEFT, INV_CUR_TOP);
    gfx_draw_texrect(&hud_rc_ammo[0], GFX_LAYER_FRONT, (i * 40) + INV_CUR_LEFT, INV_CUR_TOP + 32);
    gfx_draw_texrect(&hud_rc_ammo[1], GFX_LAYER_FRONT, (i * 40) + INV_CUR_LEFT, INV_CUR_TOP + 16);
    hud_draw_number(player.arms[arm_id].level, (i * 40) + INV_CUR_LEFT + 32, INV_CUR_TOP + 16 + 8);

    // draw ammo (or "--" if infinite)
    if (player.arms[arm_id].max_ammo) {
      hud_draw_number(player.arms[arm_id].ammo, (i * 40) + INV_CUR_LEFT + 32, INV_CUR_TOP + 24 + 8);
      hud_draw_number(player.arms[arm_id].max_ammo, (i * 40) + INV_CUR_LEFT + 32, INV_CUR_TOP + 32 + 8);
    } else {
      gfx_draw_texrect(&hud_rc_ammo[2], GFX_LAYER_FRONT, (i * 40) + INV_CUR_LEFT + 16, INV_CUR_TOP + 24); // "--"
      gfx_draw_texrect(&hud_rc_ammo[2], GFX_LAYER_FRONT, (i * 40) + INV_CUR_LEFT + 16, INV_CUR_TOP + 32); // "--"
    }
  }

  // draw items cursor
  cur_idx = (inventory.in_items) ? flash_idx : 1;
  int icol = (inventory.item_idx % 6);
  int irow = (inventory.item_idx / 6);
  gfx_draw_texrect(&rc_cursor_items[cur_idx], GFX_LAYER_FRONT, icol * 32 + INV_CUR_LEFT, irow * 16 + INV_ITEM_TOP);

  // draw items
  for (i = 0; i < inventory.num_items; ++i) {
    const int item_id = inventory.items_owned[i];
    if (item_id == 0)
      break; // probably nothing else owned
    icol = (i % 6);
    irow = (i / 6);
    gfx_draw_texrect(&hud_rc_item[item_id], GFX_LAYER_FRONT, icol * 32 + INV_CUR_LEFT, irow * 16 + INV_ITEM_TOP);
  }
}

typedef void (*menu_func_t)(void);

static const struct menu_desc {
  menu_func_t openfunc;
  menu_func_t actfunc;
  menu_func_t drawfunc;
} menu_desctab[] = {
  { menu_null,           menu_null,          menu_null           }, // NONE
  { menu_null,           menu_null,          menu_null           }, // TITLE
  { menu_null,           menu_pause_act,     menu_pause_draw     }, // PAUSE
  { menu_inventory_open, menu_inventory_act, menu_inventory_draw }, // INVENTORY
  { menu_null,           menu_null,          menu_null           }, // MAP
  { menu_null,           menu_null,          menu_null           }, // STAGESELECT
};

void menu_open(const int menu) {
  menu_id = menu;
  menu_desctab[menu].openfunc();
}

int menu_active(void) {
  return menu_id;
}

void menu_act(void) {
  menu_desctab[menu_id].actfunc();
}

void menu_draw(void) {
  menu_desctab[menu_id].drawfunc();
}
