// ====================== 23B1023 =========================
//
// rpc_server.cpp -- listening socket, dispatch table, service loop.

#include "rpc_server.h"

#include <arpa/inet.h>
#include <cerrno>
#include <cstring>
#include <iostream>
#include <map>
#include <sys/socket.h>
#include <unistd.h>

static int g_listenSock = -1;
static std::map<std::string, ServerStub> g_table;   // signature -> stub

int initRPC(int port) {
    g_listenSock = socket(AF_INET, SOCK_STREAM, 0);
    if (g_listenSock < 0) throw RpcError(std::string("initRPC: socket: ") + strerror(errno));

    int opt = 1;   // allow restarting the server right away on the same port
    setsockopt(g_listenSock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = INADDR_ANY;
    if (bind(g_listenSock, (sockaddr*)&addr, sizeof(addr)) < 0)
        throw RpcError("initRPC: bind to port " + std::to_string(port) + ": " + strerror(errno));
    if (listen(g_listenSock, 16) < 0)
        throw RpcError(std::string("initRPC: listen: ") + strerror(errno));

    socklen_t len = sizeof(addr);   // find the real port (matters when port == 0)
    getsockname(g_listenSock, (sockaddr*)&addr, &len);
    return ntohs(addr.sin_port);
}

void addEntry(const std::string& signature, ServerStub stub) {
    g_table[signature] = std::move(stub);
}

Packet dispatch(const std::string& signature, Packet& args) {
    auto it = g_table.find(signature);
    if (it == g_table.end())
        throw RpcError("unknown method " + signature);
    return it->second(args);
}

// Handle one connection: one request in, one reply out.
static void serveOne(int client) {
    std::string request;
    try {
        request = recvFrame(client);
    } catch (const RpcError& e) {
        std::cerr << "serviceLoop: " << e.what() << "\n";   // client vanished: nothing to answer
        return;
    }

    Packet reply;
    try {
        Packet req(request);
        std::string signature = req.get<std::string>();
        Packet result = dispatch(signature, req);
        reply.put("OK");
        reply = Packet(reply.raw() + result.raw());
    } catch (const std::exception& e) {
        // Unknown method, bad arguments, or the user's method threw:
        // tell the client instead of crashing the server.
        reply = Packet();
        reply.put("ERR");
        reply.put(std::string(e.what()));
    }

    try {
        sendFrame(client, reply.raw());
    } catch (const RpcError& e) {
        std::cerr << "serviceLoop: " << e.what() << "\n";
    }
}

void serviceLoop() {
    if (g_listenSock < 0) throw RpcError("serviceLoop: call initRPC() first");
    while (true) {
        int client = accept(g_listenSock, nullptr, nullptr);
        if (client < 0) continue;
        serveOne(client);
        close(client);
    }
}
