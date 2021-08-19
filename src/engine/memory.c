#include "engine/memory.h"

#include <malloc.h>
#include <stdio.h>
#include <string.h>

#include "engine/common.h"

#define RAM_END 0x80200000

#define MEM_ALIGNMENT 8
#define MEM_MAX_ALLOCS 128

static u8 *mem_base;
static u8 *mem_ptr;
static u8 *mem_lastptr;
static int mem_size;
static int mem_left;

static u32 mem_allocs[MEM_MAX_ALLOCS];
static u32 mem_numallocs;

static struct {
  u8 *mem_ptr;
  u8 *mem_lastptr;
  s32 mem_left;
  u32 mem_numallocs;
} mem_mark[MEM_MARK_COUNT];

// this should be defined somewhere
extern u32 __bss_end;

void mem_init(void) {
  mem_base = (u8 *)&__bss_end + MALLOC_HEAP_SIZE;
  mem_base = (u8 *)ALIGN((u32)mem_base, MEM_ALIGNMENT);
  mem_size = mem_left = (u8 *)RAM_END - mem_base - STACK_SIZE;
  mem_ptr = mem_lastptr = mem_base;
  mem_numallocs = 0;

  printf("mem_init: mem_base=%08x mem_size=%u\n", (u32)mem_base, mem_size);
}

void *mem_alloc(const u32 size) {
  const u32 asize = ALIGN(size, MEM_ALIGNMENT);
  if (size == 0)
    PANIC("mem_alloc: size == 0");
  if (mem_left < asize)
    PANIC("mem_alloc: failed to alloc %u bytes", size);
  if (mem_numallocs == MEM_MAX_ALLOCS)
    PANIC("mem_alloc: MAX_ALLOCS reached");
  mem_lastptr = mem_ptr;
  mem_ptr += asize;
  mem_left -= asize;
  mem_allocs[mem_numallocs++] = asize;
  return mem_lastptr;
}

void *mem_zeroalloc(const u32 size) {
  void *buf = mem_alloc(size);
  memset(buf, 0, size);
  return buf;
}

void *mem_realloc(void *ptr, const u32 newsize) {
  if (!ptr)
    return mem_alloc(newsize); // just like realloc()
  const u32 anewsize = ALIGN(newsize, MEM_ALIGNMENT);
  const u32 oldsize = mem_allocs[mem_numallocs - 1];
  if (ptr != mem_lastptr || !mem_numallocs)
    PANIC("mem_realloc: this is a stack allocator you dolt");
  if (mem_left < anewsize)
    PANIC("mem_realloc: failed to realloc %p from %u to %u bytes", oldsize, anewsize);
  mem_left += newsize - oldsize;
  mem_ptr = mem_lastptr + newsize;
  mem_allocs[mem_numallocs - 1] = newsize;
  return mem_lastptr;
}

void mem_free(void *ptr) {
  if (!ptr)
    return;  // NULL free is a no-op
  if (ptr != mem_lastptr)
    PANIC("mem_free: this is a stack allocator you dolt");
  if (mem_numallocs == 0)
    PANIC("mem_free: nothing to free");
  const u32 size = mem_allocs[--mem_numallocs];
  mem_left += size;
  mem_ptr = mem_lastptr;
  if (mem_numallocs)
    mem_lastptr -= mem_allocs[mem_numallocs - 1];
  else
    ASSERT(mem_lastptr == mem_base);
}

void mem_set_mark(const int m) {
  mem_mark[m].mem_numallocs = mem_numallocs;
  mem_mark[m].mem_left = mem_left;
  mem_mark[m].mem_lastptr = mem_lastptr;
  mem_mark[m].mem_ptr = mem_ptr;
  printf("mem_set_mark: mark %d set at %p/%p, %d left\n", m, mem_lastptr, mem_ptr, mem_left);
}

void mem_free_to_mark(const int m) {
  mem_numallocs = mem_mark[m].mem_numallocs;
  mem_left = mem_mark[m].mem_left;
  mem_lastptr = mem_mark[m].mem_lastptr;
  mem_ptr = mem_mark[m].mem_ptr;
  printf("mem_free_to_mark: reset to mark %d: %p/%p, %d left\n", m, mem_lastptr, mem_ptr, mem_left);
}

u32 mem_get_free_space(void) {
  return mem_left;
}
