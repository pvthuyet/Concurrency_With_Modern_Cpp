#pragma once
#include <string>
#include <thread>
#include <sstream>
#include <random>
#include <list>
#include <vld.h>
#include <string_view>
#include <optional>
#include <filesystem>
#include <iterator>
#include <charconv>
#include <numeric>
#include <execution>
#include <ostream>

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

	private:
		// convert string to int if possible:
		template<typename T>
		static std::optional<T> fromChars(std::string_view sv)
		{
			T val;
			// read character sequence into the int:
			auto[ptr, ec] = std::from_chars(sv.data(), sv.data() + sv.size(), val);
			// if we have an error code, return no value:
			if (ec != std::errc{}) {
				return std::nullopt;
			}
			return val;
		}

	public:
		static auto s2i(std::string_view sv)
		{
			return fromChars<int>(sv);
		}

		static auto s2f(std::string_view sv)
		{
			return fromChars<float>(sv);
		}

		static auto s2d(std::string_view sv)
		{
			return fromChars<double>(sv);
		}

		static auto s2ld(std::string_view sv)
		{
			return fromChars<long double>(sv);
		}

	private:
		template<typename T>
		static bool toStr(char* str, int& size, T val)
		{
			if (auto[p, ec] = std::to_chars(str, str + size, val); ec == std::errc{})
			{
				*p = '\0'; // ensure a trailing null character is behind
				size = static_cast<int>(p - str);
				return true;
			}
			return false;
		}
	public:
		static bool i2s(char* str, int& size, int val)
		{
			return toStr(str, size, val);
		}
		static bool f2s(char* str, int& size, float val)
		{
			return toStr(str, size, val);
		}
		static bool d2s(char* str, int& size, double val)
		{
			return toStr(str, size, val);
		}
		static bool ld2s(char* str, int& size, long double val)
		{
			return toStr(str, size, val);
		}

		static auto calcSysSize(std::string_view path)
		{
			std::filesystem::path root{ path };
			std::vector<std::filesystem::path> paths;
			try
			{
				std::filesystem::recursive_directory_iterator dirpos{ root };
				std::copy(begin(dirpos), end(dirpos), std::back_inserter(paths));
			}
			catch (std::exception const& e)
			{
				std::cerr << e.what() << std::endl;
				return 0ull;
			}

			return std::transform_reduce(std::execution::par, 
				paths.cbegin(), paths.cend(),
				std::uintmax_t{ 0 },
				std::plus<>(),
				[](const std::filesystem::path& p) {
				return std::filesystem::is_regular_file(p) ? std::filesystem::file_size(p) : std::uintmax_t{ 0 };
			});			
		}

		// print out
		template<typename T>
		static inline void printElements(const T& coll, std::string_view optStr = " ")
		{
			std::cout << optStr;
			for (const auto& elem : coll)
				std::cout << elem << ' ';
			
			std::cout << std::endl;
		}
	};
}