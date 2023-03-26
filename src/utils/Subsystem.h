#ifndef __SUBSYSTEM_H__
#define __SUBSYSTEM_H__

#include <exception>
#include <atomic>

#include <spdlog/spdlog.h>

template <typename T>
class Subsystem {
 private:
    bool _has_parallel_thread = false;
    std::thread _thread;

 protected:
    std::atomic<bool> _running = false;

    void StartWorkerThread() {
        _has_parallel_thread = true;
    }

 public:
    Subsystem() = default;
    Subsystem(const Subsystem<T> &rhs) = delete;

    void Init() {
        _running = true;

        if (_has_parallel_thread) {
            _thread = std::thread([this] { dynamic_cast<T*>(this)->ThreadFunction(); });
        }
    }

    void Teardown() {
        _running = false;

        if (_has_parallel_thread) {
            _thread.join();
        }
    }

    virtual void ThreadFunction() {
        spdlog::warn("Subsystem created without implementing thread function");
    }
};

#endif  // __MODULE_H__
