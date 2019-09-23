#pragma once
#include <thread>
#include <exception>
#include "InterruptFlag.h"

namespace tvp
{
	thread_local InterruptFlag tlFlag;

	struct ClearCVOnDestruct
	{
		~ClearCVOnDestruct()
		{
			tlFlag.clearCV();
		}
	};

	class ThreadInterrupted : public std::exception
	{
	public:
		virtual char const* what() const
		{
			return "Thread Interrupted Exception";
		}
	};

	class InterruptibleThread
	{
	private:
		std::thread mT;
		InterruptFlag* mFlag;

	private:
		static void interruptionPoint()
		{
			if (tlFlag.isSet())
			{
				throw ThreadInterrupted();
			}
		}

	public:
		// Constructors
		InterruptibleThread() noexcept = default;
		
		template<typename Callable, typename... Args>
		explicit InterruptibleThread(Callable&& func, Args&&... args) :
			mT(std::forward<Callable>(func), std::forward<Args>(args)...)
		{}
		
		explicit InterruptibleThread(std::thread other) noexcept :
			mT(std::move(other))
		{}
		
		InterruptibleThread(InterruptibleThread&& other) noexcept :
			mT(std::move(other.mT))
		{}

		template<typename Callable>
		InterruptibleThread(Callable func)
		{
			std::promise<InterruptFlag*> p;
			mT = std::thread([func, &p] {
				p.set_value(&tlFlag);
				try
				{
					func();
				}
				catch (ThreadInterrupted const& e)
				{
					std::cout << e.what();
				}
			});
			mFlag = p.get_future().get();
		}

		// Destructor
		~InterruptibleThread() noexcept
		{
			if (joinable())
			{
				join();
			}
		}

		// Operators
		InterruptibleThread& operator=(InterruptibleThread&& other) noexcept
		{
			if (joinable())
			{
				join();
			}
			mT = std::move(other.mT);
			return *this;
		}

		InterruptibleThread& operator=(std::thread other) noexcept
		{
			if (joinable())
			{
				join();
			}
			mT = std::move(other);
			return *this;
		}
		// Methods
		void swap(InterruptibleThread& other) noexcept
		{
			mT.swap(other.mT);
		}

		std::thread::id get_id() const noexcept
		{
			return mT.get_id();
		}

		bool joinable() const noexcept
		{
			return mT.joinable();
		}

		void join()
		{
			mT.join();
		}

		void detach()
		{
			mT.detach();
		}

		std::thread& asThread() noexcept
		{
			return mT;
		}

		const std::thread& asThread() const noexcept
		{
			return mT;
		}

		// Delete methods
		InterruptibleThread(InterruptibleThread&) = delete;
		InterruptibleThread& operator=(InterruptibleThread&) = delete;

		// Interrupted functoin
		void interrupt()
		{
			if (mFlag)
			{
				mFlag->set();
			}
		}

		static void interruptibleWait(std::condition_variable& cv, std::unique_lock<std::mutex>& lk)
		{
			InterruptibleThread::interruptionPoint();
			tlFlag.setCV(cv);
			ClearCVOnDestruct guard;
			InterruptibleThread::interruptionPoint();
			cv.wait_for(lk, std::chrono::milliseconds(1));
			InterruptibleThread::interruptionPoint();
		}
	};
}
