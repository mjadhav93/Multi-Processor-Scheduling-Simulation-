#include "Scheduler.h"
#include "Task.h"

Scheduler::Scheduler(std::string name, int capacity)
    : name_(std::move(name)), slots_(capacity) {}

bool Scheduler::Cmp::operator()(const Task* a, const Task* b) const {
    int ra = a->remaining.load(std::memory_order_relaxed);
    int rb = b->remaining.load(std::memory_order_relaxed);
    if (ra != rb) return ra > rb;        // min-heap by remaining
    return a->id > b->id;                // tie-breaker by id
}

void Scheduler::addTask(Task* t) {
    // Reserve a seat for a newly admitted task
    slots_.acquire();

    qMutex_.acquire();
    pq_.push(t);
    qMutex_.release();

    // Wake a CPU
    availableTasks_.release();
}

void Scheduler::reAddTask(Task* t) {
    // Preempted task already holds its seat
    qMutex_.acquire();
    pq_.push(t);
    qMutex_.release();

    availableTasks_.release();
}

Task* Scheduler::getNextTask() {
    // Wait for a task or for stop() to wake us
    availableTasks_.acquire();

    qMutex_.acquire();
    Task* t = nullptr;
    if (!pq_.empty()) {
        t = pq_.top();
        pq_.pop();
    }
    qMutex_.release();

    if (t == nullptr && stopping_.load(std::memory_order_relaxed)) return nullptr;
    return t;
}

int Scheduler::peekMinRemainingOrInf() {
    qMutex_.acquire();
    int v = pq_.empty() ? INT_MAX : pq_.top()->remaining.load(std::memory_order_relaxed);
    qMutex_.release();
    return v;
}

void Scheduler::releaseSlot() {
    // Free the seat when the task truly completes
    slots_.release();
}

void Scheduler::stop() {
    // Mark stopping and wake any CPUs blocked on availableTasks_
    stopping_.store(true, std::memory_order_relaxed);
    availableTasks_.release(1'000'000);  // generous nudge to unblock all waiters
}
