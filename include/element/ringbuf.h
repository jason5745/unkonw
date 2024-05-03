#ifndef __RINGBUF_H__
#define __RINGBUF_H__

#include <vector>
#include <memory>

template<typename T>
class ringbuf {
private:
    volatile size_t _head = 0;
    volatile size_t _tail = 0;
    std::vector<T> _node;
    size_t _max;
public:

    ringbuf(size_t initialsize) :_max(initialsize + 1), _head(0), _tail(0) {
        _node = std::vector<T>(initialsize + 1);
    }
    virtual ~ringbuf() {}

    ringbuf(const ringbuf&) = delete;
    ringbuf& operator = (const ringbuf&) = delete;


    bool full() {
        return ((_tail + 1) % _max == _head);
    }
    bool empty() {
        return (_head == _tail);
    }


    template <typename _TP>
    bool _push(_TP&& val) {
        if (full()) {
            return false;
        }

        _node[_tail] = std::move(std::forward<_TP>(val));
        _tail = (_tail + 1) % _max; 
        return true;
    }

    bool push(const T& val) {
        return _push(val);
    }

    bool push(T&& val) {
        return _push(std::move(val));
    }

    bool pop(T& val) {
        if (empty()) {
            return false;
        }
        val = std::move(_node[_head]);
        _head = (_head + 1) % _max;
        return true;
    }
};

#endif