#pragma once
#include <thread>
namespace tvp
{
	class ThreadGuard
	{
	private:
		std::thread mIntThread;

	public:
		template<class Func, class... Args>
		explicit ThreadGuard(Func&& f, Args&&... args) :
			mIntThread(std::forward<Func>(f), std::forward<Args>(args)...)
		{
		}

		~ThreadGuard() noexcept;

		// Disable methods
		ThreadGuard() = delete;									// default constructor
		ThreadGuard(ThreadGuard const&) = delete;				// copy constructor
		ThreadGuard& operator=(ThreadGuard const&) = delete;	// assign operator

		// Default methods
		ThreadGuard(ThreadGuard&&) noexcept;				// move constructor
		ThreadGuard& operator=(ThreadGuard&&) noexcept;		// move operator

		void detach();
	};
}