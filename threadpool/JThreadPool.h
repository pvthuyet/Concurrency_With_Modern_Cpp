#pragma once
#include <thread>
#include <atomic>
#include <vector>
#include <functional>
#include <memory>
#include <queue>
#include "../queue/JQueue.h"
#include "../thread/JThread.h"
#include <vld.h>

namespace tvp
{
	class JThreadPool
	{
	public:
		using Callable = std::function<void()>;
		using LocalQueueType = std::queue<Callable>;
		using UniqueThread = std::unique_ptr<tvp::JThread>;

	private:
		// data members
		std::atomic_bool mShutdown;
		JQueue<Callable> mPoolWorkQueue;
		static thread_local std::unique_ptr<LocalQueueType> mLocalWorkQueue;

		// mThreads
		std::vector<UniqueThread> mThreads;

		// private method.
		void workerThread() {
			gLogger.debug(Utils::getThreadId() + " Initialize...:\n");
			mLocalWorkQueue.reset(new LocalQueueType());
			while (!mShutdown) {
				runPendingTask();
			}
		}

	public:
		explicit JThreadPool(std::size_t numThread = std::thread::hardware_concurrency()) : mShutdown(false)
		{
			try {
				for (std::size_t i = 0; i < numThread; ++i) {
					mThreads.emplace_back(std::make_unique<tvp::JThread>(&JThreadPool::workerThread, this));
				}
			}
			catch (...) {
				mShutdown.store(true);
				throw;
			}
		}
		
		JThreadPool(const JThreadPool&) = delete;
		JThreadPool& operator=(const JThreadPool&) = delete;

		virtual ~JThreadPool() 
		{
			shutdown();
		}

		void shutdown()
		{
			mShutdown.store(true);
		}

		bool isShutdown()
		{
			return mShutdown.load();
		}

		void runPendingTask() {
			Callable task;
			if (mLocalWorkQueue && !mLocalWorkQueue->empty()) {
				gLogger.debug(Utils::getThreadId() + " POP mLocalWorkQueue\n");
				task = std::move(mLocalWorkQueue->front());
				mLocalWorkQueue->pop();
				task();
			}
			else if (mPoolWorkQueue.tryPop(task)) {
				gLogger.debug(Utils::getThreadId() + " POP mPoolWorkQueue\n");
				task();
			}
			else {
				std::this_thread::sleep_for(std::chrono::milliseconds(1000));
			}
		}

		template<class Fn, class... Args>
		auto submit(Fn&& f, Args&&... args)
			-> std::future<std::result_of_t<Fn(Args...)> >
		{
			using return_type = std::result_of_t<Fn(Args...)>;
			auto task = std::make_shared<std::packaged_task<return_type()> >
				(std::bind(std::forward<Fn>(f), std::forward<Args>(args)...)
					);
			std::future<return_type> res = task->get_future();
			if (mLocalWorkQueue) {
				gLogger.debug(Utils::getThreadId() + " push mLocalWorkQueue...:\n");
				mLocalWorkQueue->push([task]() {
					(*task)();
				});
			}
			else {
				gLogger.debug(Utils::getThreadId() + " push mLocalWorkQueue...:\n");
				mPoolWorkQueue.push([task]() {
					(*task)();
				});
			}

			return res;
		}
	};
	// Must initialize for thread_local variable.
	thread_local std::unique_ptr<JThreadPool::LocalQueueType> JThreadPool::mLocalWorkQueue = nullptr;
}