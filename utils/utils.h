#pragma once
#include <string>
#include <thread>
#include <sstream>
#include <random>
#include <list>
#include <vld.h>

namespace tvp
{
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

		template<typename T>
		static T randomNum(T min = 0, T max = (std::numeric_limits<T>::max)())
		{
			std::random_device rd;
			std::uniform_int_distribution<T> dist(min, max);
			return dist(rd);
		}

		template<typename T>
		static std::list<T> random(std::size_t size, 
			T min = 0, 
			T max = (std::numeric_limits<T>::max)()) 
		{
			std::random_device rd;
			std::list<T> lst;
			std::uniform_int_distribution<T> dist(min, max);
			for (std::size_t i = 0; i < size; ++i) {
				lst.emplace_front(dist(rd)); // note: demo only: the performance of many
											 // implementations of random_device degrades sharply
											 // once the entropy pool is exhausted. For practical use
											 // random_device is generally only used to seed
											 // a PRNG such as mt19937
			}
			return lst;
		}
	};
}