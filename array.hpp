#pragma once

#include <stdlib.h>

template <typename T>
struct Array {
    T*   data   = nullptr;
    int  count  = 0;

    size_t capacity = 0;
};

template <typename T>
void array_init(Array<T>* array) {
    if (array->data != nullptr) return;

    array->count     = 0;
    array->capacity  = sizeof(T) * 100;
    array->data      = (T*) malloc(array->capacity);
}

template <typename T>
void array_add(Array<T>* array, T value) {
    array_init(array);

    if (array->capacity < (size_t) array->count) {
        array->data = (T*) realloc(array->data, array->capacity * 2);
    }

    array->data[array->count] = value;
    array->count += 1;
}

template <typename T>
T array_pop(Array<T>* array) {
    auto last = array->data[array->count - 1];
    array->count -= 1;

    return last;
}

template <typename T>
T* array_peek(Array<T>* array) {
    return &array->data[array->count - 1];
}

template <typename T>
int array_find_index(Array<T>* array, T to_find) {
    auto index = -1;

    for (int i = 0; i < array->count; i++) {
        if (array->data[i] == to_find) {
            index = i;
            break;
        }
    }

    return index;
}

// Note: ordered removal.
template <typename T>
T array_remove_at_index(Array<T>* array, int index) {
    auto removed = array->data[index];

    for (int i = index + 1; i < array->count; i++) {
        array->data[i - 1] = array->data[i];
    }

    array->count -= 1;

    return removed;
}

template <typename T>
T array_remove_at_index_unordered(Array<T>* array, int index) {
    auto removed = array->data[index];

    auto last = array_pop(array);
    array->data[index] = last;

    return removed;
}

template <typename T>
void array_free(Array<T>* array) {
    free(array->data);

    array->data     = nullptr;
    array->count    = 0;
    array->capacity = 0;
}
