#include <doctest.h>
#include <spdlog/spdlog.h>

#include <string>

#include <random>

#include <list>
#include <vector>

#include "List.h"

struct WrappedString {
    std::string _data;
    List<WrappedString>::Node _node;

    WrappedString(const std::string &str) {
        _data = str;
    }
};

TEST_CASE("List_SimpleClassInsertion") {
    List<WrappedString> experimental;
    std::list<std::string> expected;

    REQUIRE(experimental.Empty());
    REQUIRE(expected.empty());

    for (int i = 0; i < 1024; i++) {
        std::string raw = fmt::format("{}", i);
        WrappedString *wrapped_string = new WrappedString(raw);

        experimental.PushBack(wrapped_string, &wrapped_string->_node);
        expected.push_back(raw);
    }

    List<WrappedString>::Node *iter = experimental.Begin();
    for (auto it = expected.begin(); it != expected.end(); it++, iter = iter->Next()) {
        REQUIRE(*it == iter->Entry()->_data);
    }
    REQUIRE(iter == experimental.End());

    for (int i = 0; i < 1024; i++) {
        WrappedString *wrapped_string = experimental.Back();
        REQUIRE(wrapped_string->_data == expected.back());
        wrapped_string = experimental.PopBack();
        REQUIRE(wrapped_string->_data == expected.back());

        expected.pop_back();
        delete wrapped_string;
    }

    REQUIRE(experimental.Empty());
    REQUIRE(expected.empty());
}

TEST_CASE("List_RandomizedInsertionAndRemoval") {
    std::random_device rd;
    std::mt19937 generator(rd());

    std::uniform_real_distribution<double> distribution(0, 1);

    std::vector<WrappedString *> managed;
    std::vector<std::string> expected;

    List<WrappedString> experimental;

    for (int i = 0; i < 1024; i++) {
        REQUIRE(experimental.Empty() == managed.empty());
        if (managed.empty() || distribution(generator) < 0.7) {
            WrappedString *entry = new WrappedString(fmt::format("{}", i));

            managed.push_back(entry);
            expected.push_back(std::string(entry->_data));
            experimental.PushBack(entry, &entry->_node);
        } else {
            std::uniform_int_distribution<size_t> idx_distribution(0, managed.size() - 1);
            size_t idx = idx_distribution(generator);
            WrappedString *entry = managed[idx];
            REQUIRE(entry->_data == expected[idx]);

            expected.erase(expected.begin() + idx);
            managed.erase(managed.begin() + idx);

            delete entry;
        }

        size_t idx_iter = 0;
        for (List<WrappedString>::Node *node = experimental.Begin(); node != experimental.End(); node = node->Next()) {
            CHECK(node->Entry()->_data == expected[idx_iter++]);
        }
    }

    while (!experimental.Empty()) {
        WrappedString *entry = experimental.PopBack();
        delete entry;

        managed.pop_back();
    }

    REQUIRE(experimental.Empty());
    REQUIRE(managed.empty());
}
