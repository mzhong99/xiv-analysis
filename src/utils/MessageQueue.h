#ifndef __MESSAGE_QUEUE_H__
#define __MESSAGE_QUEUE_H__

#include <queue>
#include <mutex>
#include <condition_variable>

#include <chrono>

using namespace std::literals::chrono_literals;

template <typename T>
class MessageQueue {
 private:
    std::mutex _lock;
    std::condition_variable _cond;
    std::queue<T> _queue;

 public:
    MessageQueue() = default;

    bool Push(const T &data) {
        std::unique_lock<std::mutex> lock_acquire(_lock);
        _queue.push(data);
        _cond.notify_one();

        return true;
    }

    bool Pop(T &data_out, std::chrono::milliseconds timeout_ms = -1ms) {
        std::unique_lock<std::mutex> lock_acquire(_lock);

        if (timeout_ms < 0ms) {
            _cond.wait(lock_acquire, [this] { return !_queue.empty(); });
        } else {
            if (_queue.empty()) {
                if (!_cond.wait_for(lock_acquire, timeout_ms, [this] { return !_queue.empty(); })) {
                    return false;
                }
            }
        }

        data_out = _queue.front();
        _queue.pop();

        return true;
    }
};

#endif  // __MESSAGE_QUEUE_H__
