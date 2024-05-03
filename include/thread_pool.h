#ifndef __THREAD_POOL_H_
#define __THREAD_POOL_H_

#include <future>
#include <vector>
#include <memory>
#include "ringbuf_worker.h"


class ThreadPool
{
private:
    std::vector<std::thread> _pool;
    std::shared_ptr<RingWorker> ringWorker;
    std::atomic_bool isRunning;
public:
    ThreadPool(uint32_t threadCount = 10,uint32_t taskCount = 100) {
        isRunning = true;
        ringWorker = std::make_shared<RingWorker>(taskCount);
        for (int i = 0;i < threadCount; i++) {
            _pool.emplace_back(std::thread([&] () {
                while (isRunning) {
                    ringWorker->tick();
                }
            }));
        }
    }
    ~ThreadPool() {
        
    }

    ThreadPool(const ThreadPool&) = delete;
    ThreadPool& operator = (const ThreadPool&) = delete;


    template <typename F, typename... Args>
    std::future<typename std::result_of<F(Args...)>::type> 
    submit(F&& f, Args&&... args) {
        using return_type = typename std::result_of<F(Args...)>::type;
        auto task = std::make_shared<std::packaged_task<return_type()>>(
            std::bind(std::forward<F>(f), std::forward<Args>(args)...)
        );
      
        if (isRunning && ringWorker != nullptr) {
            ringWorker->submit([task] () -> void {
                (*task)();
            });
        } else {
            //task.task->get_future().set_exception(std::make_exception_ptr(std::runtime_error("线程池未启用")));
        }
        
        return task->get_future();
    }
    static ThreadPool &instance();
};


ThreadPool& ThreadPool::instance() {
    static ThreadPool instance;
    return instance;
}

#endif