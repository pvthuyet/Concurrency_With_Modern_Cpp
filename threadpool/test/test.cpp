// test.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include <iostream>
#include "../JThreadPool.h"
#include "../../logger/logger.h"
#include "../../utils/utils.h"
#include <chrono>
#include <list>
#include "Quicksort.h"
#include "TestThreadPool.h"
#include "Accumulate.h"

tvp::Logger gLogger;

// ****************************************
int main()
{
	try
	{
		while (true)
		{
			gLogger.debug("************\n\
				1:\tQuicksort\n\
				2:\tParallel accumulate\n\
				3:\tTest ThreadPool\n\
				q:\tQuit\n\
				Choose: ", true, true);
			std::string s;
			std::cin >> s;
			if (s == "1")
			{
				tvp::parallelQuickSort();
			}
			else if (s == "2")
			{
				tvp::parallelAccumulate();
			}
			else if (s == "3")
			{
				tvp::testThreadPool();
			}
			else if (s == "q" || s == "Q")
			{
				break;
			}
		}
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
