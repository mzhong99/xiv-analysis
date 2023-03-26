#include "SoftwareBusSubsystem.h"

#include <spdlog/spdlog.h>

void SoftwareBusSubsystem::ThreadFunction() {
    while (true) {
        SoftwareBusMessage message;
        _mqueue.Pop(message);

        if (message._endpoint == SoftwareBusSubsystem::SHUTDOWN_ENDPOINT) {
            spdlog::debug("Shutdown signal received from software bus");
            break;
        } else {
            std::lock_guard<std::mutex> lock_acquire(_subscribers_lock);
            for (SoftwareBusHandler &handler : _subscribers[message._endpoint]) {
                handler.Handle(message);
            }
        }
    }
}

void SoftwareBusSubsystem::Init() {
    StartWorkerThread();
    Subsystem<SoftwareBusSubsystem>::Init();
}

void SoftwareBusSubsystem::Teardown() {
    Publish(SoftwareBusSubsystem::SHUTDOWN_ENDPOINT);
    Subsystem<SoftwareBusSubsystem>::Teardown();
}

void SoftwareBusSubsystem::Subscribe(const std::string &endpoint, void *context, SoftwareBusCallback callback) {
    std::lock_guard<std::mutex> lock_acquire(_subscribers_lock);
    _subscribers[endpoint].push_back(SoftwareBusHandler(context, callback));
}

void SoftwareBusSubsystem::Publish(const std::string &endpoint, const void *payload, size_t size) {
    SoftwareBusMessage message(payload, size, endpoint);
    _mqueue.Push(message);
}

