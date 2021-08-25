#pragma once

#include "engine/common.h"

enum menu_type {
  MENU_NONE = 0,
  MENU_TITLE,
  MENU_PAUSE,
  MENU_INVENTORY,
  MENU_MAP,
  MENU_STAGESELECT,
  MENU_SAVE,
  MENU_LOAD,
  MENU_OPTIONS,
  // HACK: this behaves similar enough to a menu
  MENU_FALLING_ISLAND_0,
  MENU_FALLING_ISLAND_1,
};

void menu_init(void);
int menu_active(void);
void menu_act(void);
void menu_draw(void);
void menu_open(const int type);

bool menu_uses_tsc(void);
