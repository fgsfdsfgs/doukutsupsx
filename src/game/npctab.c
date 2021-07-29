#include "engine/common.h"
#include "engine/filesystem.h"
#include "engine/memory.h"

#include "game/npc.h"
#include "game/npc_act/npc_act.h"

npc_class_t *npc_classtab;

void npc_load_classtab(const char *tabpath) {
  fs_file_t *f = fs_fopen(tabpath, 0);
  if (!f) panic("could not open\n%s", tabpath);

  const int size = fs_fsize(f);
  const int count = size / sizeof(npc_class_t);
  ASSERT(count >= 1);

  npc_classtab = mem_alloc(size);

  // data in the table is stored column by column because fuck you
  // read all columns at once and hope there's enough memory
  u16 *p_bits = mem_alloc(size); 
  fs_fread_or_die(p_bits, size, 1, f);
  fs_fclose(f);

  // reconfigure
  u16 *p_life = &p_bits[count];
  u8 *p_surf_id = (u8 *)&p_life[count];
  u8 *p_snd_hit = (u8 *)&p_surf_id[count];
  u8 *p_snd_die = (u8 *)&p_snd_hit[count];
  u8 *p_size = (u8 *)&p_snd_die[count];
  s32 *p_exp = (s32 *)&p_size[count];
  s32 *p_damage = (s32 *)&p_exp[count];
  npc_tab_hitbox_t *p_hit = (npc_tab_hitbox_t *)&p_damage[count];
  npc_tab_hitbox_t *p_view = (npc_tab_hitbox_t *)&p_hit[count];
  for (int i = 0; i < count; ++i) {
    npc_class_t *c = &npc_classtab[i];
    c->bits = p_bits[i];
    c->life = p_life[i];
    c->surf_id = p_surf_id[i];
    c->snd_hit = p_snd_hit[i];
    c->snd_die = p_snd_die[i];
    c->size = p_size[i];
    c->exp = p_exp[i];
    c->damage = p_damage[i];
    c->hit = p_hit[i];
    c->view = p_view[i];
  }

  mem_free(p_bits);
}

npc_func_t npc_functab[NPC_MAX_ACTFUNC] = {
  npc_act_000,
  npc_act_001,
  npc_act_002,
  npc_act_003,
  npc_act_004,
  npc_act_005,
  npc_act_006,
  npc_act_007,
  npc_act_008,
  npc_act_009,
  npc_act_010,
  npc_act_011,
  npc_act_012,
  npc_act_013,
  npc_act_014,
  npc_act_015,
  npc_act_016,
  npc_act_017,
  npc_act_018,
  npc_act_019,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
};
