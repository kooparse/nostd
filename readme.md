# nostd

A collection of very useful code which I'm using often to make software.

## Arena Allocator

This code allows us to manage the memory to objects which are sharing one block
of memory (and lifetime) all together. Way better than malloc/free all the
time. For a video game, we would have one frame allocator for example which
would be reset after each frame tick.

```c++
#include "arena_allocator.hpp"

struct MyStruct {
    int         id;
    const char* text;
};

int main() {
    Arena arena;

    // Let's first create some allocated memory which we will fill to our
    // arena. It could be allocated on the stack or on the heap. I will soon add
    // my own code to allocate memory with mmap.
    //
    // For this example, let's use malloc.
    auto buffer_length = 1024;
    auto backbuffer    = malloc(buffer_length);

    // Let's assign our arena to this allocated memory.
    arena_init(&arena, backbuffer, buffer_length);

    // Now, let's push some data here.
    auto ptr_to_int = arena_push<int>(&arena, 23);
    auto ptr_to_struct = arena_push<MyStruct>(&arena, { 10, "Hey" });

    // Let's display our data.
    printf("My int: %i", *ptr_to_int);
    printf("My struct: id: %i, text: %s", ptr_to_struct->id, ptr_to_struct->text);


    // We can as well have some "scratch" memory in our arena. Very useful for
    // little "allocation".
    {
        Temp_Arena temp = temp_begin(&arena);

        arena_push<float>(&arena, 1200.23);
        arena_push<float>(&arena, 2200.23);

        // Once we call "temp_end" we simply restore the arena cursor to where
        // we were at the beginning.
        temp_end(temp);
    }

    // Arena internal cursor is now back at 0.
    arena_reset(&arena);

    return 0;
}
```

## Thread Group

Often we want to parallelize the "same" kind of work. This code is doing exactly that.
It's notably useful for works which are "independent" to each other. Like
loading files, textures, etc...

*The interface of Thread_Group is inspired by Jonathan Blow's own Thread_Group
Jai module.*

Here's how it works:

```c++
#include "thread_group.hpp"

// Let's say we want to parse some file content, and add all value.
const char* FILE_1 = "10";
const char* FILE_2 = "20";
const char* FILE_3 = "30";
const char* FILE_4 = "40";

struct Work {
    const char* file_content;
    int         parsed_int;
};

void do_some_work(Work_Info work_info, Thread_Group* group) {
    printf("Thread %i in Thread Group %s is doing some work", work_info.index, group->name);

    auto work = (Work*) work_info.work;
    work->parsed_int = atoi(work->file_content);
}

int main() {
    Thread_Group group;
    // You can name your group, useful for debug purpose.
    group.name = "This is my thread group.";

    // Assign the kind of work you want the threads to do.
    group.initialize(do_some_work);

    const int WORK_COUNT_TO_DO = 4;

    Work worklist [WORK_COUNT_TO_DO] = {
        { .file_content = FILE_1, },
        { .file_content = FILE_2, },
        { .file_content = FILE_3, },
        { .file_content = FILE_4, },
    };

    // Add work to do, once "add_work" is called, some thread will pick it up.
    for (int i = 0; i < WORK_COUNT_TO_DO; i++) {
        group.add_work(&worklist[i]);
    }

    auto finish_count = 0;
    auto result = 0;
    while (finish_count < WORK_COUNT_TO_DO) {
        // Pull all the finished work.
        auto finish_list = group.get_finished_work();
        finish_count += finish_list.count;

        // Do something with the finished work.
        for (int i = 0; i < finish_list.count; i++) {
            auto work = (Work*) finish_list.data[i];
            result += work->parsed_int;
        }
    }

    // Should display the number 100!
    printf("Work finished, result: %i", result);

    // The destructor is calling the "shutdown" function.

    return 0;
}
```

## Array

Basic dynamic array with useful functions on it. It soon will accept an arena
allocator instead of internally malloc/free though.

```c++
#include "array.hpp"

int main() {
    Array<float> arr;

    array_add(&arr, 12.0);
    array_add(&arr, 7.0);
    array_add(&arr, 1.0);
    array_add(&arr, 7.0);

    // Will display 4.
    printf("Array count is %i.", arr.count);

    // We return a copy of the value removed.
    auto value_removed = array_pop(&arr);

    // Will display 3.
    printf("Array count is %i.", arr.count);

    auto ref = array_peek(&arr);
    // This will display 1.0.
    printf("Value %f.", *ref);

    // This remove the value at a specified index, and keep the original order.
    array_remove_at_index(&arr, 2);

    // But we you don't care about the order of items, there's this.
    // Which internally will place the last item in the removed slot index.
    // array_remove_at_index_unordered(&arr, 2);

    // Free everything.
    array_free(&arr);

    return 0;
}
```


# License

MIT License

Copyright (c) 2026 Alexandre Chêne

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
