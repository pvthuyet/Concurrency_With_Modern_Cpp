#pragma once
#include <atomic>
#include <thread>
#include <chrono>

namespace tvp
{

	// SpinMutex will busy wait if number of running thread > number of hardware thread.
	// Then be careful
	class SpinMutex
	{
	private:
		std::atomic_flag mFlag = ATOMIC_FLAG_INIT;

	public:
		SpinMutex() = default;
		~SpinMutex() = default;
		SpinMutex(SpinMutex const&) = delete;
		SpinMutex& operator=(SpinMutex const&) = delete;

		void lock() noexcept
		{
			while (mFlag.test_and_set(std::memory_order_acquire))
				std::this_thread::sleep_for(std::chrono::milliseconds(1));
		}
		void unlock() noexcept
		{
			mFlag.clear(std::memory_order_release);
		}
	};

}