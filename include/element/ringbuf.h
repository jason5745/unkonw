#ifndef __RINGBUF_H__
#define __RINGBUF_H__

#include <vector>
#include <memory>

template<typename T>
class RingBuf {
private:
    volatile size_t head_;
    volatile size_t tail_;
    size_t max_;
    std::vector<T> node_;

    template<typename TP>
    bool push_(TP &&val) {
        if (full()) {
            return false;
        }

        node_[tail_] = std::move(std::forward<TP>(val));
        tail_ = (tail_ + 1) % max_;
        return true;
    }
public:
    RingBuf(const size_t size) : head_(0), tail_(0), max_(size + 1), node_(std::vector<T>(size + 1)) {}
    virtual ~RingBuf() = default;

    RingBuf(const RingBuf &) = delete;
    RingBuf &operator=(const RingBuf &) = delete;

    bool full() {
        return ((tail_ + 1) % max_ == head_);
    }
    bool empty() {
        return (head_ == tail_);
    }

    bool push(const T &val) {
        return push_(val);
    }

    bool push(T &&val) {
        return push_(std::move(val));
    }

    bool pop(T &val) {
        if (empty()) {
            return false;
        }
        val = std::move(node_[head_]);
        head_ = (head_ + 1) % max_;
        return true;
    }
};

#endif