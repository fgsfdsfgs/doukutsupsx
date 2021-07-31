#pragma once

#include "engine/common.h"
#include "engine/graphics.h"

#include "game/stage.h"
#include "game/npctab.h"

#define NPC_MAX 512
#define NPC_MAX_FLAGS 1024
#define NPC_STARTIDX_DYNAMIC 256
#define NPC_STARTIDX_EVENT 170
#define NPC_MAIN_TABLE "\\MAIN\\NPC.TBL;1"

enum npc_cond_flags {
  NPCCOND_KILLED      = 0x08, // Hit by a bullet and died
  NPCCOND_DAMAGE_BOSS = 0x10, // (boss npc exclusive) When set, damage the main boss
  NPCCOND_ALIVE       = 0x80, // Whether the NPC is alive or not
};

// Be careful when changing these: they're baked into the 'npc.tbl' file
enum npc_bits {
  NPC_SOLID_SOFT               = 1 << 0,  // Pushes Quote out
  NPC_IGNORE_TILE_44           = 1 << 1,  // Ignores tile 44, which normally blocks NPCs
  NPC_INVULNERABLE             = 1 << 2,  // Can't be hurt
  NPC_IGNORE_SOLIDITY          = 1 << 3,  // Doesn't collide with anything
  NPC_BOUNCY                   = 1 << 4,  // Quote bounces on top of NPC
  NPC_SHOOTABLE                = 1 << 5,  // Can be shot
  NPC_SOLID_HARD               = 1 << 6,  // Essentially acts as level tiles
  NPC_REAR_AND_TOP_DONT_HURT   = 1 << 7,  // Rear and top don't hurt when touched
  NPC_EVENT_WHEN_TOUCHED       = 1 << 8,  // Run event when touched
  NPC_EVENT_WHEN_KILLED        = 1 << 9,  // Run event when killed
  NPC_APPEAR_WHEN_FLAG_SET     = 1 << 11, // Only appear when flag is set
  NPC_SPAWN_IN_OTHER_DIRECTION = 1 << 12, // Spawn facing to the right (or however the NPC interprets the direction)
  NPC_INTERACTABLE             = 1 << 13, // Run event when interacted with
  NPC_HIDE_WHEN_FLAG_SET       = 1 << 14, // Hide when flag is set
  NPC_SHOW_DAMAGE              = 1 << 15, // Show the number of damage taken when harmed
};

enum npc_classnames {
  NPC_NULL = 0,
  NPC_EXP = 1,
  NPC_ENEMY_BEHEMOTH = 2,
  NPC_DAMAGE_TEXT_HOLDER = 3,
  NPC_SMOKE = 4,
  NPC_ENEMY_FROG = 104,
  NPC_SPEECH_BALLOON_HEY_LOW = 105,
  NPC_SPEECH_BALLOON_HEY_HIGH = 106,
  NPC_MALCO_UNDAMAGED = 107,
  NPC_PROJECTILE_BALFROG_SPITBALL = 108,
  NPC_MALCO_DAMAGED = 109,
  NPC_ENEMY_PUCHI = 110,
  NPC_KINGS_SWORD = 145
};

typedef struct npc {
  gfx_texrect_t texrect;
  rect_t rect_delta;
  hitbox_t hit;
  hitbox_t view;

  const rect_t *rect;
  const rect_t *rect_prev;
  const npc_class_t *info;
  struct npc *parent;

  u32 flags;
  s32 x;
  s32 y;
  s32 xvel;
  s32 yvel;
  s32 xvel2;
  s32 yvel2;
  s32 tgt_x;
  s32 tgt_y;

  u16 class_num;
  u16 bits;
  s16 life;
  s16 damage;
  s16 damage_view;
  s16 event_flag;
  s16 event_num;
  s16 anim;
  s16 anim_wait;
  s16 act;
  s16 act_wait;
  s16 count1;
  s16 count2;

  u8 cond;
  s8 dir;
  u8 shock;
  u8 snd_die;
  u8 snd_hit;
  u8 exp;
  u8 surf;
} npc_t;

extern u8 npc_flags[NPC_MAX_FLAGS];
extern npc_t npc_list[NPC_MAX];
extern int npc_list_max;

void npc_init(const char *tabpath);
void npc_parse_event_list(const stage_event_t *ev, const int numev);
npc_t *npc_spawn(int class_num, int x, int y, int xv, int yv, int dir, npc_t *parent, int startidx);
void npc_delete(npc_t *npc);
void npc_delete_by_class(const int class_num, const int spawn_smoke);
void npc_show_death_damage(npc_t *npc);
void npc_kill(npc_t *npc, bool show_damage);
void npc_spawn_death_fx(int x, int y, int w, int num, int up);
void npc_spawn_exp(int x, int y, int exp);
npc_t *npc_spawn_life(int x, int y, int val);
npc_t *npc_spawn_ammo(int x, int y, int val);

void npc_draw(int cam_x, int cam_y);
void npc_act(void);

static inline u8 npc_get_flag(const u32 i) {
  return npc_flags[i >> 3] & (1 << (i & 7));
}

static inline void npc_set_flag(const u32 i) {
  npc_flags[i >> 3] |= (1 << (i & 7));
}

static inline void npc_clear_flag(const u32 i) {
  npc_flags[i >> 3] &= ~(1 << (i & 7));
}

