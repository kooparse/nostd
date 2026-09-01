#pragma once

#include <algorithm>
#include <thread>
#include <mutex>
#include <cstdint>
#include <semaphore>
#include "array.hpp"

using std::thread;
using std::counting_semaphore;

class Thread_Group;

struct Work_Info {
    void*    work; // User work data.
    uint32_t index; // Thread index.
};

typedef void (*Group_Func)(Work_Info, Thread_Group*);
static const uint32_t MAX_THREAD_COUNT = 64;

class Thread_Group {
    public:
        const char* name;

        void initialize(Group_Func);
        void shutdown();
        void add_work(void* work_data);
        Array<void*> get_finished_work();

        ~Thread_Group() { shutdown(); }

    private:
        Array<void*>      work_available;
        Array<void*>      work_finished;

        Group_Func        work_job;
        std::mutex        mut;
        std::atomic<bool> should_exit = false;

        counting_semaphore<> semaphore{0};

        thread  threads[MAX_THREAD_COUNT];
        uint32_t    thread_count = 0;
};

inline void Thread_Group::add_work(void* work_data) {

    {
        std::unique_lock<std::mutex> lock(mut);
        array_add(&work_available, work_data);
    }

    semaphore.release();
}


inline void Thread_Group::initialize(Group_Func func) {
    thread_count = std::max<uint32_t>(1, thread::hardware_concurrency());
    thread_count = std::min<uint32_t>(thread_count, MAX_THREAD_COUNT);

    work_job     = func;

    work_available = {};
    work_finished  = {};

    for (uint32_t i = 0; i < thread_count; i++) {
        threads[i] = std::thread([this, i]{
            while (true) {
                semaphore.acquire();

                if (should_exit.load()) {
                    return;
                }

                void* work_data;

                {
                    std::unique_lock<std::mutex> lock(mut);
                    work_data = array_pop(&work_available);
                }

                auto work_info = Work_Info{ .work = work_data, .index = i };

                work_job(work_info, this);

                std::unique_lock<std::mutex> lock(mut);
                array_add(&work_finished, work_info.work);
            }
        });
    }
}

inline Array<void*> Thread_Group::get_finished_work() {
    std::unique_lock<std::mutex> lock(mut);

    auto finished = work_finished;
    work_finished = {};

    return finished;
}

inline void Thread_Group::shutdown () {
    if (thread_count == 0) return;

    should_exit.store(true);

    for (uint32_t i = 0; i < thread_count; i++) {
        semaphore.release();
    }

    for (uint32_t i = 0; i < thread_count; i++) {
        threads[i].join();
    }

    thread_count = 0;

    array_free(&work_available);
    array_free(&work_finished);
}
