// Adapted from RAD Debugger, Copyright (c) Epic Games Tools.
#pragma once

#include "base.h"

constexpr usz ARENA_HEADER_SIZE = 128;

// Blocks remain owned by their arena for the module's lifetime. Wasm cannot
// release pages, so clearing an arena retains excess blocks for reuse.
struct Arena {
    Arena* prev;
    Arena* current;
    Arena* free_blocks;
    usz block_size;
    usz capacity;
    usz pos;
    ulong base_pos;
};
static_assert(sizeof(Arena) <= ARENA_HEADER_SIZE);

struct Temp {
    Arena* arena;
    ulong pos;
};

inline Arena* arena_alloc(usz block_size) {
    if (block_size <= ARENA_HEADER_SIZE) TRAP();
    Arena* arena = new (os_reserve(block_size)) Arena{};
    arena->current = arena;
    arena->block_size = block_size;
    arena->capacity = block_size;
    arena->pos = ARENA_HEADER_SIZE;
    return arena;
}

inline void* arena_push(Arena* arena, usz size, usz align, bool zero) {
    if (align == 0 || (align & (align - 1)) != 0) TRAP();
    Arena* current = arena->current;
    ulong address = (ulong)(uptr)current + current->pos;
    ulong aligned = (address + align - 1) & ~((ulong)align - 1);
    ulong position = aligned - (uptr)current;

    if (position + size > current->capacity) {
        Arena** link = &arena->free_blocks;
        Arena* block = nullptr;
        for (; *link; link = &(*link)->prev) {
            Arena* candidate = *link;
            address = (ulong)(uptr)candidate + ARENA_HEADER_SIZE;
            aligned = (address + align - 1) & ~((ulong)align - 1);
            position = aligned - (uptr)candidate;
            if (position + size <= candidate->capacity) {
                block = candidate;
                *link = candidate->prev;
                break;
            }
        }
        if (!block) {
            // Include worst-case padding because backing blocks are only
            // guaranteed 16-byte alignment, while callers may require more.
            ulong required = (ulong)ARENA_HEADER_SIZE + align - 1 + size;
            if (required > (usz)-1) TRAP();
            usz capacity = arena->block_size;
            if (required > capacity) capacity = (usz)required;
            block = arena_alloc(capacity);
        }
        block->base_pos = current->base_pos + current->capacity;
        block->prev = current;
        arena->current = current = block;
        address = (ulong)(uptr)current + ARENA_HEADER_SIZE;
        aligned = (address + align - 1) & ~((ulong)align - 1);
        position = aligned - (uptr)current;
    }

    if (aligned > (usz)-1 || position + size > current->capacity) TRAP();
    current->pos = (usz)(position + size);
    void* result = (char*)current + (usz)position;
    if (zero) MEMSET(result, 0, size);
    return result;
}

inline ulong arena_pos(Arena* arena) {
    return arena->current->base_pos + arena->current->pos;
}

// Positions must come from this arena and be restored in lifetime order.
// Restoring a position invalidates every allocation made after it.
inline void arena_pop_to(Arena* arena, ulong pos) {
    if (pos < ARENA_HEADER_SIZE || pos > arena_pos(arena)) TRAP();
    Arena* target = arena->current;
    while (target->prev && pos <= target->base_pos) target = target->prev;
    if (pos < target->base_pos + ARENA_HEADER_SIZE ||
        pos > target->base_pos + target->pos) TRAP();

    while (arena->current != target) {
        Arena* block = arena->current;
        arena->current = block->prev;
#ifdef DEBUG
        MEMSET((char*)block + ARENA_HEADER_SIZE, 0xCD, block->pos - ARENA_HEADER_SIZE);
#endif
        block->pos = ARENA_HEADER_SIZE;
        block->prev = arena->free_blocks;
        arena->free_blocks = block;
    }
    usz position = (usz)(pos - target->base_pos);
#ifdef DEBUG
    MEMSET((char*)target + position, 0xCD, target->pos - position);
#endif
    target->pos = position;
}

inline void arena_clear(Arena* arena) {
    arena_pop_to(arena, ARENA_HEADER_SIZE);
}

inline Temp temp_begin(Arena* arena) {
    return {arena, arena_pos(arena)};
}

inline void temp_end(Temp temp) {
    arena_pop_to(temp.arena, temp.pos);
}

template <typename T>
inline T* push_array(Arena* arena, usz count) {
    if (count > (usz)-1 / sizeof(T)) TRAP();
    return (T*)arena_push(arena, sizeof(T) * count, alignof(T), true);
}

template <typename T>
inline T* push_array_nz(Arena* arena, usz count) {
    if (count > (usz)-1 / sizeof(T)) TRAP();
    return (T*)arena_push(arena, sizeof(T) * count, alignof(T), false);
}
