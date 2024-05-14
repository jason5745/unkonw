#ifndef __RING_WORKER_H__
#define __RING_WORKER_H__

#include <future>
#include "ringbuf.h"

class RingWorker : public RingBuf<std::function<void()>> {
private:
    std::mutex _mtx;
    std::condition_variable _submit_cv;
    std::condition_variable _tick_cv;
public:
    RingWorker(size_t initialsize) : RingBuf<std::function<void()>>(initialsize) {};
    ~RingWorker() override {};

    void submit(std::function<void()> task) {
        std::unique_lock<std::mutex> lock(_mtx);
        _tick_cv.wait(lock, [this] { return !this->full(); });

        if (this->push(task)) {
            _tick_cv.notify_one();
        } else {
            throw std::exception();
        }
    }

    void tick() {
        std::function<void()> task;
        std::unique_lock<std::mutex> lock(_mtx);
        _tick_cv.wait(lock, [this] { return !this->empty(); });
        while (!this->empty()) {
            this->pop(task);
            _submit_cv.notify_one();
            (task)();
        }
    }
};

#endif
