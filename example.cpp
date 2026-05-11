#include <stdio.h>
#include <assert.h>
#include "array.h"
#include "hash_table.h"

void array_tests() {
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

int main() {
    array_tests();

    return 0;
}

