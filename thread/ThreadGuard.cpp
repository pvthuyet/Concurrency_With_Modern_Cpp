#include "ThreadGuard.h"
namespace tvp
{
	ThreadGuard::~ThreadGuard() noexcept
	{
		if (mIntThread.joinable())
		{
			mIntThread.join();
		}
	}

	ThreadGuard::ThreadGuard(ThreadGuard&& other) noexcept :
		mIntThread(std::move(other.mIntThread))
	{}

	ThreadGuard& ThreadGuard::operator=(ThreadGuard&& other) noexcept
	{
		if (this == &other)
		{
			return *this;
		}
		mIntThread = std::move(other.mIntThread);
		return *this;
	}

	void ThreadGuard::detach()
	{
		if (mIntThread.joinable())
		{
			mIntThread.detach();
		}
	}
}