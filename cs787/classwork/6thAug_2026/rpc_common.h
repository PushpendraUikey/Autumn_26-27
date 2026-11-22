// ====================== 23B1023 =========================
//
// rpc_common.h -- the RPC PROTOCOL shared by client and server.
// Hand-written once, reused by every service (not generated).
//
// ---------------------------------------------------------------------------
// Wire format
// ---------------------------------------------------------------------------
// Every value is one FIELD:   <length>:<bytes>      e.g. 5:hello   3:100
//   - length-prefixed, so a string may contain ':', spaces or newlines.
//   - non-string values travel as text (100 -> "100").
//
// REQUEST payload  = field(signature) field(arg1) field(arg2) ...
//   signature = "name:ret:arg1type:arg2type..."   e.g. "pushData:int:std::string"
//   (the class convention "foo:int:int:int": name first, then the return
//    type, then the argument types in declaration order)
//
// RESPONSE payload = field("OK")  field(result)
//                  | field("ERR") field(error message)
//
// FRAME on the TCP socket = <payload length>\n<payload>
//   TCP is a byte stream, so the receiver first reads the length line and
//   then exactly that many bytes. One recv() is NOT guaranteed to be one message.
// ---------------------------------------------------------------------------

#ifndef RPC_COMMON_H
#define RPC_COMMON_H

#include <sstream>
#include <stdexcept>
#include <string>
#include <type_traits>

// Any RPC failure: connection problems, unknown method, bad packet, remote error.
class RpcError : public std::runtime_error {
public:
    using std::runtime_error::runtime_error;
};

// A Packet is an ordered list of fields. put() appends, get() reads the next one.
// Both sides MUST put/get in the same order: that order is the convention
// ("which argument belongs to which parameter").
class Packet {
public:
    Packet() = default;
    explicit Packet(std::string raw) : buf(std::move(raw)) {}

    // ---- marshal -----------------------------------------------------------
    template <typename T>
    void put(const T& value) {
        if constexpr (std::is_same_v<T, std::string>) {
            putField(value);
        } else {
            std::ostringstream os;
            os << value;
            putField(os.str());
        }
    }
    void put(const char* s) { putField(s); }   // so put("OK") is a string, not a pointer

    // ---- unmarshal ---------------------------------------------------------
    template <typename T>
    T get() {
        std::string field = getField();
        if constexpr (std::is_same_v<T, std::string>) {
            return field;
        } else {
            std::istringstream is(field);
            T value{};
            if (!(is >> value) || is.peek() != EOF)
                throw RpcError("Packet: field \"" + field + "\" is not a valid value of the expected type");
            return value;
        }
    }

    bool atEnd() const { return pos >= buf.size(); }
    const std::string& raw() const { return buf; }

private:
    std::string buf;   // the encoded fields
    size_t pos = 0;    // read cursor for get()

    void putField(const std::string& s) {
        buf += std::to_string(s.size());
        buf += ':';
        buf += s;
    }

    std::string getField() {
        size_t colon = buf.find(':', pos);
        if (colon == std::string::npos)
            throw RpcError("Packet: missing field (fewer values than expected)");
        std::string lenText = buf.substr(pos, colon - pos);
        if (lenText.empty() || lenText.find_first_not_of("0123456789") != std::string::npos)
            throw RpcError("Packet: bad field length \"" + lenText + "\"");
        size_t len = std::stoul(lenText);
        if (colon + 1 + len > buf.size())
            throw RpcError("Packet: field is cut short");
        std::string field = buf.substr(colon + 1, len);
        pos = colon + 1 + len;
        return field;
    }
};

// Framing over a connected socket (implemented in rpc_common.cpp).
void sendFrame(int sock, const std::string& payload);   // throws RpcError
std::string recvFrame(int sock);                         // throws RpcError

#endif
