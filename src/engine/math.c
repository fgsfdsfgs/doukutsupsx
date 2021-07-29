#include "engine/common.h"
#include "engine/math.h"

// also known as Triangle.cpp
// these can be generated with trigcalc.exe

s16 m_sintab[0x100] = {
     0,   12,   25,   37,   50,   62,   75,   87,
    99,  112,  124,  136,  148,  160,  172,  184,
   195,  207,  218,  230,  241,  252,  263,  273,
   284,  294,  304,  314,  324,  334,  343,  353,
   362,  370,  379,  387,  395,  403,  411,  418,
   425,  432,  439,  445,  451,  457,  462,  468,
   473,  477,  482,  486,  489,  493,  496,  499,
   502,  504,  506,  508,  509,  510,  511,  511,
   511,  511,  511,  510,  509,  508,  506,  504,
   502,  499,  496,  493,  489,  486,  482,  477,
   473,  468,  462,  457,  451,  445,  439,  432,
   425,  418,  411,  403,  395,  387,  379,  370,
   362,  353,  343,  334,  324,  314,  304,  294,
   284,  273,  263,  252,  241,  230,  218,  207,
   195,  184,  172,  160,  148,  136,  124,  112,
    99,   87,   75,   62,   50,   37,   25,   12,
     0,  -12,  -25,  -37,  -50,  -62,  -75,  -87,
   -99, -112, -124, -136, -148, -160, -172, -184,
  -195, -207, -218, -230, -241, -252, -263, -273,
  -284, -294, -305, -315, -324, -334, -343, -353,
  -362, -370, -379, -387, -395, -403, -411, -418,
  -425, -432, -439, -445, -451, -457, -462, -468,
  -473, -477, -482, -486, -489, -493, -496, -499,
  -502, -504, -506, -508, -509, -510, -511, -511,
  -511, -511, -511, -510, -509, -508, -506, -504,
  -502, -499, -496, -493, -489, -486, -482, -477,
  -473, -468, -462, -457, -451, -445, -439, -432,
  -425, -418, -411, -403, -395, -387, -379, -370,
  -362, -353, -343, -334, -324, -314, -304, -294,
  -284, -273, -263, -252, -241, -230, -218, -207,
  -195, -184, -172, -160, -148, -136, -124, -112,
};

s16 math_tantab[0x21] = {
     0,  201,  402,  604,  806, 1010, 1215, 1421,
  1629, 1839, 2051, 2267, 2485, 2706, 2931, 3160,
  3393, 3631, 3874, 4123, 4378, 4640, 4910, 5187,
  5473, 5769, 6075, 6393, 6723, 7066, 7424, 7799,
  8192,
};

// preserved as is
u8 m_atan2(int x, int y) {
  register s16 k;
  register u8 a;

  x = -x;
  y = -y;

  a = 0;

  if (x > 0) {
    if (y > 0) {
      if (x > y) {
        k = (y * 0x2000) / x;
        while (k > math_tantab[a]) ++a;
      } else {
        k = (x * 0x2000) / y;
        while (k > math_tantab[a]) ++a;
        a = 0x40 - a;
      }
    } else {
      if (x > -y) {
        k = (-y * 0x2000) / x;
        while (k > math_tantab[a]) ++a;
        a = 0x100 - a;
      } else {
        k = (x * 0x2000) / -y;
        while (k > math_tantab[a]) ++a;
        a = 0x100 - 0x40 + a;
      }
    }
  } else {
    if (y > 0) {
      if (-x > y) {
        k = (y * 0x2000) / -x;
        while (k > math_tantab[a]) ++a;
        a = 0x80 - a;
      } else {
        k = (-x * 0x2000) / y;
        while (k > math_tantab[a]) ++a;
        a = 0x40 + a;
      }
    } else {
      if (-x > -y) {
        k = (-y * 0x2000) / -x;
        while (k > math_tantab[a]) ++a;
        a = 0x80 + a;
      } else {
        k = (-x * 0x2000) / -y;
        while (k > math_tantab[a]) ++a;
        a = 0x100 - 0x40 - a;
      }
    }
  }

  return a;
}

static u32 rand_seed = 0;

void m_srand(const u32 seed) {
  rand_seed = seed;
}

int m_rand(const int min, const int max) {
  register int r1, r2, range;
  range = max - min + 1;
  r1 = (214013 * rand_seed + 2531011);
  r1 = (r1 >> 16) & 0x7FFF;
  r2 = (214013 * r1 + 2531011);
  r2 = (r1 >> 16) & 0x7FFF;
  rand_seed = r2;
  return r2 % range;
}
