#include "stdafx.h"
#include "ThreadGuard.h"

namespace tet {
	ThreadGuard::ThreadGuard(std::thread&& t) :
		mItnThread(std::move(t))
	{
	}
	ThreadGuard::~ThreadGuard() noexcept
	{
		if (mItnThread.joinable())
		{
			mItnThread.join();
		}
	}
	std::thread & ThreadGuard::get()
	{
		return mItnThread;
	}
}