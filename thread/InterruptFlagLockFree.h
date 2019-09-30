#pragma once
#include <condition_variable>
#include <exception>
#include <chrono>
#include "../spinlock/Spinlock.h"

namespace tvp
{
	void interruptionPoint();
	class InterruptFlagLockFree
	{
	private:
		std::atomic<bool> mFlag;
		std::condition_variable* mThreadCond;
		std::condition_variable_any* mThreadCondAny;
		tvp::Spinlock mMutex;

	public:
		InterruptFlagLockFree() noexcept : mThreadCond(nullptr), mThreadCondAny(nullptr), mFlag(false)
		{}

		void set()
		{
			mFlag.store(true, std::memory_order_relaxed);
			tvp::LockGuard lk(mMutex);
			if (mThreadCond)
			{
				mThreadCond->notify_all();
			}
			else if (mThreadCondAny)
			{
				mThreadCondAny->notify_all();
			}
		}

		bool isSet() const noexcept
		{
			return mFlag.load(std::memory_order_relaxed);
		}

		void setCV(std::condition_variable& cv)
		{
			tvp::LockGuard lk(mMutex);
			mThreadCond = &cv;
		}

		void clearCV()
		{
			tvp::LockGuard lk(mMutex);
			mThreadCond = nullptr;
		}
	};
}
