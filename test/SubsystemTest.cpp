#include <doctest.h>

#include "Subsystem.h"

class NonThreadedSubsystem : public Subsystem<NonThreadedSubsystem> {
 public:
    int _a, _b, _c;

    void Init(int a, int b, int c) {
        _a = a;
        _b = b;
        _c = c;
        Subsystem<NonThreadedSubsystem>::Init();
    }

    void Teardown() {
        Subsystem<NonThreadedSubsystem>::Teardown();
        _a = 0;
        _b = 0;
        _c = 0;
    }
};

class ThreadedSubsystem : public Subsystem<ThreadedSubsystem> {
 public:
    int _count;
    int _dc;

    void Init(int init_count) {
        _count = init_count;

        StartWorkerThread();
        Subsystem<ThreadedSubsystem>::Init();
    }

    void Teardown() {
        _dc = 4000;
        Subsystem<ThreadedSubsystem>::Teardown();
    }

    void ThreadFunction() override {
        while (_running || _count < 2000) {
            _count++;
        }
    }
};

TEST_CASE("Non-threaded subsystem") {
    NonThreadedSubsystem subsystem;

    subsystem.Init(1, 2, 3);

    REQUIRE(subsystem._a == 1);
    REQUIRE(subsystem._b == 2);
    REQUIRE(subsystem._c == 3);

    subsystem.Teardown();

    REQUIRE(subsystem._a == 0);
    REQUIRE(subsystem._b == 0);
    REQUIRE(subsystem._c == 0);
}

TEST_CASE("Threaded subsystem") {
    ThreadedSubsystem subsystem;

    subsystem.Init(5);
    subsystem.Teardown();

    REQUIRE(subsystem._count >= 2000);
    REQUIRE(subsystem._dc == 4000);
}

