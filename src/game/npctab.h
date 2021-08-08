#pragma once

#include "engine/common.h"

#define NPC_MAX_ACTFUNC 384

#pragma pack(push, 1)

typedef struct {
  u8 front;
  u8 top;
  u8 back;
  u8 bottom;
} npc_tab_hitbox_t;

typedef struct {
  u16 bits;
  u16 life;
  u8 surf_id;
  u8 snd_hit;
  u8 snd_die;
  u8 size;
  s32 exp;
  s32 damage;
  npc_tab_hitbox_t hit;
  npc_tab_hitbox_t view;
} npc_class_t;

#pragma pack(pop)

struct npc;
typedef void (*npc_func_t)(struct npc *);

extern npc_class_t *npc_classtab;
extern npc_func_t npc_functab[];
extern npc_func_t npc_boss_functab[];

void npc_load_classtab(const char *tabpath);
