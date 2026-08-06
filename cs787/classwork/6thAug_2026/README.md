# 23B1023: RPC Assignment — tool / stuff / proxy

## How the pieces fit together

This follows the design built up in the "Building an RPC infrastructure"
lecture (slides 16–26), applied to the `TAXCalculator` example from
`abstract.cpp` / `abstract1.cpp`.

```
service.spec  --(read by)-->  tool.cpp  --(writes)-->  proxy.h/proxy.cpp
                                        --(writes)-->  stuff.h/stuff.cpp

client.cpp  --includes-->  TAXCalculator.h + proxy.h  --links-->  proxy.cpp
server.cpp  --includes-->  TAXCalculator.h + stuff.h  --links-->  stuff.cpp
```

| File | Role | Hand-written or generated? |
|---|---|---|
| `TAXCalculator.h` | The abstract interface shared by client and server (same as `abstract.cpp`) | hand-written |
| `service.spec` | Tiny IDL: service name, port, method signature | hand-written |
| `tool.cpp` | The stub generator (like `rpcgen`) | hand-written |
| `proxy.h` / `proxy.cpp` | Client-side stub: `TAXCalculatorProxy`. Fills in the `// socket calls here` TODO from `abstract1.cpp` — marshals the call, sends it over TCP, unmarshals the reply | **generated** by `tool.cpp` |
| `stuff.h` / `stuff.cpp` | Server-side skeleton: `serve_TAXCalculator()`. Accepts connections, unmarshals the request, dispatches to the real object, marshals the reply | **generated** by `tool.cpp` |
| `client.cpp` | Creates a `TAXCalculatorProxy`, points it at the server, calls `calculate()` exactly like a local call | hand-written |
| `server.cpp` | Defines `TAXCalculatorReal` (the actual `0.1 * income` logic) and hands it to the generated dispatcher | hand-written |

## Why split it this way

This mirrors the "reusable part of client stub" idea from slide 21 and the
bind/dispatch server from slides 23–24: the parts that talk sockets and do
marshalling/unmarshalling are mechanical and can be generated from a
description of the interface, while the parts that are actually
interesting (what `calculate()` does, what income to tax) stay
hand-written in `client.cpp`/`server.cpp`.

## Wire protocol (kept intentionally simple)

Request:  `"<methodName>:<arg>\n"`  e.g. `"calculate:1000\n"`
Response: `"<result>"`  e.g. `"100"`

## Build & run

```
make            # builds tool, regenerates proxy/stuff, builds server_bin & client_bin
./server_bin &  # start the server (listens on port 9090)
./client_bin    # run the client -> prints "Tax on income 1000 = 100"
```

`make clean` removes the generated files and binaries so you can confirm
the generator step actually runs from scratch.

## Extending it

`service.spec` currently declares one method. To add another RPC method,
add another `method <name> <retType> <argType>` line and re-run `make` —
`tool.cpp` loops over all declared methods when generating both the proxy
and the server dispatcher, so no other file needs to change by hand.

## Note on the AI-usage report

This project (and this README) were built with Claude's help
