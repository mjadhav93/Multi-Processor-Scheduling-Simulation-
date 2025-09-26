#pragma once
#include <queue>
#include <vector>
#include <string>
#include <semaphore>   // C++20
#include <climits>
#include <atomic>

struct Task;

// Short-term scheduler with capacity 15 and SJF-ready queue (min remaining first)
class Scheduler {
public:
    explicit Scheduler(std::string name, int capacity = 15);

    const std::string& name() const { return name_; }

    // Blocks if capacity full; enqueues a fresh task (consumes one capacity seat)
    void addTask(Task* t);

    // Re-enqueue (e.g., after preemption). Does NOT touch capacity.
    void reAddTask(Task* t);

    // CPU side: wait for work and pop the shortest.
    // Returns nullptr if stop() was called AND the queue is empty.
    Task* getNextTask();

    // Peek min remaining (for optional SRTF)
    int peekMinRemainingOrInf();

    // Release one capacity seat (call when a task actually completes)
    void releaseSlot();

    // Initiate shutdown: wake CPUs blocked on availableTasks_
    void stop();

    bool isStopping() const { return stopping_.load(std::memory_order_relaxed); }

private:
    struct Cmp {
        bool operator()(const Task* a, const Task* b) const;
    };

    std::string name_;
    std::priority_queue<Task*, std::vector<Task*>, Cmp> pq_;
    std::binary_semaphore qMutex_{1};                       // guards pq_
    std::counting_semaphore<INT_MAX> availableTasks_{0};    // tasks present for CPUs
    std::counting_semaphore<15>      slots_;                // capacity seats (max 15)
    std::atomic<bool> stopping_{false};
};
