#pragma once

#include "engine/common.h"

enum menu_type {
  MENU_NONE = 0,
  MENU_TITLE,
  MENU_PAUSE,
  MENU_INVENTORY,
  MENU_MAP,
  MENU_STAGESELECT,
};

void menu_init(void);
int menu_active(void);
void menu_act(void);
void menu_draw(void);
void menu_open(const int type);
