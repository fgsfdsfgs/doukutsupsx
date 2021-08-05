#pragma once

#include "engine/common.h"
#include "engine/graphics.h"

extern gfx_texrect_t hud_rc_digit[];

void hud_init(void);
void hud_draw(void);
void hud_show_map_name(void);
