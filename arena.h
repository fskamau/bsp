/*
  simple singl header arena pool allocator

*/  
  
#ifndef ARENA_H
#define ARENA_H

#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>

typedef struct {
    uint8_t *buffer;
    size_t capacity;
    size_t offset;
} Arena;


Arena* arena_create(size_t size);
void* arena_alloc(Arena *arena, size_t size);
void* arena_alloc_or_die(Arena *arena, size_t size);
void arena_reset(Arena *arena);
void arena_free(Arena *arena);

#ifdef ARENA_IMPLEMENTATION

Arena* arena_create(size_t size)
{
    Arena *arena = (Arena*)malloc(sizeof(Arena));
    if (!arena) return NULL;
    
    arena->buffer = (uint8_t*)malloc(size);
    if (!arena->buffer) {
        free(arena);
        return NULL;
    }
    
    arena->capacity = size;
    arena->offset = 0;
    return arena;
}


void* arena_alloc(Arena *arena, size_t size)
{
  assert(arena&&"arena is NULL");
  size_t padding = (8 - (arena->offset % 8)) % 8;
  size_t total_size = size + padding;

  if (arena->offset + total_size > arena->capacity) {
    return NULL;
  }

  void *ptr = arena->buffer + arena->offset + padding;
  arena->offset += total_size;
  return ptr;
}

void* arena_alloc_or_die(Arena *arena, size_t size)
{
    void *ptr = arena_alloc(arena, size);
    if (ptr == NULL) {
      fprintf(stderr, "Error: [Arena] out of memory (%zu bytes requested, have %zu)\n", size,arena->capacity-arena->offset);
        exit(1); 
    }
    return ptr;
}

void arena_reset(Arena *arena)
{
    assert(arena&&"arena is NULL");
    arena->offset = 0;
}

void arena_free(Arena *arena)
{
    if (arena) {
        free(arena->buffer);
        free(arena);
    }
}

#endif // ARENA_IMPLEMENTATION
#endif // ARENA_H
