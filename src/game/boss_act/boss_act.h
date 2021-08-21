#pragma once

#include <stdlib.h>

#include "engine/common.h"
#include "engine/math.h"
#include "engine/graphics.h"
#include "engine/sound.h"
#include "engine/input.h"

#include "game/game.h"
#include "game/npc.h"
#include "game/caret.h"
#include "game/bullet.h"
#include "game/player.h"
#include "game/stage.h"
#include "game/camera.h"

void boss_act_core(npc_t *root);
void boss_act_undead_core(npc_t *root);
void boss_act_balfrog(npc_t *root);
void boss_act_ballos(npc_t *root);
void boss_act_omega(npc_t *root);
void boss_act_ironhead(npc_t *root);
void boss_act_heavy_press(npc_t *root);
void boss_act_twins(npc_t *root);
void boss_act_monster_x(npc_t *root);
