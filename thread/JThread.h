#pragma once
#include "InterruptFlag.h"
#include "../logger/logger.h"

extern tvp::Logger gLogger;

namespace tvp
{
	thread_local InterruptFlag gInterruptedFlag;

	struct FlagGuard
	{
		~FlagGuard()
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
		FlagGuard guard;
		interruptionPoint();
		cv.wait_for(lk, std::chrono::milliseconds(1));
		interruptionPoint();
	}

	template<typename Predicate>
	void interruptibleWait(std::condition_variable& cv, std::unique_lock<std::mutex>& lk, Predicate pred)
	{
		interruptionPoint();
		gInterruptedFlag.setCV(cv);
		FlagGuard guard;
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

	class JThread
	{
	private:
		std::thread mT;
		InterruptFlag* mFlag;

	public:
		// Constructors
		JThread() noexcept = default;
		
		template<typename Callable, typename... Args>
		explicit JThread(Callable&& func, Args&&... args)
		{
			using return_type = std::result_of_t<Callable(Args...)>;
			// TODO: can't catch if using std::packaged_task
			//auto task = std::make_shared<std::packaged_task<return_type()> >(std::bind(std::forward<Callable>(func), std::forward<Args>(args)...));
			auto f = std::bind(std::forward<Callable>(func), std::forward<Args>(args)...);

			std::promise<InterruptFlag*> p;
			mT = std::thread([f, &p] {
				p.set_value(&gInterruptedFlag);
				try
				{
					//(*task)();
					f();
				}
				catch (const tvp::ThreadInterrupted& e)
				{
					gLogger.debug(e.what());
				}
			});
			mFlag = p.get_future().get();
		}
		
		explicit JThread(std::thread other) noexcept :
			mT(std::move(other))
		{}
		
		JThread(JThread&& other) noexcept :
			mT(std::move(other.mT))
		{}

		// Destructor
		~JThread() noexcept
		{
			join();
			mFlag = nullptr;
		}

		// Operators
		JThread& operator=(JThread&& other) noexcept
		{
			join();
			mT = std::move(other.mT);
			return *this;
		}

		JThread& operator=(std::thread other) noexcept
		{
			join();
			mT = std::move(other);
			return *this;
		}
		// Methods
		void swap(JThread& other) noexcept
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
			if (joinable())
			{
				mT.join();
			}
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
		JThread(JThread&) = delete;
		JThread& operator=(JThread&) = delete;

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
