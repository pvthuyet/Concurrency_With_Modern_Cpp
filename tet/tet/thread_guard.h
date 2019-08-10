#pragma once
#include <thread>
namespace tet {
	class thread_guard
	{
	public:
		enum class thread_type
		{
			join,
			detach
		};
	private:
		std::thread m_t;
		thread_type m_type;

	public:
		thread_guard(std::thread&& t, thread_type typ = thread_type::join);
		template<class Fn,
			class... Args>
			explicit thread_guard(thread_type typ, Fn&& fx, Args&&... ax) :
				m_t(std::forward<Fn>(fx), std::forward<Args>(ax)...),
				m_type(typ)
				
		{
		}
		virtual ~thread_guard() noexcept;
		// Disable copy
		thread_guard(const thread_guard&) = delete;
		thread_guard& operator=(const thread_guard&) = delete;
		// default move semantics
		thread_guard(thread_guard&&) = default;
		thread_guard& operator=(thread_guard&&) = default;

		std::thread& get();
	};
}
