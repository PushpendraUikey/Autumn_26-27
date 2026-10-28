# 24th Aug 2026 — Threads: Basics & Producer–Consumer

Intro to `std::thread`: creating and joining threads, running class member
functions as threads, and a producer–consumer on a shared circular queue,
improved step by step with `sleep`, `yield`, and finally a `mutex`.

## Files

| File | Topic |
|---|---|
| `thread_ex.cpp` | Create two threads from free functions, then `join()` them |
| `thread_from_class.cpp` | Thread from a member function. Producer–consumer with no synchronisation |
| `thread_from_class_sleep.cpp` | + `sleep_for` in the producer, + `get_id()` |
| `thread_from_class_sleep_yield.cpp` | + `this_thread::yield()` while the consumer waits |
| `thread_from_class_sleep_yield_mutex.cpp` | + `std::mutex` protecting the shared queue |

Build: `g++ -pthread <file>.cpp && ./a.out`. The producer–consumer programs
loop forever, so stop them with Ctrl+C.

---

## Key concepts

### Creating and joining threads (`thread_ex.cpp`)
- `thread t1(f);` starts running `f` on a new thread straight away.
- `t.join()` makes the caller **wait until that thread finishes**. Without
  it, `main` could exit while the threads are still running.
- A `std::thread` that is still joinable when it is destroyed calls
  `std::terminate`. Always `join()` (or `detach()`).
- The output order is **not deterministic**. `"Finished main"` can print
  before or after `Hello`/`World`.

### Threads from class member functions
- `thread t(&Class::method, objPtr);` passes the member-function pointer
  plus the object to call it on.
- Shown with `A::f` / `B::g` (commented out) and with
  `Producer::produce` / `Consumer::consume`.
- `t.get_id()` gives each thread a unique ID.

### Producer–consumer setup
- `Cirqueue<T>`: a fixed-size (10) **circular queue** template. It holds at
  most 9 items, because one slot is kept empty to tell "full" apart from
  "empty".
- **Producer:** fills the queue with `Event(ctr++)` until it is full.
- **Consumer:** waits while the queue is empty, then pops and prints one
  event.
- Both threads share a global `Cirqueue<Event>* q`. This shared state is
  where the trouble starts.

### Step-by-step evolution
| Version | Producer | Consumer waits by | What it shows |
|---|---|---|---|
| `thread_from_class` | Refills nonstop (busy loop) | `sleep_for(100ms)` | Producer burns CPU, output floods (~440k lines in 2.5 s) |
| `_sleep` | Refills, then `sleep_for(5s)` | Empty spin loop (busy-wait) | Bursts of 9 events every 5 s; consumer burns CPU while it waits |
| `_sleep_yield` | Refills, then `sleep_for(1s)` | `this_thread::yield()` | `yield` is a **hint** to the scheduler to let other threads run. Still a busy-wait, just a politer one |
| `_sleep_yield_mutex` | `lock()` → refill → `unlock()` → sleep 1 s | Unlock → sleep 100 ms → relock | The queue is only touched while holding the lock. Consumer prints one event every 500 ms |

### Race condition and mutex
- In the first three versions, both threads read and write `front_index`
  and `rear` with **no synchronisation**. This is a **data race**, which is
  undefined behaviour. It can show up as lost or duplicated events, or a
  corrupted queue.
- `std::mutex`: `lock()` / `unlock()` around the **critical section**, so
  only one thread is inside at a time.
- The consumer **releases the lock while it waits** (unlock → sleep →
  relock). Otherwise the producer could never get the lock to refill the
  queue.
- Keep sleeps **outside** the lock.

### Sleep vs yield vs busy-wait
- **Busy-wait** (an empty `while`): wastes a whole CPU core.
- **`yield()`**: gives up the current time slice, but the thread is
  rescheduled right away. It is still polling.
- **`sleep_for(d)`**: the thread really blocks for `d`. That saves CPU but
  adds delay.
- None of these is ideal. The proper fix (covered later) is a
  **condition variable**: block until notified.

## Things to watch in the code
- In `_sleep.cpp` and `_sleep_yield.cpp`, the consumer's `sleep_for` sits
  **after** an infinite `while(true)`, so it never runs.
- Raw `lock()`/`unlock()` is fragile, because an early return or exception
  skips the unlock. Prefer `lock_guard` or `unique_lock` (RAII).
- The objects created with `new` (`q`, `p`, `c`) are never deleted. That
  doesn't matter here because the program never ends.

## Quick recap
1. `std::thread` runs a function concurrently. Always `join()` it.
2. Member functions: `thread(&C::m, obj)`.
3. Shared data without a lock is a data race.
4. Use a `mutex` around the critical section, and release it while waiting.
5. Busy-wait < `yield` < `sleep` for CPU use. A condition variable is the
   real solution.
