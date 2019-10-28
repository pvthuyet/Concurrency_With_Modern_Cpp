#pragma once
#include <string>
#include <string_view>
#include <iostream>
#include <optional>
#include <charconv>
#include "../../utils/Utils.h"
#include <array>

void testFromChars()
{
	for (auto s : { "10", "42.3", " 077.1", "hello", "0x33" })
	{
		// try to convert s to int and print the result if possible:
		{
			auto i = tvp::Utils::s2i(s);
			if (i)
			{
				std::cout << "convert '" << s << "' to int: " << *i << "\n";
			}
			else
			{
				std::cout << "can't convert '" << s << "' to int\n";
			}
		}
		{
			auto f = tvp::Utils::s2f(s);
			if (f)
			{
				std::cout << "convert '" << s << "' to float: " << *f << "\n";
			}
			else
			{
				std::cout << "can't convert '" << s << "' to float\n";
			}
		}
		{
			auto d = tvp::Utils::s2d(s);
			if (d)
			{
				std::cout << "convert '" << s << "' to double: " << *d << "\n";
			}
			else
			{
				std::cout << "can't convert '" << s << "' to double\n";
			}
		}
		{
			auto ld = tvp::Utils::s2d(s);
			if (ld)
			{
				std::cout << "convert '" << s << "' to long double: " << *ld << "\n";
			}
			else
			{
				std::cout << "can't convert '" << s << "' to long double\n";
			}
		}
	}
}
void testToChars()
{
	constexpr int SIZE = 32;
	std::array<char, SIZE + 1> str;
	int newSz = SIZE;

	// int
	tvp::Utils::i2s(str.data(), newSz, 11);
	std::cout << "int to string " << std::string_view (str.data(), newSz) << std::endl;

	// float
	newSz = SIZE;
	tvp::Utils::f2s(str.data(), newSz, 11.333111f);
	std::cout << "float to string " << std::string_view(str.data(), newSz) << std::endl;

	// double
	newSz = SIZE;
	tvp::Utils::d2s(str.data(), newSz, 3.1234);
	std::cout << "double to string " << std::string_view(str.data(), newSz) << std::endl;

	// long double
	newSz = SIZE;
	tvp::Utils::d2s(str.data(), newSz, 5.987l);
	std::cout << "long double to string " << std::string_view(str.data(), newSz) << std::endl;
}

//*******************************************************
// Check string and string_view behaviour
void* operator new(size_t sz)
{
	std::cout << "new size " << sz << std::endl;
	return malloc(sz);
}
void operator delete(void* p)
{
	std::cout << "delete\n";
	free(p);
}

void operator delete(void* p, size_t sz)
{
	std::cout << "delete size " << sz << std::endl;
	free(p);
}

void riskStrView()
{
	std::string_view sv;
	{
		std::string s = "hello world\n";
		sv = s;
	}
	std::cout << sv;
	std::cout << "finish\n";
}