// ====================== 23B1023 =========================
//
// rpc_common.cpp -- framing: send/receive one whole message on a TCP socket.

#include "rpc_common.h"

#include <sys/socket.h>

// send() may write fewer bytes than asked, so loop until everything is out.
static void sendAll(int sock, const std::string& data) {
    size_t sent = 0;
    while (sent < data.size()) {
        ssize_t n = send(sock, data.data() + sent, data.size() - sent, 0);
        if (n <= 0) throw RpcError("sendFrame: connection lost while sending");
        sent += n;
    }
}

void sendFrame(int sock, const std::string& payload) {
    sendAll(sock, std::to_string(payload.size()) + "\n" + payload);
}

std::string recvFrame(int sock) {
    // 1. Length line, one byte at a time up to '\n' (it is only a few digits).
    std::string lenText;
    char c;
    while (true) {
        ssize_t n = recv(sock, &c, 1, 0);
        if (n <= 0) throw RpcError("recvFrame: connection closed before a message arrived");
        if (c == '\n') break;
        if (c < '0' || c > '9') throw RpcError("recvFrame: bad frame header");
        lenText += c;
    }
    if (lenText.empty()) throw RpcError("recvFrame: empty frame header");
    size_t len = std::stoul(lenText);

    // 2. Exactly `len` payload bytes; recv() may return them in several pieces.
    std::string payload(len, '\0');
    size_t got = 0;
    while (got < len) {
        ssize_t n = recv(sock, &payload[got], len - got, 0);
        if (n <= 0) throw RpcError("recvFrame: connection closed in the middle of a message");
        got += n;
    }
    return payload;
}
