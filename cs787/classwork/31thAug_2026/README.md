# 31st Aug 2026 — Threads: Monitoring, Multiple Resources & Deadlock Avoidance

Continuation of the threads lecture (24th Aug). It covers:
- one thread stopping another through a shared flag
- tracking thread states from `main`
- synchronising threads that need **several resources at once** without
  deadlock

## Files

| File | Topic |
|---|---|
| `thread_ex.cpp` | Monitor thread stops a worker thread through a shared flag |
| `thread.cpp` | `ThreadMonitor`: a registry of thread IDs and states, guarded by `lock_guard` |
| `multi-resource.cpp` | 7 threads over 3 resources `a,b,c`, one mutex each, consistent lock order |
| `multi-resource-syncrhonization.cpp` | Bank transfer between two accounts with `std::scoped_lock` |

Build: `g++ -std=c++17 -pthread <file>.cpp && ./a.out`

---

## Key concepts

### Stopping a thread cooperatively (`thread_ex.cpp`)
- `f()` loops while `c != 0`, printing `hello<v>` every 200 ms.
- `monitor()` polls `v` every 100 ms and sets `c = 0` once `v >= 15`.
- `f()` then **exits gracefully**. You can't safely kill a thread from
  outside, so you ask it to stop through a flag.
- ⚠️ `c` and `v` are plain `int`s shared between threads, which is a data
  race. They should be `std::atomic<int>` / `std::atomic<bool>`.

### Thread monitoring (`thread.cpp`)
- `std::this_thread::get_id()` gives a unique `std::thread::id`, which is
  used as the key in an `unordered_map` registry.
- Each worker reports `STARTING → RUNNING → WAITING → FINISHED`.
- `main` prints a snapshot every 300 ms.
- `std::lock_guard<std::mutex>`: an **RAII** lock that is released
  automatically at the end of its scope. It is safer than calling
  `lock()`/`unlock()` yourself.
- `vector<thread>` + `emplace_back(func, args)` starts many threads.
  `joinable()` is checked before `join()`.
- When this was run, the first snapshot showed only **2 of the 3** workers.
  Thread start-up timing is not deterministic.

### Multiple resources (`multi-resource.cpp`)
- **Problem (from the file):** resources `a, b, c`. The threads need: 1→a,
  2→b, 3→c, 4→ab, 5→bc, 6→ac, 7→abc. Threads that use disjoint resources
  should be allowed to run at the same time, and no allowed overlap should
  be blocked.
- **Solution:**
  - one mutex per resource
  - each thread locks only the mutexes it needs
  - the locks are always taken in the **same global order (a → b → c)** and
    released in reverse order
- **Lock ordering prevents deadlock**, because it rules out circular wait.
- Observed: 1, 2 and 3 run together, then 4, 5, 6 and 7 effectively one at a
  time. The whole run takes about 40 s.
- Limitation: a thread holds `a` while it waits for `b` (hold-and-wait), so
  it can block a thread that needs only `a` even though `a` sits idle. Some
  allowed overlaps are lost.
- The original semaphore version is kept in comments
  (`counting_semaphore<1>`). A binary semaphore works like a mutex here.
- The comment says the gap is "200 ms", but the code sleeps **2000 ms**.

### Deadlock and `std::scoped_lock` (`multi-resource-syncrhonization.cpp`)
- t1 transfers acc1→acc2 while t2 transfers acc2→acc1.
- If each thread locks `from` and then `to`, this is a **classic deadlock**:
  each holds one lock and waits for the other.
- **`std::scoped_lock lock(m1, m2);`** (C++17) locks several mutexes using a
  deadlock-avoidance algorithm, then releases all of them at the end of the
  scope (RAII).
- The self-transfer guard (`&from == &to`) prevents locking the same mutex
  twice, which would be undefined behaviour.
- Pass objects to a thread by reference with `std::ref(acc)`. Thread
  arguments are copied by default.
- Final balances: `$750 / $750`.
  - Account 1: 1000 − 5×100 + 5×50 = 750.
  - Account 2: 500 + 5×100 − 5×50 = 750.

### Deadlock: the 4 conditions (and how today's code breaks one)
| Condition | Broken by |
|---|---|
| Mutual exclusion | — (needed for correctness) |
| Hold and wait | `scoped_lock` acquires all the locks together |
| No preemption | — |
| Circular wait | Fixed lock order a → b → c |

## Quick recap
1. Stop a thread by asking it with a shared flag. Make the flag
   `std::atomic`.
2. `get_id()` identifies threads. Guard shared maps with `lock_guard`.
3. Several resources → one mutex each, locked in a **consistent order**.
4. Prefer `std::scoped_lock(m1, m2, ...)` to lock several mutexes safely.
5. Use `std::ref` to pass references into a thread.
6. `cout` from several threads without a lock can mix lines together.
