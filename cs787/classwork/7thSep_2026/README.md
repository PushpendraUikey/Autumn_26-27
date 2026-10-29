# 7th Sep 2026 — Locks: `lock_guard`, `unique_lock`, `try_lock`, `recursive_mutex` & Condition Variable Pitfalls

Recap of the lecture:
- RAII lock wrappers and what each one allows
- non-blocking `try_lock` and dining philosophers
- recursive locking
- condition variables, mostly through **problematic** versions, to see what
  goes wrong without a predicate

Every program was compiled and run. The observed behaviour is noted below.

## Files

| Topic | File | Observed |
|---|---|---|
| `lock_guard`: scoped lock, no manual unlock | `lock_guards.cpp` | One method prints all 4 lines, then the other (fully serialised) |
| `unique_lock`: manual `unlock()`/`lock()` | `unique_lock.cpp` | Interleaves in pairs: `++++0 ++++1 ----0 ----1 ++++2 …` |
| `unique_lock` locking twice | `unique_lock-undefined-behavior-possible-deadlock.cpp` | Aborts: `std::system_error: Resource deadlock avoided` |
| `recursive_mutex` | `problem-code-recursive_mutex.cpp` | t1 prints sums 1..55, then t2 prints 1..36, never interleaved |
| `try_lock` spin loop | `trylock.cpp` | `f`/`g` alternate |
| Dining philosophers with `try_lock` back-off | `trylock-erroneous.cpp` | Both philosophers eat, no deadlock |
| CV + atomic flag | `0-problematic-cond-var.cpp` | Threads alternate |
| CV, lock held across the loop | `1-problematic-cond-var.cpp` | Threads alternate |
| CV without predicate | `2-problematic-cond-var.cpp` | Alternates, but **hangs 9/10 runs** if `main`'s sleep is removed |
| Relay / "rendezvous" | `rendezvous-prob0.cpp` | T1 → T2 → T3, `x` = 0..11 |
| Rendezvous done right (predicate) | `rendezvous-prob1.cpp` | Prints `0 1 2 3`, exits cleanly |
| Producer–consumer with predicates | `2prodcon-one-prob.cpp` | `producing n` / `consuming n`, alternating |

Build: `g++ -std=c++17 -pthread <file>.cpp && ./a.out`. Most programs loop
forever, so stop them with Ctrl+C.

---

## Key concepts

### RAII lock wrappers
- **RAII (Resource Acquisition Is Initialisation):** the constructor locks
  and the destructor unlocks. The unlock happens even on an early `return`
  or an exception.
- **`lock_guard<mutex>`:**
  - The simplest wrapper. It locks for the whole scope.
  - It has **no `unlock()` or `lock()`**, so calling them doesn't compile.
  - In `lock_guards.cpp`, `f` and `g` share one mutex, so whichever starts
    first finishes all its iterations before the other begins.
- **`unique_lock<mutex>`:**
  - More flexible: `unlock()`, `lock()`, `try_lock()`, deferred locking
    (`defer_lock`), and it can be moved.
  - In `unique_lock.cpp`, unlocking on odd iterations lets the other thread
    slip in.
  - **Required for `condition_variable::wait()`**, because `wait()` must
    unlock and relock the mutex.
- **Locking a `unique_lock` that already holds its lock** throws
  `std::system_error` (`resource_deadlock_would_occur`). The `unique_lock`
  checks for this, so the program aborts rather than hanging.
  - Calling `lock()` twice on a **raw `std::mutex`** from the same thread is
    **undefined behaviour**. It typically deadlocks.

### `try_lock`
- `try_lock()` **never blocks**. It returns `true` if it got the lock and
  `false` if not.
- `while (!m.try_lock());` (`trylock.cpp`) is a **spinlock**. It busy-waits
  and burns CPU whenever the lock is contended. Here the critical section is
  tiny, so it barely spins.
- **Dining philosophers** (`trylock-erroneous.cpp`):
  - P1 locks `f1` then `f2`, blocking.
  - P2 uses `try_lock` on `f2` then `f1`, and **releases what it holds and
    retries** if the second lock fails.
  - This breaks *hold-and-wait*, so there is **no deadlock**.
  - Remaining risks: P2 can **starve**, or with unlucky timing **livelock**
    (repeatedly back off and retry without progress).
  - `std::lock(f1, f2)` / `std::scoped_lock` (from 31st Aug) does this
    safely.

### `recursive_mutex`
- The **same thread** can lock it several times. It must unlock it the same
  number of times.
- With a plain `mutex`, the recursive `f(x-1)` would lock the mutex it
  already holds, which is undefined behaviour or a self-deadlock.
- The lock is held for the **entire recursion**, so t1's whole computation
  runs before t2 starts. It is correct, but the threads don't run in
  parallel. Needing a recursive mutex is often a sign the design could be
  restructured, for example with an unlocked inner helper function.

### Condition variables: rules learnt from the "problematic" versions
- **`cv.wait(lock, pred)`** blocks only while `pred` is false.
  - If `pred` is already true, it doesn't wait at all.
  - It re-checks `pred` after every wake-up.
  - This one line deals with both of the problems below.
- **Lost wakeup:**
  - `notify` only wakes threads **already waiting**. A notification sent
    before anyone waits is simply lost.
  - `1-`, `2-problematic` and `rendezvous-prob0` depend on `main` sleeping
    first.
  - With the sleep removed, `2-problematic` hung in **9 out of 10 runs**,
    with both threads stuck in `wait()`.
- **Spurious wakeup:** `wait()` can return with no notify at all. A `wait`
  without a predicate (`2-`, `rendezvous-prob0`) would then go ahead too
  early.
- **Notify after unlocking** (`1-problematic` comment):
  - If you notify while holding the lock, the woken thread moves from the
    CV queue straight to the mutex queue and blocks again.
  - Unlock first, then notify (as `0-` and `2prodcon` do).
- **The predicate must be shared state, read under the lock.**
  - In `0-problematic`, the flag is always reset to `true` *before* the
    unlock, so the other thread never sees `false`.
  - That means its `cv.wait` is never reached, and the alternation actually
    comes from the mutex alone.
  - Note that the deadlock described in the file's comment does not happen.
- **Holding the lock across a whole loop** (`1-problematic`) only works
  because `wait()` releases it. All the printing and sleeping happens while
  holding the lock.

### Rendezvous / relay
- **Rendezvous:** threads meet at a sync point before continuing.
- **Relay (baton passing)** (`rendezvous-prob0`):
  - `main` notifies one thread.
  - That thread works, then calls `notify_all()`, and the rest follow one at
    a time.
  - It is fragile: a thread that hasn't reached `wait()` yet misses the
    baton and hangs.
- **`rendezvous-prob1`** fixes it with a predicate on `work_done`. Thread 1
  proceeds whether it runs before or after thread 0.
- Tip from the file: **reason through every interleaving** (A first, B
  first, both at once) to check a solution.

### Producer–consumer with CV (`2prodcon-one-prob.cpp`)
- Producer: `wait(ul, []{ return NITEMS < CAP; })`, so it blocks when the
  buffer is full.
- Consumer: `wait(ul, []{ return NITEMS != 0; })`, so it blocks when the
  buffer is empty.
- Each does its work, then `ul.unlock()`, then `notify_all()`. This is the
  correct pattern, and the improved version of the 24th Aug busy-wait queue.
- Minor: `A[5]` is never used.

## Quick recap
| Need | Use |
|---|---|
| Simple scoped critical section | `lock_guard` (or `scoped_lock`) |
| Unlock or relock mid-scope, or a CV wait | `unique_lock` |
| Don't block, try and back off | `try_lock()` (beware spinning, starvation, livelock) |
| Same thread re-locks (recursion) | `recursive_mutex` (prefer redesigning) |
| Wait for a condition | `cv.wait(lock, predicate)`, **always with a predicate** |
| Waking others | Change state under the lock → unlock → `notify_one`/`notify_all` |
