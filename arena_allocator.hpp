#pragma once

#include <assert.h>
#include <stdlib.h>
#include <string.h>

// Linear arena.

struct Arena {
    unsigned char* base_pointer = nullptr;
    size_t offset = 0;
    size_t prev_offset = 0;
    size_t capacity = 0;
};

inline void arena_init(Arena* arena, unsigned char* backbuffer, size_t buf_length) {
    assert(arena->base_pointer == nullptr && "Arena was allocated already. Base pointer is not null.");

    arena->base_pointer  = backbuffer;
    arena->offset        = 0;
    arena->prev_offset   = 0;
    arena->capacity      = buf_length;
}

inline size_t align_forward(size_t s, size_t alignment) {
    auto size     = s;
    auto reminder = size % alignment;

    if (reminder > 0) {
        size += alignment - reminder;
    }

    return size;
}

inline void* arena_alloc(Arena* arena, size_t size) {
    auto pos = align_forward(arena->offset, 16);
    assert((pos + size) <= arena->capacity && "New allocation is outreaching Arena capacity.");

    arena->prev_offset = pos;
    arena->offset = pos + size;

    return arena->base_pointer + arena->prev_offset;
}

template <typename T>
T* arena_push(Arena* arena, T data) {
    auto dest = arena_alloc(arena, sizeof(T));

    memcpy(dest, &data, sizeof(T));
    return (T*) (arena->base_pointer + arena->prev_offset);
}

inline void arena_reset(Arena* arena) {
    arena->offset = 0;
    arena->prev_offset = 0;
}

struct Temp_Arena {
    Arena* arena;
    size_t offset;
    size_t prev_offset;
};

inline Temp_Arena temp_begin(Arena* arena) {
    return {
        .arena = arena,
        .offset = arena->offset,
        .prev_offset = arena->prev_offset,
    };
}

inline void temp_end(Temp_Arena temp) {
    temp.arena->offset = temp.offset;
    temp.arena->prev_offset = temp.prev_offset;
}
