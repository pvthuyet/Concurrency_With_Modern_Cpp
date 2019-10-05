#pragma once
#include <atomic>
#include <thread>
#include <chrono>

namespace tvp
{
	namespace lockfree
	{
		class Spinlock
		{
		private:
			std::atomic_flag mFlag = ATOMIC_FLAG_INIT;

		public:
			Spinlock() = default;
			~Spinlock() = default;
			Spinlock(Spinlock const&) = delete;
			Spinlock& operator=(Spinlock const&) = delete;

			void lock() noexcept
			{
				while (mFlag.test_and_set(std::memory_order_acquire))
				{
					std::this_thread::sleep_for(std::chrono::milliseconds(1));
				}
			}
			void unlock() noexcept
			{
				mFlag.clear(std::memory_order_release);
			}
		};

		class LockGuard
		{
		private:
			Spinlock& mSpin;
		public:
			LockGuard() = delete;
			LockGuard(Spinlock& spin) noexcept
				: mSpin(spin)
			{
				mSpin.lock();
			}
			~LockGuard() noexcept
			{
				mSpin.unlock();
			}

			// copiable
			LockGuard(const LockGuard&) = delete;
			LockGuard& operator=(const LockGuard&) = delete;
		};
	}
}