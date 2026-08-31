#include <stdio.h>
#include <assert.h>

#include "array.hpp"
#include "arena_allocator.hpp"
#include "thread.hpp"

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

    auto struct_size = sizeof(MyStruct);

    Arena arena{};

    auto buffer_length = 1024;
    auto backbuffer    = malloc(buffer_length);

    arena_init(&arena, (unsigned char*) backbuffer, buffer_length);
    assert(arena.offset == 0);

    printf("arena offset: %zu\n", arena.offset);

    auto d = arena_push<MyStruct>(&arena, { 12.5, 80 });
    assert(arena.offset == struct_size);
    printf("index: %f, value: %i\n", d->field, d->another_field);

    d = arena_push<MyStruct>(&arena, { 40.245, 30 });
    assert(arena.offset == 24);
    printf("index: %f, value: %i\n", d->field, d->another_field);

    printf("arena offset: %zu\n", arena.offset);

    {
        Temp_Arena temp = temp_begin(&arena);

        d = arena_push<MyStruct>(&arena, { 2.1, 9 });
        assert(arena.offset == 40);
        printf("index: %f, value: %i\n", d->field, d->another_field);

        d = arena_push<MyStruct>(&arena, { 3.4, 7 });
        assert(arena.offset == 56);
        printf("index: %f, value: %i\n", d->field, d->another_field);

        temp_end(temp);
    }

    {
        Temp_Arena temp = temp_begin(&arena);

        arena_push<MyStruct>(&arena, { 2.1, 9 });
        assert(arena.offset == 40);

        arena_push<MyStruct>(&arena, { 3.4, 7 });
        assert(arena.offset == 56);

        temp_end(temp);
    }


    assert(arena.offset == 24);
    printf("arena offset: %zu\n", arena.offset);

    arena_reset(&arena);
    assert(arena.offset == 0);

    printf("arena offset: %zu\n", arena.offset);

}

const char* FILE_1 = "10";
const char* FILE_2 = "20";
const char* FILE_3 = "30";
const char* FILE_4 = "40";

struct Work {
    const char* file_content;
    int parsed_int;
};

void demo_worker(Work_Info work_info, Thread_Group* thread_group) {
    printf("Running \"%s\" :: thread \"%i\"\n", thread_group->name, work_info.index);

    auto work = (Work*) work_info.work;
    work->parsed_int = atoi(work->file_content);
}

void thread_tests () {
    printf("\n\n\nTHREAD GROUP\n\n\n");

    Thread_Group group;

    group.name = "My Thread group!";

    group.initialize(demo_worker);

    const int WORK_COUNT_TO_DO = 4;

    Work worklist [WORK_COUNT_TO_DO] = {
        { .file_content = FILE_1, },
        { .file_content = FILE_2, },
        { .file_content = FILE_3, },
        { .file_content = FILE_4, },
    };

    group.add_work(&worklist[0]);
    group.add_work(&worklist[1]);
    group.add_work(&worklist[2]);
    group.add_work(&worklist[3]);

    auto finish_count = 0;
    int result        = 0;

    while (finish_count < WORK_COUNT_TO_DO) {
        auto finish_list = group.get_finished_work();
        finish_count += finish_list.count;

        for (int i = 0; i < finish_list.count; i++) {
            auto work = (Work*) finish_list.data[i];
            result += work->parsed_int;
        }
    }

    printf("Work finished, result: %i", result);
    assert(result == 100);
}

int main() {
    array_tests();
    arena_tests();

    thread_tests();

    return 0;
}

