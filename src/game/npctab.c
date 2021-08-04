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
  u8 *p_snd_die = (u8 *)&p_surf_id[count];
  u8 *p_snd_hit = (u8 *)&p_snd_die[count];
  u8 *p_size = (u8 *)&p_snd_hit[count];
  s32 *p_exp = (s32 *)&p_size[count];
  s32 *p_damage = (s32 *)&p_exp[count];
  npc_tab_hitbox_t *p_hit = (npc_tab_hitbox_t *)&p_damage[count];
  npc_tab_hitbox_t *p_view = (npc_tab_hitbox_t *)&p_hit[count];
  for (int i = 0; i < count; ++i) {
    npc_class_t *c = &npc_classtab[i];
    c->bits = p_bits[i];
    c->life = p_life[i];
    c->surf_id = p_surf_id[i];
    c->snd_die = p_snd_die[i];
    c->snd_hit = p_snd_hit[i];
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
  npc_act_020,
  npc_act_021,
  npc_act_022,
  npc_act_023,
  npc_act_024,
  npc_act_025,
  npc_act_026,
  npc_act_027,
  npc_act_028,
  npc_act_029,
  npc_act_030,
  npc_act_031,
  npc_act_032,
  npc_act_033,
  npc_act_034,
  npc_act_035,
  npc_act_036,
  npc_act_037,
  npc_act_038,
  npc_act_039,
  npc_act_040,
  npc_act_041,
  npc_act_042,
  npc_act_043,
  npc_act_044,
  npc_act_045,
  npc_act_046,
  npc_act_047,
  npc_act_048,
  npc_act_049,
  npc_act_050,
  npc_act_051,
  npc_act_052,
  npc_act_053,
  npc_act_054,
  npc_act_055,
  npc_act_056,
  npc_act_057,
  npc_act_058,
  npc_act_059,
  npc_act_060,
  npc_act_061,
  npc_act_062,
  npc_act_063,
  npc_act_064,
  npc_act_065,
  npc_act_066,
  npc_act_067,
  npc_act_068,
  npc_act_069,
  npc_act_070,
  npc_act_071,
  npc_act_072,
  npc_act_073,
  npc_act_074,
  npc_act_075,
  npc_act_076,
  npc_act_077,
  npc_act_078,
  npc_act_079,
  npc_act_080,
  npc_act_081,
  npc_act_082,
  npc_act_083,
  npc_act_084,
  npc_act_085,
  npc_act_086,
  npc_act_087,
  npc_act_088,
  npc_act_089,
  npc_act_090,
  npc_act_091,
  npc_act_092,
  npc_act_093,
  npc_act_094,
  npc_act_095,
  npc_act_096,
  npc_act_097,
  npc_act_098,
  npc_act_099,
  npc_act_null, // 100
  npc_act_null, // 101
  npc_act_null, // 102
  npc_act_null, // 103
  npc_act_null, // 104
  npc_act_null, // 105
  npc_act_null, // 106
  npc_act_null, // 107
  npc_act_null, // 108
  npc_act_null, // 109
  npc_act_null, // 110
  npc_act_null, // 111
  npc_act_null, // 112
  npc_act_null, // 113
  npc_act_null, // 114
  npc_act_null, // 115
  npc_act_null, // 116
  npc_act_null, // 117
  npc_act_null, // 118
  npc_act_null, // 119
  npc_act_null, // 120
  npc_act_null, // 121
  npc_act_null, // 122
  npc_act_null, // 123
  npc_act_null, // 124
  npc_act_null, // 125
  npc_act_null, // 126
  npc_act_null, // 127
  npc_act_null, // 128
  npc_act_null, // 129
  npc_act_null, // 130
  npc_act_null, // 131
  npc_act_null, // 132
  npc_act_null, // 133
  npc_act_null, // 134
  npc_act_null, // 135
  npc_act_null, // 136
  npc_act_null, // 137
  npc_act_null, // 138
  npc_act_null, // 139
  npc_act_140,
  npc_act_141,
  npc_act_142,
  npc_act_143,
  npc_act_144,
  npc_act_145,
  npc_act_146,
  npc_act_147,
  npc_act_148,
  npc_act_149,
  npc_act_150,
  npc_act_151,
  npc_act_152,
  npc_act_153,
  npc_act_154,
  npc_act_155,
  npc_act_156,
  npc_act_157,
  npc_act_158,
  npc_act_159,
  npc_act_null, // 160
  npc_act_null, // 161
  npc_act_null, // 162
  npc_act_null, // 163
  npc_act_null, // 164
  npc_act_null, // 165
  npc_act_null, // 166
  npc_act_null, // 167
  npc_act_null, // 168
  npc_act_null, // 169
  npc_act_null, // 170
  npc_act_null, // 171
  npc_act_null, // 172
  npc_act_null, // 173
  npc_act_null, // 174
  npc_act_null, // 175
  npc_act_null, // 176
  npc_act_null, // 177
  npc_act_null, // 178
  npc_act_null, // 179
  npc_act_null, // 180
  npc_act_null, // 181
  npc_act_null, // 182
  npc_act_null, // 183
  npc_act_null, // 184
  npc_act_null, // 185
  npc_act_null, // 186
  npc_act_null, // 187
  npc_act_null, // 188
  npc_act_null, // 189
  npc_act_null, // 190
  npc_act_null, // 191
  npc_act_null, // 192
  npc_act_null, // 193
  npc_act_null, // 194
  npc_act_null, // 195
  npc_act_null, // 196
  npc_act_null, // 197
  npc_act_null, // 198
  npc_act_null, // 199
  npc_act_200,
  npc_act_201,
  npc_act_202,
  npc_act_203,
  npc_act_204,
  npc_act_205,
  npc_act_206,
  npc_act_207,
  npc_act_208,
  npc_act_209,
  npc_act_210,
  npc_act_211,
  npc_act_212,
  npc_act_213,
  npc_act_214,
  npc_act_215,
  npc_act_216,
  npc_act_217,
  npc_act_218,
  npc_act_219,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_340,
  npc_act_341,
  npc_act_342,
  npc_act_343,
  npc_act_344,
  npc_act_345,
  npc_act_346,
  npc_act_347,
  npc_act_348,
  npc_act_349,
  npc_act_350,
  npc_act_351,
  npc_act_352,
  npc_act_353,
  npc_act_354,
  npc_act_355,
  npc_act_356,
  npc_act_357,
  npc_act_358,
  npc_act_359,
  npc_act_360,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
  npc_act_null,
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
