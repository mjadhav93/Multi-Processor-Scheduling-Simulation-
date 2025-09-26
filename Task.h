#pragma once
#include <atomic>
#include <semaphore>   

struct Task {
    int id;
    int totalUnits;
    std::atomic<int> remaining;
    std::binary_semaphore done{0}; // signaled by CPU on completion

    Task(int id_, int units) : id(id_), totalUnits(units), remaining(units) {}

    void executeQuantum(int q) {
        int r = remaining.load(std::memory_order_relaxed);
        r -= q;
        remaining.store(r < 0 ? 0 : r, std::memory_order_relaxed);
    }
    bool isCompleted() const { return remaining.load(std::memory_order_relaxed) <= 0; }
};
