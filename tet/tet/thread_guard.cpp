#include "stdafx.h"
#include "thread_guard.h"

namespace tet {
	thread_guard::thread_guard(std::thread&& t, thread_type typ) :
		m_t(std::move(t)),
		m_type(typ)
	{
	}


	thread_guard::~thread_guard() noexcept
	{
		if (m_t.joinable())
		{
			if (m_type == thread_type::join)
			{
				m_t.join();
			}
			else
			{
				m_t.detach();
			}
		}
	}
	std::thread & thread_guard::get()
	{
		return m_t;
	}
}