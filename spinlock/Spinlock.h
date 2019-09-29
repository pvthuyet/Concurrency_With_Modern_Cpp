#pragma once
#include <atomic>

namespace tvp
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
			while(mFlag.test_and_set())
			{ }
		}
		void unlock() noexcept
		{
			mFlag.clear();
		}
	};

	class SpinlockGuard
	{
	private:
		Spinlock& mSpin;
	public:
		SpinlockGuard() = delete;
		SpinlockGuard(Spinlock& spin) noexcept
			: mSpin(spin)
		{
			mSpin.lock();
		}
		~SpinlockGuard() noexcept
		{
			mSpin.unlock();
		}

		SpinlockGuard(const Spinlock&) = delete;
		SpinlockGuard& operator=(const Spinlock&) = delete;
	};
}