#pragma once
#include <thread>
#include <atomic>
#include <vector>
#include <functional>
#include <memory>
#include <queue>
#include "../queue/JQueue.h"
#include "../thread/JThread.h"
#include "../../logger/Logger.h"

namespace tvp
{
	using Callable = std::function<void()>;
	class JWorkStealingQueue
	{
	private:
		std::deque<Callable> mQueue;
		mutable std::mutex mMutex;

	public:
		JWorkStealingQueue()
		{}

		JWorkStealingQueue(const JWorkStealingQueue&) = delete;
		JWorkStealingQueue& operator=(const JWorkStealingQueue&) = delete;

		void push(Callable data)
		{
			std::lock_guard<std::mutex> lk(mMutex);
			mQueue.push_front(std::move(data));
		}

		bool empty() const noexcept
		{
			std::lock_guard<std::mutex> lk(mMutex);
			return mQueue.empty();
		}

		bool tryPop(Callable& res)
		{
			std::lock_guard<std::mutex> lk(mMutex);
			if (mQueue.empty())
			{
				return false;
			}
			res = std::move(mQueue.front());
			mQueue.pop_front();
			return true;
		}

		bool trySteal(Callable& res)
		{
			std::lock_guard<std::mutex> lk(mMutex);
			if (mQueue.empty())
			{
				return false;
			}
			res = std::move(mQueue.back());
			mQueue.pop_back();
			return true;
		}
	};

	class JThreadPool
	{
	public:		
		using LocalQueueType = std::queue<Callable>;
		using UniqueThread = std::unique_ptr<tvp::JThread>;

	private:
		// data members		
		std::atomic_bool mShutdown;
		tvp::JQueue<Callable> mPoolWorkQueue;
		std::vector<std::unique_ptr<JWorkStealingQueue> > mQueues;
		inline static thread_local JWorkStealingQueue* mLocalWorkQueue{ nullptr }; // C++17: inline
		inline static thread_local std::size_t mIndex{ 0 }; // C++17: inline

		// mThreads
		std::vector<UniqueThread> mThreads;

		//
		bool popTaskFromLocalQueue(Callable& task)
		{
			return mLocalWorkQueue && mLocalWorkQueue->tryPop(task);
		}
		//
		bool popTaskFromPoolQueue(Callable& task)
		{
			return mPoolWorkQueue.tryPop(task);
		}
		//
		bool popTaskOtherThreadQueue(Callable& task)
		{
			for (std::size_t i = 0; i < mQueues.size(); ++i)
			{
				const std::size_t idx = (mIndex + i + 1) % mQueues.size();
				if (mQueues[idx]->trySteal(task))
				{
					return true;
				}
			}
			return false;
		}

		// private method.
		void workerThread(std::size_t idx)
		{
			tvp::Logger* gLogger = tvp::Logger::getInstance();
			gLogger->debug(Utils::getThreadId() + " initialized\n");
			mIndex = idx;
			mLocalWorkQueue = mQueues[mIndex].get();
			while (!mShutdown.load(std::memory_order_relaxed))
			{
				try
				{
					runPendingTask();
				}
				catch (...)
				{
					gLogger->debug(tvp::Utils::getThreadId() + " throw exception!\n");
				}
			}
			gLogger->debug(tvp::Utils::getThreadId() + " workerThread() threadPool was shutdown!\n");
		}

	public:
		explicit JThreadPool(std::size_t numThread = std::thread::hardware_concurrency()) :
			mShutdown(false)
		{
			try 
			{
				mQueues.reserve(numThread);
				// Create queue first
				for (std::size_t i = 0; i < numThread; ++i)
				{
					mQueues.emplace_back(std::make_unique<JWorkStealingQueue>());
				}

				// create thread
				for (std::size_t i = 0; i < numThread; ++i) 
				{
					mThreads.emplace_back(std::make_unique<tvp::JThread>(&JThreadPool::workerThread, this, i));
				}
			}
			catch (...) 
			{
				mShutdown.store(true, std::memory_order_relaxed);
				throw std::current_exception();
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
			mShutdown.store(true, std::memory_order_relaxed);
		}

		bool isShutdown() const noexcept
		{
			return mShutdown.load(std::memory_order_relaxed);
		}

		void runPendingTask() 
		{
			tvp::Logger* gLogger = tvp::Logger::getInstance();
			Callable task;
			if (popTaskFromLocalQueue(task) ||
				popTaskFromPoolQueue(task) ||
				popTaskOtherThreadQueue(task))
			{
				gLogger->debug(Utils::getThreadId() + " pop local work queue\n");
				task();
			}
			else 
			{
				std::this_thread::yield();
			}
		}

		// Throw ShutdownThreadPoolException
		template<class Fn, class... Args>
		decltype(auto) submit(Fn&& f, Args&&... args)
		{
			if (isShutdown())
			{
				throw JException(tvp::ExceptionCode::THREADPOOL_SHUTDOWN, 
					tvp::Utils::getThreadId() + " submit() threadPool was shutdown!\n");
			}

			using return_type = std::invoke_result_t<Fn, Args...>;
			auto task = std::make_shared<std::packaged_task<return_type()> >
				(std::bind(std::forward<Fn>(f), std::forward<Args>(args)...)
					);
			auto res = task->get_future();
			if (mLocalWorkQueue) 
			{
				tvp::Logger* gLogger = tvp::Logger::getInstance();
				gLogger->debug(Utils::getThreadId() + " push local work queue\n");
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
	//thread_local JWorkStealingQueue* JThreadPool::mLocalWorkQueue = nullptr;
	//thread_local std::size_t JThreadPool::mIndex = 0;
}