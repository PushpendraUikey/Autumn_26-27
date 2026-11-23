// ====================== 23B1023 =========================
//
// rpc_client.h -- reusable CLIENT side of the RPC runtime ("RpcClient.c",
// slide 32: reusable for all servers). Hand-written once, never generated.
// Generated client stubs call Rpc(); user code calls setRpcServer().

#ifndef RPC_CLIENT_H
#define RPC_CLIENT_H

#include "rpc_common.h"

#include <string>

// Slide 31 "setrpcserver(servip, port)": where the next Rpc() calls go.
// Stored as a side effect (a global), exactly as the slides describe.
void setRpcServer(const std::string& ip, int port);

// Slide 30 "Rpc(name, server, port, ...)": one remote call.
//   signature : "name:ret:argtypes", e.g. "calculate:int:int"
//   args      : the arguments, already packed by the stub in declaration order
// Returns the reply packet positioned at the result (the stub does get<Ret>()).
// Throws RpcError if the server can't be reached or answers ERR.
Packet Rpc(const std::string& signature, const Packet& args);

#endif
