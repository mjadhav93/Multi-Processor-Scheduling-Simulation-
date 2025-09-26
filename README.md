# Multi-Processor-Scheduling-Simulation-
A modular C++20 simulation of OS scheduling with 8 processors and 2 short-term schedulers, built using threads and semaphores to enforce concurrency, fairness, and synchronization.


## Overview
This project simulates a simplified **operating system CPU scheduler** using C++20 threads and semaphores.  
It models **user tasks**, **short-term schedulers**, and **processors** to demonstrate concurrency control, task scheduling, and synchronization.

The simulation enforces:
- Two short-term schedulers (capacity: 15 tasks each)
- Eight processors (4 per scheduler)
- Random user task arrivals (1–25 execution units each)
- Non-preemptive **Shortest Job First (SJF)** scheduling
- Semaphore-only synchronization (no mutexes or condition variables)

---

## 🔑 Key Features
- **Semaphore-based synchronization** (binary + counting semaphores only).
- **Shortest Job First (SJF)** scheduling policy with priority queues.
- **Randomized task arrivals** to simulate real system dynamics.
- **Atomic logging** using semaphores to prevent interleaved output.
- **Graceful shutdown** ensuring no dangling threads or crashes.
- **Modular design** (Task, Scheduler, Processor, Logging).

---

## 📂 Modules
- `Task`: Represents a user process (ID, execution units, completion semaphore).
- `Scheduler`: Manages task queues, enforces capacity, signals task availability.
- `Processor`: Simulates a CPU core executing tasks unit by unit.
- `Log`: Provides thread-safe logging with a global semaphore.
- `main.cpp`: Initializes the system, spawns tasks, and manages shutdown.

---

## ⚙️ How to Build & Run
### Compile with `g++`:
```bash
g++ -std=c++20 -O2 -pthread main.cpp Scheduler.cpp -o os_sim
./os_sim what descprion hould i add to put on github
