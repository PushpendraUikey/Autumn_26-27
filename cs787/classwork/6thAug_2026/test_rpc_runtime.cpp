// ====================== 23B1023 =========================
//
// test_rpc_runtime.cpp -- Step 2 test: client + server runtime over real TCP.
// The stubs below are HAND-WRITTEN here; Steps 3-4 make tool.cpp generate them.
// Build:
//   g++ -std=c++17 -Wall -pthread test_rpc_runtime.cpp rpc_common.cpp rpc_client.cpp rpc_server.cpp -o test_rpc_runtime

#include "rpc_client.h"
#include "rpc_server.h"

#include <cassert>
#include <iostream>
#include <thread>

// ---------------- what the USER writes (the real methods) -------------------
int calculate(int income) { return income / 10; }
int add(int a, int b) { return a + b; }
std::string greet(std::string name) {
    if (name.empty()) throw std::invalid_argument("name must not be empty");
    return "hello " + name;
}

// ---------------- server stubs (will be GENERATED) --------------------------
// unpack args in declaration order -> call the real method -> pack the result
Packet stub_calculate(Packet& in) { int a1 = in.get<int>(); Packet out; out.put(calculate(a1)); return out; }
Packet stub_add(Packet& in) { int a1 = in.get<int>(); int a2 = in.get<int>(); Packet out; out.put(add(a1, a2)); return out; }
Packet stub_greet(Packet& in) { std::string a1 = in.get<std::string>(); Packet out; out.put(greet(a1)); return out; }

void export_Demo() {   // slide 37 addentry, once per method
    addEntry("calculate:int:int", stub_calculate);
    addEntry("add:int:int:int", stub_add);
    addEntry("greet:std::string:std::string", stub_greet);
}

// ---------------- client stubs (will be GENERATED) --------------------------
// pack args in declaration order -> Rpc(signature) -> unpack the result
namespace remote {
int calculate(int a1) { Packet p; p.put(a1); return Rpc("calculate:int:int", p).get<int>(); }
int add(int a1, int a2) { Packet p; p.put(a1); p.put(a2); return Rpc("add:int:int:int", p).get<int>(); }
std::string greet(std::string a1) { Packet p; p.put(a1); return Rpc("greet:std::string:std::string", p).get<std::string>(); }
}

int main() {
    // 0. Calling before setRpcServer() is a clear error.
    try { remote::calculate(1); assert(false); }
    catch (const RpcError& e) { std::cout << "no server set  : " << e.what() << "\n"; }

    // 1. Server: initRPC on a free port, export, serviceLoop in the background.
    int port = initRPC(0);
    export_Demo();
    std::thread(serviceLoop).detach();
    std::cout << "server on port : " << port << "\n";

    // 2. Client: looks like ordinary local calls.
    setRpcServer("127.0.0.1", port);
    assert(remote::calculate(15000) == 1500);
    assert(remote::add(2, 40) == 42);                       // 2 arguments, order kept
    assert(remote::greet("a:b c") == "hello a:b c");         // string with ':' and space
    std::cout << "calculate      : " << remote::calculate(15000) << "\n";
    std::cout << "add(2, 40)     : " << remote::add(2, 40) << "\n";
    std::cout << "greet          : " << remote::greet("a:b c") << "\n";

    // 3. The user's method throws -> ERR reply, the server stays up.
    try { remote::greet(""); assert(false); }
    catch (const RpcError& e) { std::cout << "method threw   : " << e.what() << "\n"; }

    // 4. A client from a different spec (signature not exported) -> unknown method.
    try { Packet p; p.put(1.5); Rpc("calculate:int:double", p); assert(false); }
    catch (const RpcError& e) { std::cout << "wrong signature: " << e.what() << "\n"; }

    // 5. Too few arguments for the signature -> server stub fails to unpack.
    try { Packet p; p.put(1); Rpc("add:int:int:int", p); assert(false); }
    catch (const RpcError& e) { std::cout << "missing arg    : " << e.what() << "\n"; }

    // 6. Server still alive after all the errors.
    assert(remote::add(1, 1) == 2);
    std::cout << "still serving  : add(1, 1) = 2\n";

    // 7. Nobody listening on the port -> connection error, not a hang.
    setRpcServer("127.0.0.1", 1);
    try { remote::add(1, 1); assert(false); }
    catch (const RpcError& e) { std::cout << "no listener    : " << e.what() << "\n"; }

    std::cout << "ALL STEP 2 TESTS PASSED\n";
}
