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
constexpr int NUM_THREAD = 12;
using chronoDura = std::chrono::duration<double>;
using futDura = std::future<chronoDura>;

void terminal(int signal)
{
	delete (tvp::acqrel::Singleton::getInstance());
	delete (tvp::acqrelspin::Singleton::getInstance());
	delete (tvp::seqcst::Singleton::getInstance());	
	delete (&tvp::onceflag::Singleton::getInstance());
	delete (&tvp::spinlock::Singleton::getInstance());
	delete (&tvp::lock::Singleton::getInstance());	
}

//*************** Single thread *****************
void testSingleThread()
{
	constexpr auto twelvehundredMill = 12 * tenMill;
	auto begin = std::chrono::system_clock::now();

	for (size_t i = 0; i <= twelvehundredMill; ++i) {
		tvp::singlethread::Singleton::getInstance().test();
	}

	auto end = std::chrono::system_clock::now() - begin;

	std::cout << std::chrono::duration<double>(end).count();
}

//*************** Scott Mayer *****************
std::chrono::duration<double> getTimeScottMayer()
{
	auto begin = std::chrono::system_clock::now();
	for (size_t i = 0; i <= tenMill; ++i)
	{
		tvp::scottmayer::Singleton::getInstance().test();
	}
	return std::chrono::system_clock::now() - begin;
};

void testScottMayer()
{
	std::vector<futDura> vt;
	vt.reserve(NUM_THREAD);
	for (int i = 0; i < NUM_THREAD; ++i)
	{
		vt.emplace_back(std::async(std::launch::async, getTimeScottMayer));
	}	

	chronoDura total(0);
	for (int i = 0; i < NUM_THREAD; ++i)
	{	
		total += vt[i].get();
	}
	std::cout << total.count();
}

//*************** Once Flag *****************
std::chrono::duration<double> getTime() 
{
	auto begin = std::chrono::system_clock::now();
	for (size_t i = 0; i <= tenMill; ++i) 
	{
		tvp::onceflag::Singleton::getInstance().test();
	}
	return std::chrono::system_clock::now() - begin;
};
void testOnceFlag()
{
	std::vector<futDura> vt;
	vt.reserve(NUM_THREAD);
	for (int i = 0; i < NUM_THREAD; ++i)
	{
		vt.emplace_back(std::async(std::launch::async, getTime));
	}

	chronoDura total(0);
	for (int i = 0; i < NUM_THREAD; ++i)
	{
		total += vt[i].get();
	}
	std::cout << total.count();
}

//*************** Lock *****************
std::chrono::duration<double> getTimeLock()
{
	auto begin = std::chrono::system_clock::now();
	for (size_t i = 0; i <= tenMill; ++i)
	{
		tvp::lock::Singleton::getInstance().test();
	}
	return std::chrono::system_clock::now() - begin;
};
void testLock()
{
	std::vector<futDura> vt;
	vt.reserve(NUM_THREAD);
	for (int i = 0; i < NUM_THREAD; ++i)
	{
		vt.emplace_back(std::async(std::launch::async, getTimeLock));
	}

	chronoDura total(0);
	for (int i = 0; i < NUM_THREAD; ++i)
	{
		total += vt[i].get();
	}
	std::cout << total.count();
}

//*************** Sequential consistency *****************
std::chrono::duration<double> getTimeSeqCst()
{
	auto begin = std::chrono::system_clock::now();
	for (size_t i = 0; i <= tenMill; ++i)
	{
		tvp::seqcst::Singleton::getInstance()->test();
	}
	return std::chrono::system_clock::now() - begin;
};
void tesSeqCst()
{
	std::vector<futDura> vt;
	vt.reserve(NUM_THREAD);
	for (int i = 0; i < NUM_THREAD; ++i)
	{
		vt.emplace_back(std::async(std::launch::async, getTimeSeqCst));
	}

	chronoDura total(0);
	for (int i = 0; i < NUM_THREAD; ++i)
	{
		total += vt[i].get();
	}
	std::cout << total.count();
}

//*************** Acquire release *****************
std::chrono::duration<double> getTimeAcqRel()
{
	auto begin = std::chrono::system_clock::now();
	for (size_t i = 0; i <= tenMill; ++i)
	{
		tvp::acqrel::Singleton::getInstance()->test();
	}
	return std::chrono::system_clock::now() - begin;
};
void testAcqRel()
{
	std::vector<futDura> vt;
	vt.reserve(NUM_THREAD);
	for (int i = 0; i < NUM_THREAD; ++i)
	{
		vt.emplace_back(std::async(std::launch::async, getTimeAcqRel));
	}

	chronoDura total(0);
	for (int i = 0; i < NUM_THREAD; ++i)
	{
		total += vt[i].get();
	}
	std::cout << total.count();
}

//*************** Acquire release spin *****************
std::chrono::duration<double> getTimeAcqRelSpin()
{
	auto begin = std::chrono::system_clock::now();
	for (size_t i = 0; i <= tenMill; ++i)
	{
		tvp::acqrelspin::Singleton::getInstance()->test();
	}
	return std::chrono::system_clock::now() - begin;
};
void testAcqRelSpin()
{
	std::vector<futDura> vt;
	vt.reserve(NUM_THREAD);
	for (int i = 0; i < NUM_THREAD; ++i)
	{
		vt.emplace_back(std::async(std::launch::async, getTimeAcqRelSpin));
	}

	chronoDura total(0);
	for (int i = 0; i < NUM_THREAD; ++i)
	{
		total += vt[i].get();
	}
	std::cout << total.count();
}

//*************** Spin lock *****************
std::chrono::duration<double> getTimeSpin()
{
	auto begin = std::chrono::system_clock::now();
	for (size_t i = 0; i <= tenMill; ++i)
	{
		tvp::spinlock::Singleton::getInstance().test();
	}
	return std::chrono::system_clock::now() - begin;
};
void testSpinlock()
{
	std::vector<futDura> vt;
	vt.reserve(NUM_THREAD);
	for (int i = 0; i < NUM_THREAD; ++i)
	{
		vt.emplace_back(std::async(std::launch::async, getTimeSpin));
	}

	chronoDura total(0);
	for (int i = 0; i < NUM_THREAD; ++i)
	{
		total += vt[i].get();
	}
	std::cout << total.count();
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
				3:\tAcquire release(spin) Singleton\n\
				4:\tSequential consistency Singleton\n\
				5:\ttonce_flag Singleton\n\
				6:\tSpin lock Singleton\n\
				7:\tLock mutex Singleton\n\
				q:\tQuit\n\
				Choose: ", true, true);
			std::string s;
			std::cin >> s;
			if (s == "1")
			{
				testSingleThread(); std::cout << std::endl;
			}
			else if (s == "2")
			{
				testScottMayer(); std::cout << std::endl;
			}
			else if (s == "3")
			{
				testAcqRelSpin(); std::cout << std::endl;
				testAcqRel(); std::cout << std::endl;
			}
			else if (s == "4")
			{
				tesSeqCst(); std::cout << std::endl;
			}
			else if (s == "5")
			{
				testOnceFlag(); std::cout << std::endl;
			}
			else if (s == "6")
			{
				testSpinlock(); std::cout << std::endl;
			}
			else if (s == "7")
			{
				testLock(); std::cout << std::endl;
			}
			else if (s == "a")
			{
				testSingleThread(); std::cout << "\tSingle thread"; std::cout << std::endl;
				testScottMayer(); std::cout << "\tScott Mayer"; std::cout << std::endl;
				testAcqRelSpin(); std::cout << "\tmemory order acquire-release with Spin lock"; std::cout << std::endl;
				testAcqRel(); std::cout << "\tmemory order acquire-release with mutex lock"; std::cout << std::endl;
				tesSeqCst(); std::cout << "\tMemory order Sequential consistency"; std::cout << std::endl;
				testOnceFlag(); std::cout << "\tFunction onceflag"; std::cout << std::endl;
				testSpinlock(); std::cout << "\tSpin lock"; std::cout << std::endl;
				testLock(); std::cout << "\tMutex lock"; std::cout << std::endl;				
				std::cout << std::endl;
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
