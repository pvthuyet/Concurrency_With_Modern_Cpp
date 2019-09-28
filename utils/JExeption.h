#pragma once
#include <string>
#include <thread>
#include <sstream>
#include <exception>
#include <random>
#include <list>

namespace tvp
{
	enum class ExceptionCode
	{
		TIME_OUT,
		QUEUE_SHUTDOWN,
		QUEUE_LIMIT,
		THREAD_INTERRUPTED,
		THREADPOOL_SHUTDOWN,
	};

	class JException : public std::exception
	{
	private:
		ExceptionCode mExCode;
	public:
		explicit JException(ExceptionCode const& exCode, std::string const& msg) noexcept
			: std::exception(msg.c_str()),
			mExCode(exCode)
		{
		}
		ExceptionCode getCode() const noexcept
		{
			return mExCode;
		}
	};
}