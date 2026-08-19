#include <assert.h>
#include <stdlib.h>

// Linear arena.

struct Arena {
    unsigned char* base_pointer = NULL;
    size_t offset = 0;
    size_t prev_offset = 0;
    size_t capacity = 0;
};

void arena_init(Arena* arena, unsigned char* backbuffer, size_t buf_length) {
    assert(arena->base_pointer == NULL);

    arena->base_pointer  = backbuffer;
    arena->offset      = 0;
    arena->prev_offset = 0;
    arena->capacity    = buf_length;
}

void* arena_alloc(Arena* arena, size_t size) {
    assert((arena->offset + size) <= arena->capacity);

    arena->prev_offset = arena->offset;
    arena->offset += size;

    return arena->base_pointer + arena->prev_offset;
}

template <typename T>
T* arena_push(Arena* arena, T data) {
    auto dest = arena_alloc(arena, sizeof(T));

    memcpy(dest, &data, sizeof(T));
    return (T*) (arena->base_pointer + arena->prev_offset);
}

void arena_reset(Arena* arena) {
    arena->offset = 0;
    arena->prev_offset = 0;
}

struct Temp_Arena {
    Arena* arena;
    size_t offset;
    size_t prev_offset;
};

Temp_Arena temp_begin(Arena* arena) {
    return {
        .arena = arena,
        .offset = arena->offset,
        .prev_offset = arena->prev_offset,
    };
}

void temp_end(Temp_Arena temp) {
    temp.arena->offset = temp.offset;
    temp.arena->prev_offset = temp.prev_offset;
}
