#include <stdio.h>
#include <assert.h>
#include <string.h>

#include "array.h"
#include "hash_table.h"
#include "arena_allocator.h"

void array_tests() {
    printf("\n\n\nARRAY TESTS \n\n\n");

    auto array = Array<int>{};

    array_add(&array, 2);
    array_add(&array, 3);
    array_add(&array, 4);

    for(int i = 0; i < array.count; i++) {
        printf("index: %i, value: %d\n", i, array.data[i]);
    }

    assert(array.count == 3);

    auto last = array_pop(&array);
    assert(last == 4);
    assert(array.count == 2);

    last = array_pop(&array);
    assert(last == 3);
    assert(array.count == 1);

    array_add(&array, 4);
    array_add(&array, 83);
    array_add(&array, 24);
    array_add(&array, 4);
    array_add(&array, 76);
    array_add(&array, 12);
    assert(array.count == 7);
    assert(*array_peek(&array) == 12);

    auto removed = array_remove_at_index(&array, 2);
    assert(removed == 83);
    assert(array.count == 6);
    assert(array.data[2] == 24);
    assert(array.data[3] == 4);
    assert(array.data[4] == 76);
    assert(array.data[5] == 12);

    removed = array_remove_at_index_unordered(&array, 2);
    assert(removed == 24);
    assert(array.count == 5);
    assert(array.data[2] == 12);
    assert(array.data[4] == 76);

    auto index = array_find_index(&array, 76);
    assert(index == 4);

    array_free(&array);
    assert(array.data == nullptr && array.count == 0);
}


void arena_tests () {
    printf("\n\n\nARENA TESTS \n\n\n");

    struct MyStruct {
        float field;
        int another_field;
    };

    auto struct_size = align_forward(sizeof(MyStruct), 16);

    Arena arena{};

    auto buffer_length = sizeof(char) * 1024;
    auto backbuffer    = malloc(buffer_length);

    arena_init(&arena, (unsigned char*) backbuffer, buffer_length);
    assert(arena.offset == 0);

    printf("arena offset: %zu", arena.offset);

    auto d = arena_push<MyStruct>(&arena, { 12.5, 80 });
    assert(arena.offset == struct_size);
    printf("index: %f, value: %i\n", d->field, d->another_field);

    d = arena_push<MyStruct>(&arena, { 40.245, 30 });
    assert(arena.offset == struct_size * 2);
    printf("index: %f, value: %i\n", d->field, d->another_field);

    printf("arena offset: %zu\n", arena.offset);

    {
        Temp_Arena temp = temp_begin(&arena);

        d = arena_push<MyStruct>(&arena, { 2.1, 9 });
        assert(arena.offset == struct_size * 3);
        printf("index: %f, value: %i\n", d->field, d->another_field);

        d = arena_push<MyStruct>(&arena, { 3.4, 7 });
        assert(arena.offset == struct_size * 4);
        printf("index: %f, value: %i\n", d->field, d->another_field);

        temp_end(temp);
    }

    {
        Temp_Arena temp = temp_begin(&arena);

        arena_push<MyStruct>(&arena, { 2.1, 9 });
        assert(arena.offset == struct_size * 3);

        arena_push<MyStruct>(&arena, { 3.4, 7 });
        assert(arena.offset == struct_size * 4);

        temp_end(temp);
    }


    assert(arena.offset == struct_size * 2);
    printf("arena offset: %zu\n", arena.offset);

    arena_reset(&arena);
    assert(arena.offset == 0);

    printf("arena offset: %zu\n", arena.offset);

}

int main() {
    array_tests();
    arena_tests();

    return 0;
}

