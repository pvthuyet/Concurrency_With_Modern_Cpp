#pragma once
#include <thread>

namespace tvp
{
	class JoiningThread
	{
	private:
		std::thread mT;

	public:
		// Constructors
		JoiningThread() noexcept = default;
		template<typename Callable, typename... Args>
		explicit JoiningThread(Callable&& func, Args&&... args) :
			mT(std::forward<Callable>(func), std::forward<Args>(args)...)
		{}
		explicit JoiningThread(std::thread other) noexcept :
			mT(std::move(other))
		{}
		JoiningThread(JoiningThread&& other) noexcept :
			mT(std::move(other.mT))
		{}

		// Destructor
		~JoiningThread() noexcept
		{
			if (joinable())
			{
				join();
			}
		}

		// Operators
		JoiningThread& operator=(JoiningThread&& other) noexcept
		{
			if (joinable())
			{
				join();
			}
			mT = std::move(other.mT);
			return *this;
		}
		JoiningThread& operator=(std::thread other) noexcept
		{
			if (joinable())
			{
				join();
			}
			mT = std::move(other);
			return *this;
		}
		// Methods
		void swap(JoiningThread& other) noexcept
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
		JoiningThread(JoiningThread&) = delete;
		JoiningThread& operator=(JoiningThread&) = delete;
	};
}