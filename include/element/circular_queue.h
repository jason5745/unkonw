#ifndef __CIRCULAR_QUEUE_H__
#define __CIRCULAR_QUEUE_H__

#include <vector>
#include <memory>
#include <condition_variable>
#include "logger.h"
template<typename T>
class CircularQueue {
private:
    volatile size_t head_;
    volatile size_t tail_;
    size_t max_;
    std::vector<std::shared_ptr<T>> node_;
    std::condition_variable cv_pop_;
    std::condition_variable cv_push_;

public:
    CircularQueue(const size_t size) :
        head_(0),
        tail_(0),
        max_(size + 1),
        node_(std::move(std::vector<std::shared_ptr<T>>(size + 1))) {}
    virtual ~CircularQueue() = default;

    CircularQueue(const CircularQueue &) = delete;
    CircularQueue &operator=(const CircularQueue &) = delete;

    inline bool full() {
        return ((tail_ + 1) % max_ == head_);
    }
    inline bool empty() {
        return (head_ == tail_);
    }
    
    bool push(T &&val) {
        if (full()) {
            std::mutex mtx;
            std::unique_lock<std::mutex> lock(mtx);
            if (!cv_pop_.wait_for(lock, std::chrono::seconds(3), [&]() { return !full(); })) {
                //超时后仍处于堆积，返回失败
                //TODO: vector扩容
                return false;
            }
        }
        node_[tail_] = std::make_shared<T>(std::forward<T>(val));
        tail_ = (tail_ + 1) % max_;
        cv_push_.notify_one();
        return true;
    }
    std::shared_ptr<T> pop() {
        if (empty()) {
            std::mutex mtx;
            std::unique_lock<std::mutex> lock(mtx);
            if (!cv_push_.wait_for(lock, std::chrono::microseconds(1), [&]() { return !empty(); })) {
                //超时后仍然没有新元素输入，返回失败
                return nullptr;
            }
        }
        std::shared_ptr<T> val = std::move(node_[head_]); //CPU Cache导致其他线程修改后，当前线程无法及时同步的问题，下一次取
        if (val != nullptr) {
            head_ = (head_ + 1) % max_;
            cv_pop_.notify_one();
        }
        return std::move(val);
    }
};

#endif