#ifndef __TCP_MESSAGES_H__
#define __TCP_MESSAGES_H__

#include <stdint.h>

enum TcpMessageType : uint8_t {
    TCP_SUBSYSTEM_FORWARD = 0,
    TCP_SUBSYSTEM_REGISTER = 1,
    TCP_SUBSYSTEM_UNREGISTER = 2
};

struct TcpMessage {
    TcpMessageType _type;   // Type of message to receive
    uint64_t _size = 0;     // Size of ENTIRE malloc block
    uint8_t _payload[0];    // Either endpoint register target OR payload to forward

    uint64_t PayloadSize() const {
        return _size - sizeof(TcpMessage);
    }
};

#endif  // __TCP_MESSAGES_H__
