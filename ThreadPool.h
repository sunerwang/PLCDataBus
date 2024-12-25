#pragma once
#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <queue>

class ThreadPool {
public:
	ThreadPool(int numThreads);
	~ThreadPool();

	template<typename F, typename... Args>
	void enqueue(F&& f, Args&&... args) {
		std::function<void()> task(std::bind(std::forward<F>(f), std::forward<Args>(args)...));
		{
			std::unique_lock<std::mutex> lock(mutex);
			tasks.emplace(std::move(task));
		}
		condition.notify_one();
	}

private:
	std::vector<std::thread> threads;
	std::queue<std::function<void()>> tasks;
	std::mutex mutex;
	std::condition_variable condition;
	bool stop;

};
