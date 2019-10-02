// test.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include <iostream>
#include <csignal>
#include <chrono>
#include "../Singleton.h"
#include "../../thread/JThread.h"

constexpr auto tenMill = 10000000;
tvp::Logger gLogger;

void terminal(int signal)
{
	//delete tvp::onceflag::Singleton::getInstance();
}

//*************** Single thread *****************
void testSingleThread()
{
	constexpr auto twelvehundredMill = 12 * tenMill;
	auto begin = std::chrono::system_clock::now();

	for (size_t i = 0; i <= twelvehundredMill; ++i) {
		tvp::singlethread::Singleton::getInstance();
	}

	auto end = std::chrono::system_clock::now() - begin;

	std::cout << std::chrono::duration<double>(end).count() << std::endl;
}

//*************** Scott Mayer *****************
std::chrono::duration<double> getTimeScottMayer()
{
	auto begin = std::chrono::system_clock::now();
	for (size_t i = 0; i <= tenMill; ++i)
	{
		tvp::scottmayer::Singleton::getInstance();
	}
	return std::chrono::system_clock::now() - begin;

};
void testScottMayer()
{
	auto fut1 = std::async(std::launch::async, getTimeScottMayer);
	auto fut2 = std::async(std::launch::async, getTimeScottMayer);
	auto fut3 = std::async(std::launch::async, getTimeScottMayer);
	auto fut4 = std::async(std::launch::async, getTimeScottMayer);
	auto fut5 = std::async(std::launch::async, getTimeScottMayer);
	auto fut6 = std::async(std::launch::async, getTimeScottMayer);
	auto fut7 = std::async(std::launch::async, getTimeScottMayer);
	auto fut8 = std::async(std::launch::async, getTimeScottMayer);
	auto fut9 = std::async(std::launch::async, getTimeScottMayer);
	auto fut10 = std::async(std::launch::async, getTimeScottMayer);
	auto fut11 = std::async(std::launch::async, getTimeScottMayer);
	auto fut12 = std::async(std::launch::async, getTimeScottMayer);

	auto total = fut1.get() + fut2.get() + fut3.get() + fut4.get() +
		fut5.get() + fut6.get() + fut7.get() + fut8.get() +
		fut9.get() + fut10.get() + fut11.get() + fut12.get();

	std::cout << total.count() << std::endl;
}

//*************** Once Flag *****************
std::chrono::duration<double> getTime() 
{
	auto begin = std::chrono::system_clock::now();
	for (size_t i = 0; i <= tenMill; ++i) 
	{
		tvp::onceflag::Singleton::getInstance();
	}
	return std::chrono::system_clock::now() - begin;
};
void testOnceFlag()
{
	auto fut1 = std::async(std::launch::async, getTime);
	auto fut2 = std::async(std::launch::async, getTime);
	auto fut3 = std::async(std::launch::async, getTime);
	auto fut4 = std::async(std::launch::async, getTime);
	auto fut5 = std::async(std::launch::async, getTime);
	auto fut6 = std::async(std::launch::async, getTime);
	auto fut7 = std::async(std::launch::async, getTime);
	auto fut8 = std::async(std::launch::async, getTime);
	auto fut9 = std::async(std::launch::async, getTime);
	auto fut10 = std::async(std::launch::async, getTime);
	auto fut11 = std::async(std::launch::async, getTime);
	auto fut12 = std::async(std::launch::async, getTime);

	auto total = fut1.get() + fut2.get() + fut3.get() + fut4.get() +
		fut5.get() + fut6.get() + fut7.get() + fut8.get() +
		fut9.get() + fut10.get() + fut11.get() + fut12.get();

	std::cout << total.count() << std::endl;
}

int main()
{
	std::cout << std::fixed;
	std::cout.precision(9);
	try
	{
		std::signal(SIGTERM, terminal);
		while (true)
		{
			gLogger.debug("************\n\
				1:\tsingle thread Singleton\n\
				2:\tScott Mayer Singleton\n\
				3:\tonce_flag Singleton\n\
				q:\tQuit\n\
				Choose: ", true, true);
			std::string s;
			std::cin >> s;
			if (s == "1")
			{
				testSingleThread();
			}
			else if (s == "2")
			{
				testScottMayer();
			}
			else if (s == "3")
			{
				testOnceFlag();
			}
			else if (s == "4")
			{
			}
			else if (s == "q" || s == "Q")
			{
				break;
			}
		}

		std::raise(SIGTERM);
	}
	catch (...)
	{
		gLogger.debug("Unknow exception!\n");
	}

	return 0;
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
