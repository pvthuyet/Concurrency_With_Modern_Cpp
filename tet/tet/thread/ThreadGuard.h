#pragma once
#include <thread>
namespace tet {
	class ThreadGuard
	{
	private:
		std::thread mItnThread;

	public:
		ThreadGuard(std::thread&& t);
		template<class F, class... Args>
			explicit ThreadGuard(F&& f, Args&&... args) :
				mItnThread(std::forward<F>(f), std::forward<Args>(args)...)
		{
		}
		virtual ~ThreadGuard() noexcept;
		// Disable copy
		ThreadGuard(const ThreadGuard&) = delete;
		ThreadGuard& operator=(const ThreadGuard&) = delete;
		// default move semantics
		ThreadGuard(ThreadGuard&&) = default;
		ThreadGuard& operator=(ThreadGuard&&) = default;

		std::thread& get();
	};
}
