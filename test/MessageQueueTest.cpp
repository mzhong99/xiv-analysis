#include "MessageQueue.h"
#include <doctest.h>

#include <thread>
#include <vector>
#include <set>

TEST_CASE("MessageQueue_SingleThreaded") {
    int result;
    MessageQueue<int> mqueue;

    std::vector<int> test_data = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    REQUIRE_FALSE(mqueue.Pop(result, 0ms));

    for (const int &elem : test_data) {
        REQUIRE(mqueue.Push(elem));
    }

    for (const int &elem : test_data) {
        REQUIRE(mqueue.Pop(result));
        REQUIRE(result == elem);
    }

    REQUIRE_FALSE(mqueue.Pop(result, 0ms));
}

TEST_CASE("Multi-threaded testing") {
    MessageQueue<int> mqueue;
    std::thread producer([&] {
        for (int i = 0; i < 100000; i++) {
            REQUIRE(mqueue.Push(i));
        }
    });

    std::thread consumer([&] {
        for (int i = 0; i < 100000; i++) {
            int result;
            REQUIRE(mqueue.Pop(result));
            REQUIRE(result == i);
        }
    });

    producer.join();
    consumer.join();
}

TEST_CASE("MessageQueue_MultiThreaded") {
    MessageQueue<int> mqueue;
    std::set<int> d1, d2;

    std::thread producer([&] {
        for (int i = 0; i < 100000; i++) {
            REQUIRE(mqueue.Push(i));
        }
    });

    std::thread consumer1([&] {
        for (int i = 0; i < 50000; i++) {
            int result;
            REQUIRE(mqueue.Pop(result));
            d1.insert(result);
        }
    });

    std::thread consumer2([&] {
        for (int i = 0; i < 50000; i++) {
            int result;
            REQUIRE(mqueue.Pop(result));
            d2.insert(result);
        }
    });

    producer.join();
    consumer1.join();
    consumer2.join();

    for (int i = 0; i < 100000; i++) {
        bool exists = d1.contains(i) || d2.contains(i);
        REQUIRE(exists);
    }
}
