#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>

// generates the tables from Triangle.cpp

int main(int argc, char **argv) {
  // sine
  for (int i = 0; i < 0x100; ++i) {
    if (i) putchar((i & 7) ? ' ' : '\n');
    const int32_t s = (int32_t)(sin(i * 6.2831998 / 256.0) * 512.0);
    printf("%4d,", s);
  }

  puts("\n");

  // tangent
  for (int i = 0; i < 0x21; ++i) {
    if (i) putchar((i & 7) ? ' ' : '\n');
    const float a = (float)(i * 6.2831855f / 256.0f);
    const float b = (float)sin(a) / (float)cos(a);
    const int16_t t = (int16_t)(b * 8192.0f);
    printf("%4d,", t);
  }

  return 0;
}