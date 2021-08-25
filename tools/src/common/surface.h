#pragma once

#include <stdint.h>

#include "common.h"

enum surface_id {
  SURFACE_ID_TITLE = 0,
  SURFACE_ID_PIXEL = 1,
  SURFACE_ID_LEVEL_TILESET = 2,
  SURFACE_ID_FADE = 6,
  SURFACE_ID_ITEM_IMAGE = 8,
  SURFACE_ID_MAP = 9,
  SURFACE_ID_SCREEN_GRAB = 10,
  SURFACE_ID_ARMS = 11,
  SURFACE_ID_ARMS_IMAGE = 12,
  SURFACE_ID_ROOM_NAME = 13,
  SURFACE_ID_STAGE_ITEM = 14,
  SURFACE_ID_LOADING = 15,
  SURFACE_ID_MY_CHAR = 16,
  SURFACE_ID_BULLET = 17,
  SURFACE_ID_CARET = 19,
  SURFACE_ID_NPC_SYM = 20,
  SURFACE_ID_LEVEL_SPRITESET_1 = 21,
  SURFACE_ID_LEVEL_SPRITESET_2 = 22,
  SURFACE_ID_NPC_REGU = 23,
  SURFACE_ID_TEXT_BOX = 26,
  SURFACE_ID_FACE = 27,
  SURFACE_ID_LEVEL_BACKGROUND = 28,
  SURFACE_ID_VALUE_VIEW = 29,
  SURFACE_ID_TEXT_LINE1 = 30,
  SURFACE_ID_TEXT_LINE2 = 31,
  SURFACE_ID_TEXT_LINE3 = 32,
  SURFACE_ID_TEXT_LINE4 = 33,
  SURFACE_ID_TEXT_LINE5 = 34,
  SURFACE_ID_CREDIT_CAST = 35,
  SURFACE_ID_CREDITS_IMAGE = 36,
  SURFACE_ID_CASTS = 37,
  SURFACE_ID_FONT1 = 38,
  SURFACE_ID_FONT2 = 39,
  SURFACE_ID_MAX = 40
};

typedef struct surflist {
  char fname[2048];
  uint8_t id;
  struct bitmap img;
} surf_list_t;

int read_surflist(surf_list_t *list, FILE *f);
int read_bmp(struct bitmap *bmp, FILE *f);
int convert_surface(const uint32_t id, const struct bitmap *img, const bool force_align);
