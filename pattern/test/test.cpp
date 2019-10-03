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
	delete (tvp::relax::Singleton::getInstance());
	delete (tvp::acqrel::Singleton::getInstance());
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

//*************** Lock *****************
std::chrono::duration<double> getTimeLock()
{
	auto begin = std::chrono::system_clock::now();
	for (size_t i = 0; i <= tenMill; ++i)
	{
		tvp::lock::Singleton::getInstance();
	}
	return std::chrono::system_clock::now() - begin;
};
void testLock()
{
	auto fut1 = std::async(std::launch::async, getTimeLock);
	auto fut2 = std::async(std::launch::async, getTimeLock);
	auto fut3 = std::async(std::launch::async, getTimeLock);
	auto fut4 = std::async(std::launch::async, getTimeLock);
	auto fut5 = std::async(std::launch::async, getTimeLock);
	auto fut6 = std::async(std::launch::async, getTimeLock);
	auto fut7 = std::async(std::launch::async, getTimeLock);
	auto fut8 = std::async(std::launch::async, getTimeLock);
	auto fut9 = std::async(std::launch::async, getTimeLock);
	auto fut10 = std::async(std::launch::async, getTimeLock);
	auto fut11 = std::async(std::launch::async, getTimeLock);
	auto fut12 = std::async(std::launch::async, getTimeLock);

	auto total = fut1.get() + fut2.get() + fut3.get() + fut4.get() +
		fut5.get() + fut6.get() + fut7.get() + fut8.get() +
		fut9.get() + fut10.get() + fut11.get() + fut12.get();

	std::cout << total.count() << std::endl;
}

//*************** Sequential consistency *****************
std::chrono::duration<double> getTimeSeqCst()
{
	auto begin = std::chrono::system_clock::now();
	for (size_t i = 0; i <= tenMill; ++i)
	{
		tvp::seqcst::Singleton::getInstance();
	}
	return std::chrono::system_clock::now() - begin;
};
void tesSeqCst()
{
	auto fut1 = std::async(std::launch::async, getTimeSeqCst);
	auto fut2 = std::async(std::launch::async, getTimeSeqCst);
	auto fut3 = std::async(std::launch::async, getTimeSeqCst);
	auto fut4 = std::async(std::launch::async, getTimeSeqCst);
	auto fut5 = std::async(std::launch::async, getTimeSeqCst);
	auto fut6 = std::async(std::launch::async, getTimeSeqCst);
	auto fut7 = std::async(std::launch::async, getTimeSeqCst);
	auto fut8 = std::async(std::launch::async, getTimeSeqCst);
	auto fut9 = std::async(std::launch::async, getTimeSeqCst);
	auto fut10 = std::async(std::launch::async, getTimeSeqCst);
	auto fut11 = std::async(std::launch::async, getTimeSeqCst);
	auto fut12 = std::async(std::launch::async, getTimeSeqCst);

	auto total = fut1.get() + fut2.get() + fut3.get() + fut4.get() +
		fut5.get() + fut6.get() + fut7.get() + fut8.get() +
		fut9.get() + fut10.get() + fut11.get() + fut12.get();

	std::cout << total.count() << std::endl;
}
//*************** Relax *****************
std::chrono::duration<double> getTimeRelax()
{
	auto begin = std::chrono::system_clock::now();
	for (size_t i = 0; i <= tenMill; ++i)
	{
		tvp::relax::Singleton::getInstance();
	}
	return std::chrono::system_clock::now() - begin;
};
void testRelax()
{
	auto fut1 = std::async(std::launch::async, getTimeRelax);
	auto fut2 = std::async(std::launch::async, getTimeRelax);
	auto fut3 = std::async(std::launch::async, getTimeRelax);
	auto fut4 = std::async(std::launch::async, getTimeRelax);
	auto fut5 = std::async(std::launch::async, getTimeRelax);
	auto fut6 = std::async(std::launch::async, getTimeRelax);
	auto fut7 = std::async(std::launch::async, getTimeRelax);
	auto fut8 = std::async(std::launch::async, getTimeRelax);
	auto fut9 = std::async(std::launch::async, getTimeRelax);
	auto fut10 = std::async(std::launch::async, getTimeRelax);
	auto fut11 = std::async(std::launch::async, getTimeRelax);
	auto fut12 = std::async(std::launch::async, getTimeRelax);

	auto total = fut1.get() + fut2.get() + fut3.get() + fut4.get() +
		fut5.get() + fut6.get() + fut7.get() + fut8.get() +
		fut9.get() + fut10.get() + fut11.get() + fut12.get();

	std::cout << total.count() << std::endl;
}

//*************** Acquire release *****************
std::chrono::duration<double> getTimeAcqRel()
{
	auto begin = std::chrono::system_clock::now();
	for (size_t i = 0; i <= tenMill; ++i)
	{
		tvp::acqrel::Singleton::getInstance();
	}
	return std::chrono::system_clock::now() - begin;
};
void testAcqRel()
{
	auto fut1 = std::async(std::launch::async, getTimeAcqRel);
	auto fut2 = std::async(std::launch::async, getTimeAcqRel);
	auto fut3 = std::async(std::launch::async, getTimeAcqRel);
	auto fut4 = std::async(std::launch::async, getTimeAcqRel);
	auto fut5 = std::async(std::launch::async, getTimeAcqRel);
	auto fut6 = std::async(std::launch::async, getTimeAcqRel);
	auto fut7 = std::async(std::launch::async, getTimeAcqRel);
	auto fut8 = std::async(std::launch::async, getTimeAcqRel);
	auto fut9 = std::async(std::launch::async, getTimeAcqRel);
	auto fut10 = std::async(std::launch::async, getTimeAcqRel);
	auto fut11 = std::async(std::launch::async, getTimeAcqRel);
	auto fut12 = std::async(std::launch::async, getTimeAcqRel);

	auto total = fut1.get() + fut2.get() + fut3.get() + fut4.get() +
		fut5.get() + fut6.get() + fut7.get() + fut8.get() +
		fut9.get() + fut10.get() + fut11.get() + fut12.get();

	std::cout << total.count() << std::endl;
}

//*************** Spin lock *****************
std::chrono::duration<double> getTimeSpin()
{
	auto begin = std::chrono::system_clock::now();
	for (size_t i = 0; i <= tenMill; ++i)
	{
		tvp::spinlock::Singleton::getInstance();
	}
	return std::chrono::system_clock::now() - begin;
};
void testSpinlock()
{
	auto fut1 = std::async(std::launch::async, getTimeSpin);
	auto fut2 = std::async(std::launch::async, getTimeSpin);
	auto fut3 = std::async(std::launch::async, getTimeSpin);
	auto fut4 = std::async(std::launch::async, getTimeSpin);
	auto fut5 = std::async(std::launch::async, getTimeSpin);
	auto fut6 = std::async(std::launch::async, getTimeSpin);
	auto fut7 = std::async(std::launch::async, getTimeSpin);
	auto fut8 = std::async(std::launch::async, getTimeSpin);
	auto fut9 = std::async(std::launch::async, getTimeSpin);
	auto fut10 = std::async(std::launch::async, getTimeSpin);
	auto fut11 = std::async(std::launch::async, getTimeSpin);
	auto fut12 = std::async(std::launch::async, getTimeSpin);

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
				3:\tRelax Singleton\n\
				4:\tAcquire release Singleton\n\
				5:\tSequential consistency Singleton\n\
				6:\ttonce_flag Singleton\n\
				7:\tSpin lock Singleton\n\
				8:\tLock mutex Singleton\n\
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
				testRelax();
			}
			else if (s == "4")
			{
				testAcqRel();
			}
			else if (s == "5")
			{
				tesSeqCst();
			}
			else if (s == "6")
			{
				testOnceFlag();
			}
			else if (s == "7")
			{
				testSpinlock();
			}
			else if (s == "8")
			{
				testLock();
			}
			else if (s == "a")
			{
				std::cout << "Single thread:\t\t\t\t"; testSingleThread(); std::cout << std::endl;
				std::cout << "Scott Mayer:\t\t\t\t"; testScottMayer(); std::cout << std::endl;
				std::cout << "memory order relaxed:\t\t\t"; testRelax(); std::cout << std::endl;
				std::cout << "memory order acquire-release:\t\t"; testAcqRel(); std::cout << std::endl;
				std::cout << "Memory order Sequential consistency:\t"; tesSeqCst(); std::cout << std::endl;
				std::cout << "Function onceflag:\t\t\t"; testOnceFlag(); std::cout << std::endl;
				std::cout << "Spin lock:\t\t\t\t"; testSpinlock(); std::cout << std::endl;
				std::cout << "Mutex lock:\t\t\t\t"; testLock(); std::cout << std::endl;
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
