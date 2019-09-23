#pragma once
#include <thread>
#include <exception>
#include "InterruptFlag.h"

namespace tvp
{
	thread_local InterruptFlag gInterruptedFlag;

	struct ClearCVOnDestruct
	{
		~ClearCVOnDestruct()
		{
			gInterruptedFlag.clearCV();
		}
	};

	class ThreadInterrupted : public std::exception
	{
	public:
		virtual char const* what() const
		{
			return "Thread Interrupted Exception!\n";
		}
	};

	void interruptionPoint()
	{
		if (gInterruptedFlag.isSet())
		{
			throw ThreadInterrupted();
		}
	}

	void interruptibleWait(std::condition_variable& cv, std::unique_lock<std::mutex>& lk)
	{
		interruptionPoint();
		gInterruptedFlag.setCV(cv);
		ClearCVOnDestruct guard;
		interruptionPoint();
		cv.wait_for(lk, std::chrono::milliseconds(1));
		interruptionPoint();
	}

	template<typename Predicate>
	void interruptibleWait(std::condition_variable& cv, std::unique_lock<std::mutex>& lk, Predicate pred)
	{
		interruptionPoint();
		gInterruptedFlag.setCV(cv);
		ClearCVOnDestruct guard;
		while (!gInterruptedFlag.isSet() && !pred())
		{
			cv.wait_for(lk, std::chrono::milliseconds(1));
		}
		interruptionPoint();
	}

	template<typename Lockable>
	void interruptibleWait(std::condition_variable_any& cv, Lockable& lk)
	{
		gInterruptedFlag.wait(cv, lk);
	}

	template<typename T>
	void interruptibleWait(std::future<T>& uf)
	{
		while (!gInterruptedFlag.isSet())
		{
			if (uf.wait_for(std::chrono::milliseconds(1)) == std::future_status::ready)
			{
				break;
			}
		}
		interruptionPoint();
	}


	class InterruptibleThread
	{
	private:
		std::thread mT;
		InterruptFlag* mFlag;

	public:
		// Constructors
		InterruptibleThread() noexcept = default;
		
		template<typename Callable, typename... Args>
		explicit InterruptibleThread(Callable&& func, Args&&... args)
		{
			using return_type = std::result_of_t<Callable(Args...)>;
			auto task = std::make_shared<std::packaged_task<return_type()> >(std::bind(std::forward<Callable>(func), std::forward<Args>(args)...));

			std::promise<InterruptFlag*> p;
			mT = std::thread([task, &p] {
				p.set_value(&gInterruptedFlag);
				try
				{
					(*task)();
				}
				catch (ThreadInterrupted const& e)
				{
					std::cout << e.what();
				}
			});
			mFlag = p.get_future().get();
		}
		
		explicit InterruptibleThread(std::thread other) noexcept :
			mT(std::move(other))
		{}
		
		InterruptibleThread(InterruptibleThread&& other) noexcept :
			mT(std::move(other.mT))
		{}

		// Destructor
		~InterruptibleThread() noexcept
		{
			if (joinable())
			{
				join();
			}
			mFlag = nullptr;
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
	};
}
