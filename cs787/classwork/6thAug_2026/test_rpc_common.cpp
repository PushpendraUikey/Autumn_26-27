// ====================== 23B1023 =========================
//
// test_rpc_common.cpp -- Step 1 test: Packet (marshalling) + framing.
// Build: g++ -std=c++17 -Wall test_rpc_common.cpp rpc_common.cpp -o test_rpc_common

#include "rpc_common.h"

#include <cassert>
#include <iostream>
#include <sys/socket.h>
#include <unistd.h>

int main() {
    // 1. Round trip in a fixed order: what the client puts, the server gets back.
    Packet req;
    req.put(std::string("pushData:int:std::string:int"));   // signature first
    req.put(std::string("a:b c\nd"));                         // ':' space newline survive
    req.put(42);
    std::cout << "encoded request : " << req.raw() << "\n";

    Packet in(req.raw());                                    // what the server receives
    assert(in.get<std::string>() == "pushData:int:std::string:int");
    assert(in.get<std::string>() == "a:b c\nd");
    assert(in.get<int>() == 42);
    assert(in.atEnd());
    std::cout << "round trip      : OK\n";

    // 2. Wrong type: "hello" cannot be read as an int.
    Packet bad;
    bad.put("hello");
    try { Packet(bad.raw()).get<int>(); assert(false); }
    catch (const RpcError& e) { std::cout << "type mismatch   : " << e.what() << "\n"; }

    // 3. Missing argument: reading more fields than were sent.
    try { Packet empty; empty.get<int>(); assert(false); }
    catch (const RpcError& e) { std::cout << "missing field   : " << e.what() << "\n"; }

    // 4. Framing over a real connected socket pair.
    int sv[2];
    socketpair(AF_UNIX, SOCK_STREAM, 0, sv);
    sendFrame(sv[0], req.raw());
    assert(recvFrame(sv[1]) == req.raw());
    std::cout << "frame           : OK\n";

    // 5. A frame that arrives in two pieces must still be read as ONE message.
    std::string whole = "5\nhello";
    send(sv[0], whole.data(), 4, 0);          // "5\nhe"
    send(sv[0], whole.data() + 4, 3, 0);      // "llo"
    assert(recvFrame(sv[1]) == "hello");
    std::cout << "split frame     : OK\n";

    // 6. Peer closes before sending: a clear error, not a hang or garbage.
    close(sv[0]);
    try { recvFrame(sv[1]); assert(false); }
    catch (const RpcError& e) { std::cout << "closed peer     : " << e.what() << "\n"; }
    close(sv[1]);

    std::cout << "ALL STEP 1 TESTS PASSED\n";
}
