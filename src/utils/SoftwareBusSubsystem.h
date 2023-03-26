#ifndef __SOFTWARE_BUS_SUBSYSTEM_H__
#define __SOFTWARE_BUS_SUBSYSTEM_H__

#include "Subsystem.h"
#include "MessageQueue.h"

#include <mutex>
#include <map>

struct SoftwareBusMessage {
 public:
    std::vector<uint8_t> _payload;
    std::string _endpoint;

    SoftwareBusMessage() = default;

    SoftwareBusMessage(const void *payload_vp, size_t size, const std::string &endpoint): _endpoint(endpoint) {
        if (size > 0) {
            const uint8_t *payload = (const uint8_t *)payload_vp;
            _payload = std::vector<uint8_t>(payload, payload + size);
        }
    }
};

typedef void (*SoftwareBusCallback) (void *ctx, const SoftwareBusMessage &message);

struct SoftwareBusHandler {
    void *_context;
    SoftwareBusCallback _callback;

    SoftwareBusHandler(void *context, SoftwareBusCallback callback) {
        _context = context;
        _callback = callback;
    }

    void Handle(SoftwareBusMessage &message) {
        _callback(_context, message);
    }
};

class SoftwareBusSubsystem : public Subsystem<SoftwareBusSubsystem> {
 private:
    MessageQueue<SoftwareBusMessage> _mqueue;

    std::mutex _subscribers_lock;
    std::map<std::string, std::vector<SoftwareBusHandler>> _subscribers;

    static constexpr std::string SHUTDOWN_ENDPOINT = "/bus/shutdown";

 public:
    void Init();
    void Teardown();

    void ThreadFunction() override;

    void Subscribe(const std::string &endpoint, void *context, SoftwareBusCallback callback);
    void Publish(const std::string &endpoint, const void *payload = nullptr, size_t size = 0);
};

#endif  // __SOFTWARE_BUS_H__
