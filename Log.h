#pragma once
#include <iostream>
#include <semaphore>   // C++20

// single global binary_semaphore to serialize log lines
inline std::binary_semaphore& log_lock() {
    static std::binary_semaphore s{1};
    return s;
}

// print a full line atomically so threads don't interleave
template <class F>
inline void logln(F&& emit_line) {
    log_lock().acquire();
    emit_line(std::cout);
    std::cout << std::flush;
    log_lock().release();
}
