#pragma once

#include "engine/common.h"

// also known as Triangle.h

extern s16 m_sintab[0x100];
extern s16 math_tantab[0x21];

static inline int m_sin(u8 deg) {
  return m_sintab[deg];
}

static inline int m_cos(u8 deg) {
  deg += 0x40;
  return m_sintab[deg];
}

u8 m_atan2(int x, int y);
void m_srand(const u32 seed);
int m_rand(const int min, const int max);
