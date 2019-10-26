#pragma once
#include <future>
#include <type_traits>
#include "../logger/logger.h"
#include "../utils/utils.h"
#include "../utils/JExeption.h"

namespace tvp
{
	class InterruptFlagLockFree
	{
	private:
		std::atomic<bool> mFlag;
		std::condition_variable* mThreadCond;
		tvp::SpinMutex mSpin;

	public:
		InterruptFlagLockFree() noexcept : mThreadCond(nullptr), mFlag(false)
		{}

		void set()
		{
			mFlag.store(true, std::memory_order_relaxed);
			std::lock_guard<tvp::SpinMutex> lk(mSpin);
			if (mThreadCond)
			{
				mThreadCond->notify_all();
			}
		}

		bool isSet() const noexcept
		{
			return mFlag.load(std::memory_order_relaxed);
		}

		void setCV(std::condition_variable& cv)
		{
			std::lock_guard<tvp::SpinMutex> lk(mSpin);
			mThreadCond = &cv;
		}

		void clearCV()
		{
			std::lock_guard<tvp::SpinMutex> lk(mSpin);
			mThreadCond = nullptr;
		}
	};
	
	// Create interrupted flag
	// before C++17: Linked ERROR if declare a variable in header file and included in multiple cpp file
	// thread_local InterruptFlagLockFree gInterruptedFlag;

	// C++17 Fixed: inline 
	inline thread_local InterruptFlagLockFree gInterruptedFlag;

	struct FlagGuard
	{
		~FlagGuard()
		{
			gInterruptedFlag.clearCV();
		}
	};

	inline void interruptionPoint()
	{
		if (gInterruptedFlag.isSet())
		{
			throw tvp::JException(tvp::ExceptionCode::THREAD_INTERRUPTED,
				tvp::Utils::getThreadId() + " interrupted\n");
		}
	}

	template<typename Predicate>
	void interruptibleWait(std::condition_variable& cv, std::unique_lock<std::mutex>& lk, Predicate pred)
	{
		interruptionPoint();
		gInterruptedFlag.setCV(cv);
		FlagGuard guard;
		while (!pred() && !gInterruptedFlag.isSet())
		{
			cv.wait_for(lk, std::chrono::milliseconds(1));
		}
		interruptionPoint();
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
		InterruptFlagLockFree* mFlag;

	public:
		// Constructors
		JThread() noexcept = default;
		
		template<typename Callable, typename... Args>
		explicit JThread(Callable&& func, Args&&... args)
		{
			//using return_type = std::invoke_result_t<Callable, Args...>;
			// TODO: can't catch if using std::packaged_task
			//auto task = std::make_shared<std::packaged_task<return_type()> >(std::bind(std::forward<Callable>(func), std::forward<Args>(args)...));
			auto f = std::bind(std::forward<Callable>(func), std::forward<Args>(args)...);

			std::promise<InterruptFlagLockFree*> p;
			mT = std::thread([f, &p] {
				p.set_value(&gInterruptedFlag);
				try
				{
					//(*task)();
					f();
				}
				catch (const tvp::JException& e)
				{
					tvp::Logger* gLogger = tvp::Logger::getInstance();
					gLogger->debug(e.what());
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
			if (mFlag && joinable())
			{
				mFlag->set();
			}
		}
	};
}
