# 3rd Sep 2026 — `lock_guard`, `unique_lock`, `recursive_mutex` & `try_lock`

The lecture introduced the RAII mutex wrappers, locking inside recursive
functions (`recursive_mutex` vs unlocking before the recursive call), and
non-blocking `try_lock`. The 7th Sep lecture builds on this.

## Files

| File | Topic | Observed |
|---|---|---|
| `lockGuards.cpp` | `lock_guard<mutex>`: scoped lock, no manual control | `f`'s 4 lines, then `g`'s 4 lines (whole block serialised) |
| `uniqueLock.cpp` | `unique_lock<mutex>`: `unlock()`/`lock()` allowed mid-scope | Same as above: the unlock window at `i==2` is too short for `g` to get in |
| `recursiveMutex.cpp` | `unique_lock<recursive_mutex>` in recursive methods | One thread prints all of 11…1 |
| `recursiveMutexSimple.cpp` | Raw `recursive_mutex` `lock()`/`unlock()` in recursion | One thread (`T1` or `T2`) prints all of 11…1 |
| `recursiveUniqueLock.cpp` | Plain `mutex`, unlocked **before** recursing | Threads alternate `f`/`g`, but there is a race (below) |
| `trylock.cpp` | `try_lock()` + retry with sleep | `f() has acquired…`, `g() is waiting…`, `g() has acquired…` |

Build: `g++ -std=c++17 -pthread <file>.cpp && ./a.out`

---

## Key concepts

### `lock_guard`: simplest RAII lock
- It locks in the constructor and unlocks in the destructor at the end of
  the scope, including on an early return or an exception.
- It has **no `lock()`/`unlock()` members**. Uncommenting `lg.unlock()` is a
  **compile error** ("has no member named 'unlock'"), not a runtime error.
- Lines printed *before* the `lock_guard` (`"f() is starting"`) are not
  protected, so they interleave with the other thread's output.

### `unique_lock`: flexible RAII lock
- Same automatic unlock as `lock_guard`, plus `unlock()`, `lock()`,
  `try_lock()`, `defer_lock`, `owns_lock()`, and it can be moved.
- `unlock()` immediately followed by `lock()` opens a window too small for
  the other thread to use. To actually let the other thread run, release the
  lock **around real work or a sleep**.
- It costs slightly more than `lock_guard`, because it has to remember
  whether it currently owns the lock. It is needed for condition variables
  (from 7th Sep).

### `recursive_mutex`
- A plain `mutex` locked again by the **same thread** is undefined
  behaviour, typically a self-deadlock. That is a problem in recursive
  functions.
- `recursive_mutex` keeps an **owner and a lock count**:
  - each `lock()` by the owner increases the count
  - each `unlock()` decreases it
  - the mutex is free again when the count reaches 0
- With **`unique_lock<recursive_mutex>`** (`recursiveMutex.cpp`), each
  recursion level's lock is released automatically as the calls return.
- With a **raw** `lock()`/`unlock()` (`recursiveMutexSimple.cpp`), every
  `lock()` needs its own matching `unlock()`, including the one after the
  recursive call. Otherwise the count never reaches 0 and the other thread
  waits forever.
- **Downside:** the lock is held across the whole recursion, so the **first
  thread does all the work** and the other thread only finds `counter == 0`.
  This gives exclusivity, not parallelism.

### Alternative: plain `mutex`, unlock before recursing (`recursiveUniqueLock.cpp`)
- The code does `lock` → `counter--` → `unlock` → sleep → recurse. There is
  no self-deadlock, and the threads take turns.
- ⚠️ **Race condition:**
  - the check `if (counter <= 0)` and the `cout << counter` happen
    **before** the lock is taken
  - both threads can read the same value, so `"value 11"` was printed by both
    in the run above
  - in **36 of 200 runs**, 12 lines were printed instead of 11
- **Fix:** do the check, the read and the decrement all **under the lock**,
  then unlock and recurse.
- **Lesson:** a check-then-act sequence must be inside a single critical
  section.

### `try_lock()`
- It **doesn't block**. It returns `true` if it got the lock and `false` if
  the lock is busy.
- The retry loop `while (!m.try_lock()) { … sleep … }` lets a thread do
  something else, or give up, instead of waiting forever. This is a building
  block for avoiding deadlock (dining philosophers, 7th Sep).
- Retrying without sleeping is a busy spin that burns CPU. Retrying with a
  long sleep (3 s here) adds delay.
- A successful `try_lock()` must be followed by `unlock()`, or the lock can
  be adopted by a `unique_lock`. `unique_lock`'s own `try_lock()` handles
  this with RAII.

## Quick recap
| Need | Use |
|---|---|
| Lock for a whole scope | `lock_guard<mutex>` |
| Unlock or relock inside the scope, or condition variables | `unique_lock<mutex>` |
| Same thread locks again (recursion) | `recursive_mutex`, ideally through `unique_lock`/`lock_guard` |
| Recursion with other threads allowed in between | Plain `mutex`, released before the recursive call, with the **whole** check-and-update under the lock |
| Don't block if the lock is busy | `try_lock()` + retry/back-off |
