// ====================== 23B1023 =========================
//
// test_tool_client.cpp -- Step 3 test: the GENERATED client stub (Demo_proxy)
// speaks the protocol. The server side is still hand-written here; Step 4
// makes tool.cpp generate it too.
//
// Build & run:
//   g++ -std=c++17 -Wall tool.cpp -o tool && ./tool demo.spec
//   g++ -std=c++17 -Wall -pthread test_tool_client.cpp Demo_proxy.cpp rpc_common.cpp rpc_client.cpp rpc_server.cpp -o test_tool_client
//   ./test_tool_client

#include "Demo_proxy.h"     // generated
#include "rpc_client.h"
#include "rpc_server.h"

#include <cassert>
#include <iostream>
#include <thread>

// ---------------- hand-written server side (generated in Step 4) ------------
static int resets = 0;
void exportDemoByHand() {
    addEntry("calculate:int:int", [](Packet& in) { Packet o; o.put(in.get<int>() / 10); return o; });
    addEntry("add:int:int:int", [](Packet& in) {
        int a1 = in.get<int>(); int a2 = in.get<int>(); Packet o; o.put(a1 + a2); return o; });
    addEntry("greet:std::string:std::string", [](Packet& in) {
        Packet o; o.put("hello " + in.get<std::string>()); return o; });
    addEntry("ping:int", [](Packet&) { Packet o; o.put(7); return o; });
    addEntry("reset:void", [](Packet&) { resets++; return Packet(); });   // void: empty result
}

int main() {
    int port = initRPC(0);
    exportDemoByHand();
    std::thread(serviceLoop).detach();
    setRpcServer("127.0.0.1", port);

    // Client code sees only the interface; the generated proxy does the RPC.
    Demo* d = new DemoProxy();
    assert(d->calculate(15000) == 1500);
    assert(d->add(2, 40) == 42);
    assert(d->greet("x:y z") == "hello x:y z");
    assert(d->ping() == 7);
    d->reset();
    d->reset();
    assert(d->ping() == 7 && resets == 2);   // ping() is a full round trip after the void calls

    std::cout << "calculate(15000) = " << d->calculate(15000) << "\n"
              << "add(2, 40)       = " << d->add(2, 40) << "\n"
              << "greet(\"x:y z\")   = " << d->greet("x:y z") << "\n"
              << "ping()           = " << d->ping() << "\n"
              << "reset() x2       -> server saw " << resets << " resets\n";
    delete d;
    std::cout << "ALL STEP 3 TESTS PASSED\n";
}
