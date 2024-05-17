#ifndef __THREAD_POOL_H_
#define __THREAD_POOL_H_

#include <functional>
#include <future>
#include <vector>
#include <memory>
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

class ThreadPool {
private:
	std::vector<std::thread> _pool;
	std::shared_ptr<RingWorker> ringWorker;
	std::atomic_bool isRunning;
public:
	ThreadPool(uint32_t threadCount = 10, uint32_t taskCount = 100) {
		isRunning = true;
		ringWorker = std::make_shared<RingWorker>(taskCount);
		for (int i = 0; i < threadCount; i++) {
			_pool.emplace_back(std::thread([&]() {
				while (isRunning) {
					ringWorker->tick();
				}
			}));
		}
	}
	~ThreadPool() {

	}

	ThreadPool(const ThreadPool &) = delete;
	ThreadPool &operator=(const ThreadPool &) = delete;

	template<typename F,
		typename... Args>
	std::future<typename std::invoke_result<F(Args...)>::type>
	submit(F &&f, Args &&... args) {
		using return_type = typename std::invoke_result<F(Args...)>::type;
		auto task = std::make_shared<std::packaged_task<return_type()>>(
			std::bind(std::forward<F>(f), std::forward<Args>(args)...)
		);

		if (isRunning && ringWorker != nullptr) {
			ringWorker->submit([task]() -> void {
				(*task)();
			});
		} else {
			//task.task->get_future().set_exception(std::make_exception_ptr(std::runtime_error("线程池未启用")));
		}

		return task->get_future();
	}
	static ThreadPool &instance();
};

ThreadPool &ThreadPool::instance() {
	static ThreadPool instance;
	return instance;
}

#endif