#pragma once

#include "engine/common.h"
#include "engine/graphics.h"

#include "game/npc.h"

extern gfx_texrect_t hud_rc_digit[];
extern gfx_texrect_t hud_rc_item[];
extern gfx_texrect_t hud_rc_arms[];
extern gfx_texrect_t hud_rc_ammo[];

void hud_init(void);
void hud_update(void);
void hud_draw(void);
void hud_show_map_name(void);
void hud_init_boss_life(npc_t *npc);
void hud_clear(void);
void hud_draw_number(int val, int x, int y);
