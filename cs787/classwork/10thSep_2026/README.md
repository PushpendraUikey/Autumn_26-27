# 10th Sep 2026 — Test: Threads & Condition Variables

Three problems on thread synchronization, all solved with `std::mutex` +
`std::condition_variable`.

> **Note:** The problem statements below are **reconstructed from the
> solutions**. The original wording was not saved, so treat them as
> approximate.

| File | Problem | Pattern |
|---|---|---|
| `1.cpp` | Pause a printer thread based on a shared counter | Conditional gating / signalling |
| `2.cpp` | One coordinator + 4 workers in two ordered phases | Phase ordering (producer → workers → coordinator) |
| `3.cpp` | 3 workers must all reach a point before any proceeds | Barrier |

Build: `g++ -std=c++17 -pthread <file>.cpp && ./a.out`

---

## Problem 1 — Gate a thread on a counter (`1.cpp`)

**Problem (approx.):** Two threads share a `counter` (initially 0).
- Thread **g** increments `counter` every 300 ms forever and prints its
  value.
- Thread **f** loops forever printing `"Message1 : Before"`, sleeping
  100 ms, then printing `"Message2 : After"`.
- f must **not** run an iteration while `counter` is a multiple of 4. It
  must block and resume once g moves the counter past that value.

**Solution:**
- At the top of each iteration, f calls
  `cv.wait(ul, []{ return counter % 4 != 0; })`.
  - The lock is held only around the wait (inside its own `{}` scope).
  - The printing and sleeping happen *outside* the lock.
- g increments under a `lock_guard` and notifies only on the transition that
  unblocks f (`counter % 4 == 1`). It sets a `bool` inside the critical
  section and calls `notify_one()` **after** releasing the lock.
- Observed behaviour: f is silent from counter = 4 to 5, from 8 to 9, and so
  on. f checks the condition only at the start of an iteration, so an
  iteration that has already started finishes.
- Side note: `cout` isn't protected by the lock, so lines from f and g
  sometimes get mixed together in the output.

---

## Problem 2 — Two-phase coordination: f → 4×g → f (`2.cpp`)

**Problem (approx.):** One thread `f` and four worker threads `g` share
`counter = 0`.
1. **Phase 1:** f increments `counter` 4 times. No g may touch `counter`
   before this is done.
2. **Phase 2:** each g increments `counter`, sleeps 100 ms, then increments
   it again. The workers should run **concurrently**.
3. **Phase 3:** only after **all 4** workers finish, f doubles `counter` and
   prints the result.

Expected result: `(4 + 4×2) × 2 = 24`.

**Solution:**
- `cv_phase1` + flag `f_done`: f sets the flag and calls `notify_all()`.
  The workers wait on `f_done == 1`.
- `cv_phase2` + count `workers_done`: each g increments the count. The
  **last** worker (count == 4) calls `notify_one()`. f waits on
  `workers_done >= 4`, then doubles the counter.
- Workers sleep **without** holding the lock, so the whole run takes about
  100 ms instead of about 400 ms.
- Output: `Final counter value: 24`.

**First attempt (kept commented out in the file), with what was wrong:**
1. **Sleeping while holding the lock** forced the workers to run one at a
   time (about 400 ms).
2. **Using `sleep_for` in `main` to order f before g** is a race. Ordering
   must come from a condition variable or flag, not from timing.
3. **`cvG.wait(ul)` with no predicate** is exposed to **spurious wakeups**.
   Always pass a predicate.
4. **Calling notify while holding the lock** wakes a thread that then
   immediately blocks on the mutex ("hurry up and wait").
5. **Every g calling `notify_all()` again** after waking was redundant and
   added lock contention.

---

## Problem 3 — Barrier for 3 workers (`3.cpp`)

**Problem (approx.):** Three threads run the same function.
- Each first does some local work: it counts to 4 in a local variable.
- **No thread may update the shared `counter` until all three have finished
  their local work.** This is a barrier.
- After the barrier, each adds its local count to `counter`, sleeps briefly,
  and prints the value.
- Every worker should print the final total, `12`.

**Solution:**
- Each thread increments `workers_done` under the lock, then waits with
  `cv_phase1.wait(ul, []{ return workers_done == 3; })`.
- **Chained wake-up:**
  1. The last thread to arrive finds the condition already true and calls
     `notify_one()`.
  2. The thread it wakes calls `notify_one()` too, and so on, until every
     waiter is released.
  3. A single `notify_all()` by the last arriver would also work.
- After the barrier, each thread adds its count under a `lock_guard`, sleeps
  10 ms *outside* the lock, and then prints.
- Output: `Worker has counter: 12` three times. The 10 ms sleep makes it very
  likely that all additions are done before anyone prints, but this is not
  strictly guaranteed.

---

## Takeaways / rules followed (from the notes in `2.cpp`)

- **Never sleep while holding a lock.** Keep critical sections to the shared
  variables only.
- **Always wait with a predicate:** `cv.wait(lock, pred)`. It handles
  spurious wakeups and the case where the notify happened before the wait.
- **Decide inside the lock, notify outside it.** Set a local `bool` in the
  critical section, then call `notify_*()` after the scope ends.
- **Order threads with condition variables and flags, never with
  `sleep_for`.**
- `unique_lock` is required for `cv.wait()` because the wait must
  unlock and relock. `lock_guard` is enough for plain critical sections.
- `notify_one()` when exactly one waiter needs to wake (coordinator).
  `notify_all()` when everyone waiting must proceed (phase start, barrier).
