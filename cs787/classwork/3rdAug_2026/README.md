# 3rd Aug 2026 — Proxy Pattern & Remote Proxy over TCP Sockets

This lecture introduced the **Proxy** design pattern. A proxy implements the
same interface as the real object and forwards calls to it, so the client
can't tell the two apart. The idea was then taken across the network as a
**remote proxy** using TCP sockets. This is the base for the RPC assignment
(13th Aug).

## Files

| File | What it does |
|---|---|
| `TAXCalculator.cpp` | Proxy pattern in one process: the proxy forwards `calculate()` to the real object through a pointer. Prints `1500` (10% of 15000) |
| `server.cpp` / `client.cpp` | Plain TCP warm-up: the client sends a message, the server replies `"Got your message!"`, and `q` ends the session |
| `TaxServer.cpp` | The real tax calculator (slab rates) behind a TCP server on port 8080 |
| `TaxClient.cpp` | Remote proxy: `calculate(income)` sends the income to the server and returns the tax it gets back |
| `proxy_design_pattern.cpp` | GoF sample: `ImageProxy` (virtual proxy) and `ImagePtr` (smart-pointer proxy through `operator->`). It is a reference excerpt and doesn't compile on its own |

## Build & run

```bash
g++ TaxServer.cpp -o server && ./server        # terminal 1
g++ TaxClient.cpp -o client && ./client        # terminal 2, enter incomes, -1 to quit
```

Example: 5,00,000 → `0`, 20,00,000 → `300000`, 50,00,000 → `1000000`.

`client.cpp` connects to the lab machine `10.51.2.131`. Change it to
`127.0.0.1` to test on one machine.

---

## Key concepts

### Proxy pattern (GoF, structural)

> **Intent:** *Provide a surrogate or placeholder for another object to
> control access to it.*

| Role | Tax example | GoF example |
|---|---|---|
| **Subject**: common interface, so a proxy can be used wherever the real object is expected | `TAXCalculator` | `Graphic` |
| **RealSubject**: the actual object | `TAXCalculatorReal` | `Image` (expensive to load) |
| **Proxy**: same interface, holds a reference to the real object, controls access, and forwards calls | `TAXCalculatorProxy` | `ImageProxy` |

The client is written against the Subject only, so a proxy can replace the
real object without any change to client code.

**GoF's four kinds of proxy:**
- **Remote proxy:** a local stand-in for an object in another address space
  or on another machine. This is `TaxClient.cpp`.
- **Virtual proxy:** creates an expensive object **only when it is first
  needed**.
  - `ImageProxy` stores just the file name.
  - `GetImage()` loads the `Image` on the first `Draw()`/`HandleMouse()`.
  - `GetExtent()` is cached, so later calls don't load the image again.
  - The document can insert `new ImageProxy("file")` without loading
    anything.
- **Protection proxy:** checks access rights before forwarding a call.
- **Smart reference:** adds extra work on each access, such as reference
  counting, locking, or loading on first use.
  - `ImagePtr` overloads **`operator->` and `operator*`**, so
    `image->Draw(...)` loads the image transparently.
  - This is how smart pointers like `std::shared_ptr` work.

**Consequences:** the proxy adds a level of indirection, which can:
- hide where the object lives (remote)
- optimise on demand (virtual), including **copy-on-write**
- add checks or housekeeping (protection, smart reference)

The cost is an extra layer, and any call that isn't forwarded correctly
becomes a bug.

**Related:** **Decorator** has the same structure but *adds
responsibilities*, while a proxy *controls access*. **Adapter** provides a
*different* interface, while a proxy keeps the *same* one.

### Remote proxy
- The proxy's `calculate()` does no tax logic. Instead it:
  1. **Marshals** the argument: income → text.
  2. Sends it over a socket.
  3. Waits for the reply.
  4. **Unmarshals** the result: text → `int`.
- The server side receives the request, calls the real object, and sends the
  result back.
- To the client it looks like a normal local method call. **This is the core
  idea of RPC**, where the proxy is the client stub and the server loop is
  the server stub (skeleton).

### TCP sockets (POSIX)
| Server | Client |
|---|---|
| `socket(AF_INET, SOCK_STREAM, 0)` | `socket(...)` |
| `bind()` to a port (`INADDR_ANY`, `htons(8080)`) | `inet_pton()` turns the IP text into an address |
| `listen(fd, backlog)` | `connect()` |
| `accept()` returns a new socket for the client | |
| `recv()` / `send()` loop | `send()` / `recv()` |
| `close()` both sockets | `close()` (done in the proxy's destructor, RAII) |

- `htons()` converts the port to **network byte order**.
- **A simple protocol:** messages are plain text, numbers are sent as text,
  and `q` (or a closed connection, where `recv()` returns `<= 0`) ends the
  session.

## Limitations (fixed later)
- **The client and server don't share one interface.** `TaxClient`'s proxy
  doesn't inherit from `TAXCalculator`, so the two sides can drift apart.
  The 13th Aug version uses a shared header and **generated** proxy and stub
  code.
- **No error checking.** Results of `socket`, `bind`, `connect` and `send`
  are not checked.
- **Assumes one message per `recv()`.** TCP is a byte stream, so a real
  protocol needs framing: a delimiter or a length prefix.
- **The server handles one client, then exits.** There is no concurrency.
- **Missing virtual destructor.** `TAXCalculator` doesn't have one, so the
  compiler warns on `delete` through a pointer to the class.
