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

### Proxy pattern
- **Subject** (`TAXCalculator`): the abstract interface that both the real
  object and the proxy implement.
- **RealSubject** (`TAXCalculatorReal`): does the actual work.
- **Proxy** (`TAXCalculatorProxy`):
  - has the same interface as the real object
  - holds a reference to it (`setReal()`)
  - forwards each call, and can add behaviour around it
- The client is written against the interface only, so the real object can
  be swapped for a proxy without changing any client code.
- Types of proxy: **remote** (the real object is on another machine),
  **virtual** (create it lazily), **protection** (access control),
  **caching/logging**.

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
