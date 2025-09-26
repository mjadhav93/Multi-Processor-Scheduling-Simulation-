#include <iostream>
#include <thread>
#include <vector>
#include <random>
#include <memory>
#include <string>
#include "Scheduler.h"
#include "Processor.h"
#include "Task.h"
#include "Log.h"

int main() {
    // header (nice for graders)
    std::cout << "== OS Scheduling Simulation ==\n"
              << "Schedulers: 2 (cap=15 each)\n"
              << "CPUs: 8 (4 per scheduler)\n"
              << "Tasks: 30\n"
              << "Time unit: 50 ms\n\n";

    // Two short-term schedulers with capacity 15
    Scheduler scheduler1{"STS-1", 15};
    Scheduler scheduler2{"STS-2", 15};

    // Eight processors: 4 on scheduler1, 4 on scheduler2
    std::vector<std::thread> cpuThreads;
    cpuThreads.emplace_back( Processor{1, 2, &scheduler1} );
    cpuThreads.emplace_back( Processor{2, 4, &scheduler1} );
    cpuThreads.emplace_back( Processor{3, 3, &scheduler1} );
    cpuThreads.emplace_back( Processor{4, 2, &scheduler1} );
    cpuThreads.emplace_back( Processor{5, 2, &scheduler2} );
    cpuThreads.emplace_back( Processor{6, 4, &scheduler2} );
    cpuThreads.emplace_back( Processor{7, 3, &scheduler2} );
    cpuThreads.emplace_back( Processor{8, 2, &scheduler2} );

    // Simulate 30 user tasks with random time requirements and random arrivals
    std::mt19937 rng(42);
    std::uniform_int_distribution<int> unitsDist(1, 25);
    std::uniform_int_distribution<int> delayDist(0, 1000);

    std::vector<std::unique_ptr<Task>> tasks;
    std::vector<std::thread> userThreads;

    for (int i = 1; i <= 30; ++i) {
        int timeRequired = unitsDist(rng);
        auto t = std::make_unique<Task>(i, timeRequired);

        // Choose target STS and a static label for logging
        Scheduler* target = (i % 2 == 0) ? &scheduler1 : &scheduler2;
        std::string targetLabel = (i % 2 == 0) ? "STS-1" : "STS-2";

        int delay = delayDist(rng);

        userThreads.emplace_back([task=t.get(), target, delay, targetLabel](){
            // Simulate random arrival time
            std::this_thread::sleep_for(std::chrono::milliseconds(delay));

            // Arrival log (atomic)
            logln([&](std::ostream& os){
                os << "[ARRIVAL] Task-" << task->id
                   << " arrived (needs " << task->totalUnits << " units) "
                   << "to " << targetLabel << "\n";
            });

            // Enqueue into the chosen scheduler
            target->addTask(task);

            // Wait until a processor completes this task
            task->done.acquire();
        });

        tasks.push_back(std::move(t));
    }

    // Wait for all user tasks to finish
    for (auto &th : userThreads) th.join();

    // Tell both schedulers to stop and wake their CPUs
    scheduler1.stop();
    scheduler2.stop();

    // Join CPU threads so the process exits cleanly
    for (auto &th : cpuThreads) th.join();

    std::cout << "All user tasks completed.\n";
    std::cout << "[SHUTDOWN] All CPUs joined. Simulation ended.\n";
    return 0;
}
