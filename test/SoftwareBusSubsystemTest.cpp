#include <doctest.h>
#include <spdlog/spdlog.h>

#include <string>

#include "Subsystem.h"
#include "SoftwareBusSubsystem.h"

void TestConsumerSubsystemCallback(void *context, const SoftwareBusMessage &message);

class TestConsumerSubsystem : public Subsystem<TestConsumerSubsystem> {
 private:
    MessageQueue<std::string> _mqueue;
    std::string _endpoint;

 public:
    std::atomic<int> _rx_count;

    TestConsumerSubsystem(const std::string &endpoint, SoftwareBusSubsystem *software_bus) {
        _endpoint = endpoint;
        _rx_count = 0;
        software_bus->Subscribe(endpoint, this, TestConsumerSubsystemCallback);
        StartWorkerThread();
    }

    void ThreadFunction() {
        while (_running) {
            std::string line;
            if (_mqueue.Pop(line, 0ms)) {
                if (line == _endpoint) {
                    _rx_count++;
                }
            }
        }
    }

    void ConsumeMessage(const SoftwareBusMessage &message) {
        std::string line((const char *)message._payload.data(), message._payload.size());
        _mqueue.Push(line);
    }
};

void TestConsumerSubsystemCallback(void *context, const SoftwareBusMessage &message) {
    reinterpret_cast<TestConsumerSubsystem *>(context)->ConsumeMessage(message);
}

TEST_CASE("Init and teardown") {
    SoftwareBusSubsystem software_bus;
    software_bus.Init();
    software_bus.Teardown();
}

TEST_CASE("Single producer, multi-consumer") {
    SoftwareBusSubsystem software_bus;
    software_bus.Init();

    TestConsumerSubsystem consumer1("/endpoints/1", &software_bus);
    TestConsumerSubsystem consumer2("/endpoints/2", &software_bus);
    TestConsumerSubsystem consumer3("/endpoints/3", &software_bus);
    TestConsumerSubsystem consumer4("/endpoints/4", &software_bus);

    consumer1.Init();
    consumer2.Init();
    consumer3.Init();
    consumer4.Init();

    for (int i = 0; i < 50000; i++) {
        for (int j = 1; j <= 4; j++) {
            std::string endpoint = fmt::format("/endpoints/{}", j);
            software_bus.Publish(endpoint, (const char *)endpoint.c_str(), endpoint.length());
        }
    }

    auto all_received = [&] (int threshold) {
        return consumer1._rx_count == threshold && consumer2._rx_count == threshold
            && consumer3._rx_count == threshold && consumer4._rx_count == threshold;
    };

    while (!all_received(50000)) {
        continue;
    }

    consumer4.Teardown();
    consumer3.Teardown();
    consumer2.Teardown();
    consumer1.Teardown();

    software_bus.Teardown();

    REQUIRE(all_received(50000));
}
