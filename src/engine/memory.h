#pragma once

#include "engine/common.h"

// we're rolling our own stack allocator, but we'll leave some space for malloc
// in case libc uses it or something

#define MALLOC_HEAP_SIZE 16384
#define STACK_SIZE       16384

enum mem_mark {
  MEM_MARK_LO,
  MEM_MARK_HI,
  MEM_MARK_COUNT,
};

void mem_init(void);

// simple stack type allocator
void *mem_alloc(const u32 size);
void mem_free(void *ptr);

// same as `mem_alloc`, but zero-fills the allocated area
void *mem_zeroalloc(const u32 size);

// if `ptr` points to the top allocation, extends/shrinks it to `newsize`
// if `ptr` is NULL, calls `mem_alloc(newsize)`
void *mem_realloc(void *ptr, const u32 newsize);

// marks current alloc position
// mark can be MEM_MARK_LO or MEM_MARK_HI
void mem_set_mark(const int mark);

// frees everything allocated after the mark set by last MemSetMark call with the same mark
void mem_free_to_mark(const int mark);

// returns number of allocatable bytes
u32 mem_get_free_space(void);
