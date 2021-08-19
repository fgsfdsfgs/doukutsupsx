#pragma once

#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>

// some common constants and macros

#define MAX_ERROR 512

#ifndef NULL
#define NULL ((void *)0)
#endif

#define VID_WIDTH 320
#define VID_HEIGHT 240

#define TILE_SIZE 16
#define TILE_SHIFT 4

#define CAM_WIDTH (VID_WIDTH / TILE_SIZE)
#define CAM_HEIGHT (VID_HEIGHT / TILE_SIZE)

#define PSX_SCRATCH ((void *)0x1F800000)

#define ALIGN(x, align) (((x) + ((align) - 1)) & ~((align) - 1))
#define ASSERT(x) do_assert((int)(x), #x, __FILE__, __LINE__)

// game operates on 1.22.9 fixed point numbers

#define FIX_SCALE 0x200
#define FIX_SHIFT 9
#define TO_FIX(x) ((x) * FIX_SCALE)
#define TO_INT(x) ((x) / FIX_SCALE)

// some common types

typedef unsigned  char  u8;
typedef   signed  char  s8;
typedef unsigned short u16;
typedef   signed short s16;
typedef unsigned   int u32;
typedef   signed   int s32;

typedef int bool;

#ifndef FALSE
#define FALSE 0
#define TRUE 1
#endif

// RECT-compatible rect type
typedef union {
  struct {
    short x, y;
    short w, h;
  };
  struct {
    short left, top;
    short right, bottom;
  };
  short vec[4];
} rect_t;

//OTHER_RECT in CSE2, used for bounding boxes
typedef struct {
  int front;
  int top;
  int back;
  int bottom;
} hitbox_t;

// utilities

// since psyq lacks vs(n)printf, we'll have to replace PANIC() with a shitty macro
extern char error_msg[MAX_ERROR];

#define PANIC(...) \
  do { sprintf(error_msg, __VA_ARGS__); do_panic(); } while (0)

void do_panic(void) __attribute__((noreturn));

static inline void do_assert(const int expr, const char *strexpr, const char *file, const int line) {
  if (!expr)
    PANIC("ASSERTION FAILED: %s:%d:\n%s", file, line, strexpr);
}
