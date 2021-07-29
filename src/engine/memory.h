#pragma once

#include "engine/common.h"

// we're rolling our own stack allocator, but we'll leave some space for malloc
// in case libc uses it or something

#define MALLOC_HEAP_SIZE 16384
#define STACK_SIZE       16384

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
void mem_set_mark(void);

// frees everything allocated after the mark set by last MemSetMark call
void mem_free_to_mark(void);

// returns number of allocatable bytes
u32 mem_get_free_space(void);
