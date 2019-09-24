#pragma once
#include <string>
#include <thread>
#include <sstream>
#include <exception>

namespace tvp
{
	class TimeOut : std::exception
	{
	public:
		virtual char const* what() const
		{
			return "Time out\n";
		}
	};

	class Utils
	{
	public:
		static std::string getThreadId()
		{
			std::ostringstream os;
			os << std::this_thread::get_id();
			return os.str();
		}

		static std::string getDateTime()
		{
			struct tm time_info;
			time_t now = time(0);
			localtime_s(&time_info, &now);
			char str[32] = { 0 };
			strftime(str, 32, "%F_%X", &time_info);
			return std::string(str);
		}
	};
}