#pragma once
#include <thread>
#include <future>
#include <condition_variable>
#include <exception>
#include <chrono>

namespace tvp
{
	void interruptionPoint();
	class InterruptFlag
	{
	private:
		std::atomic<bool> mFlag;
		std::condition_variable* mThreadCond;
		std::condition_variable_any* mThreadCondAny;
		std::mutex mMutex;

	public:
		InterruptFlag() : mThreadCond(nullptr), mThreadCondAny(nullptr), mFlag(false)
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

		template<typename Lockable>
		void wait(std::condition_variable_any& cv, Lockable& lk)
		{
			struct CustomLock
			{
				InterruptFlag* self;
				Lockable& lk;
				CustomLock(InterruptFlag* self_, std::condition_variable_any& cond, Lockable& lk_) :
					self(self_),
					lk(lk_)
				{
					self->mMutex.lock();
					self->mThreadCondAny = &cond;
				}

				void unlock()
				{
					lk.unlock();
					self->mMutex.unlock();
				}

				void lock()
				{
					std::lock(self->mMutex, lk);
				}

				~CustomLock()
				{
					self->mThreadCondAny = nullptr;
					self->mMutex.unlock();
				}
			};

			CustomLock cl(this, cv, lk);
			interruptionPoint();
			cv.wait_for(lk, std::chrono::milliseconds(1));
			interruptionPoint();
		}
	};
}
