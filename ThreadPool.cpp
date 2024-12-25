#include "ThreadPool.h"

ThreadPool::ThreadPool(int numThreads) :stop(false) {
	for (int i = 0; i < numThreads; ++i) {
		threads.emplace_back([this] {
			while (true) {
				std::unique_lock<std::mutex> lock(mutex);
				condition.wait(lock, [this] { return stop || !tasks.empty(); });
				if (stop && tasks.empty()) {
					return;
				}
				std::function<void()> task(std::move(tasks.front()));
				tasks.pop();
				lock.unlock();
				task();
			}
			});
	}
}

ThreadPool::~ThreadPool() {
	{
		std::unique_lock<std::mutex> lock(mutex);
		stop = true;
	}
	condition.notify_all();
	for (std::thread& thread : threads) {
		thread.join();
	}
}

