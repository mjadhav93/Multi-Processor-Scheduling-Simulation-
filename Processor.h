#pragma once
#include <string>
#include <thread>
#include <chrono>
#include <iostream>
#include "Scheduler.h"
#include "Task.h"
#include "Log.h"

class Processor {
public:
    // 'param' kept for parity with your Java fields (e.g., core speed)
    Processor(int id, int param, Scheduler* s, int unitMs = 50)
      : id_(id), param_(param), sched_(s), unitMs_(unitMs) {}

    void operator()() {
        using namespace std::chrono;

        for (;;) {
            Task* t = sched_->getNextTask();
            if (t == nullptr) {
                // Scheduler stopping and no work left
                break;
            }

            // Log dispatch atomically
            logln([&](std::ostream& os){
                os << "[DISPATCH] " << sched_->name()
                   << " -> CPU-" << id_
                   << " picked Task-" << t->id
                   << " (remaining=" << t->remaining.load() << ")\n";
            });

            // -------- Non-preemptive SJF: run to completion --------
            while (!t->isCompleted()) {
                std::this_thread::sleep_for(milliseconds(unitMs_));
                t->executeQuantum(1);

                // Log progress each time unit (atomic)
                logln([&](std::ostream& os){
                    os << "    [RUNNING] CPU-" << id_
                       << " Task-" << t->id
                       << " (remaining=" << t->remaining.load() << ")\n";
                });
            }

            // Log completion
            logln([&](std::ostream& os){
                os << "[COMPLETE] Task-" << t->id
                   << " finished on CPU-" << id_ << "\n";
            });

            // Task finished: free seat and notify task spawner
            sched_->releaseSlot();
            t->done.release();
        }
    }

private:
    int id_;
    int param_;
    Scheduler* sched_;
    int unitMs_;
};
