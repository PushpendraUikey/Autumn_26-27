// ====================== 23B1023 =========================
//
// rpc_server.h -- reusable SERVER side of the RPC runtime ("RpcServer.c",
// slide 39: reusable for all clients). Hand-written once, never generated.
//
// Server main() from slide 34:
//     initRPC(port);
//     export_<Service>(&impl);   // generated: calls addEntry() once per method
//     serviceLoop();             // never returns

#ifndef RPC_SERVER_H
#define RPC_SERVER_H

#include "rpc_common.h"

#include <functional>
#include <string>

// A server stub (slide 39 "Stub_foo(byte *packet)"): unpacks the arguments
// from `args`, calls the real method, and returns the packed result.
using ServerStub = std::function<Packet(Packet& args)>;

// Slide 35 "Rpcinit: create serverport". Port 0 lets the OS choose a free
// port. Returns the port actually bound. Throws RpcError on failure.
int initRPC(int port);

// Slide 37 "addentry("foo", stub_foo, "int:int")". The key is the FULL
// signature ("name:ret:argtypes"), so a client built from a different spec
// gets "unknown method" instead of the server misreading its arguments.
void addEntry(const std::string& signature, ServerStub stub);

// Slide 38: look the stub up and run it. Throws RpcError if unknown.
Packet dispatch(const std::string& signature, Packet& args);

// Slide 35 serviceloop: accept, receive request, dispatch, send reply, repeat.
// Never returns. A failing call is answered with ERR; the server keeps going.
[[noreturn]] void serviceLoop();

#endif
