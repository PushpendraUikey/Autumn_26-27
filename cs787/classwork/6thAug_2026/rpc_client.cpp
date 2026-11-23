// ====================== 23B1023 =========================
//
// rpc_client.cpp -- connect, send one request frame, read one reply frame.

#include "rpc_client.h"

#include <arpa/inet.h>
#include <cerrno>
#include <cstring>
#include <sys/socket.h>
#include <unistd.h>

static std::string g_serverIp;     // set by setRpcServer()
static int g_serverPort = -1;

void setRpcServer(const std::string& ip, int port) {
    g_serverIp = ip;
    g_serverPort = port;
}

Packet Rpc(const std::string& signature, const Packet& args) {
    if (g_serverPort < 0)
        throw RpcError("Rpc: no server set, call setRpcServer(ip, port) first");

    // 1. Request payload = field(signature) followed by the packed arguments.
    Packet header;
    header.put(signature);
    std::string request = header.raw() + args.raw();

    // 2. Connect (one connection per call, like the slides' serviceloop).
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) throw RpcError(std::string("Rpc: socket: ") + strerror(errno));

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(g_serverPort);
    if (inet_pton(AF_INET, g_serverIp.c_str(), &addr.sin_addr) != 1) {
        close(sock);
        throw RpcError("Rpc: bad server address " + g_serverIp);
    }
    if (connect(sock, (sockaddr*)&addr, sizeof(addr)) < 0) {
        std::string why = strerror(errno);
        close(sock);
        throw RpcError("Rpc: cannot connect to " + g_serverIp + ":" +
                       std::to_string(g_serverPort) + " (" + why + ")");
    }

    // 3. Send the request, wait for the reply. Close the socket even on error.
    std::string replyRaw;
    try {
        sendFrame(sock, request);
        replyRaw = recvFrame(sock);
    } catch (...) {
        close(sock);
        throw;
    }
    close(sock);

    // 4. Reply = field("OK") field(result)  |  field("ERR") field(message)
    Packet reply(replyRaw);
    std::string status = reply.get<std::string>();
    if (status == "ERR")
        throw RpcError("remote error: " + reply.get<std::string>());
    if (status != "OK")
        throw RpcError("Rpc: malformed reply status \"" + status + "\"");
    return reply;   // cursor now sits on the result field
}
