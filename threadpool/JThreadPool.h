#pragma once
#include <thread>
#include <atomic>
#include <vector>
#include <functional>
#include <memory>
#include <queue>
#include "../queue/JQueue.h"
#include "../thread/JThread.h"

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
		tvp::Logger& mLogger;
		std::atomic_bool mShutdown;
		JQueue<Callable> mPoolWorkQueue;
		static thread_local std::unique_ptr<LocalQueueType> mLocalWorkQueue;

		// mThreads
		std::vector<UniqueThread> mThreads;

		// private method.
		void workerThread() 
		{
			mLogger.debug(Utils::getThreadId() + " initialized\n");
			mLocalWorkQueue.reset(new LocalQueueType());
			while (!mShutdown) 
			{
				try
				{
					runPendingTask();
				}
				catch (...)
				{
					mLogger.debug(tvp::Utils::getThreadId() + " throw exception!\n");
				}
			}
			mLogger.debug(tvp::Utils::getThreadId() + " workerThread() threadPool was shutdown!\n");
		}

	public:
		explicit JThreadPool(tvp::Logger& logger, std::size_t numThread = std::thread::hardware_concurrency()) :
			mShutdown(false), mLogger(logger)
		{
			try 
			{
				for (std::size_t i = 0; i < numThread; ++i) 
				{
					mThreads.emplace_back(std::make_unique<tvp::JThread>(&JThreadPool::workerThread, this));
				}
			}
			catch (...) 
			{
				mShutdown.store(true);
				throw;
			}
		}
		
		JThreadPool(const JThreadPool&) = delete;
		JThreadPool& operator=(const JThreadPool&) = delete;

		virtual ~JThreadPool() noexcept
		{
			shutdown();
		}

		void shutdown() noexcept
		{
			mShutdown.store(true);
		}

		bool isShutdown() const noexcept
		{
			return mShutdown.load();
		}

		void runPendingTask() 
		{
			Callable task;
			if (mLocalWorkQueue && !mLocalWorkQueue->empty()) 
			{
				mLogger.debug(Utils::getThreadId() + " pop local work queue\n");
				task = std::move(mLocalWorkQueue->front());
				mLocalWorkQueue->pop();
				task();
			}
			else if (mPoolWorkQueue.tryPop(task)) 
			{
				task();
			}
			else 
			{
				std::this_thread::yield();
			}
		}

		// Throw ShutdownThreadPoolException
		template<class Fn, class... Args>
		auto submit(Fn&& f, Args&&... args)
			-> std::future<std::result_of_t<Fn(Args...)> >
		{
			if (isShutdown())
			{
				throw JException(tvp::ExceptionCode::THREADPOOL_SHUTDOWN, 
					tvp::Utils::getThreadId() + " submit() threadPool was shutdown!\n");
			}

			using return_type = std::result_of_t<Fn(Args...)>;
			auto task = std::make_shared<std::packaged_task<return_type()> >
				(std::bind(std::forward<Fn>(f), std::forward<Args>(args)...)
					);
			std::future<return_type> res = task->get_future();
			if (mLocalWorkQueue) 
			{
				mLogger.debug(Utils::getThreadId() + " push local work queue\n");
				mLocalWorkQueue->push([task]() {
					(*task)();
				});
			}
			else 
			{
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