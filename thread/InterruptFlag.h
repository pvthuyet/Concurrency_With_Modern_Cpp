#pragma once
#include <thread>
#include <future>
#include <condition_variable>
#include <exception>
#include <chrono>

namespace tvp
{
	class InterruptFlag
	{
	private:
		std::atomic<bool> mFlag;
		std::condition_variable* mThreadCond;
		std::condition_variable_any* mThreadCondAny;
		std::mutex mMutex;

	public:
		InterruptFlag() : mThreadCond(nullptr), mThreadCondAny(nullptr)
		{}

		void set()
		{
			mFlag.store(true, std::memory_order_relaxed);
			std::lock_guard<std::mutex> lk(mMutex);
			if (mThreadCond)
			{
				mThreadCond->notify_all();
			}
			else if (mThreadCondAny)
			{
				mThreadCondAny->notify_all();
			}
		}
		bool isSet() const
		{
			return mFlag.load(std::memory_order_relaxed);
		}

		void setCV(std::condition_variable& cv)
		{
			std::lock_guard<std::mutex> lk(mMutex);
			mThreadCond = &cv;
		}
		void clearCV()
		{
			std::lock_guard<std::mutex> lk(mMutex);
			mThreadCond = nullptr;
		}
	};
}
