// test.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include <iostream>
#include "../JQueue.h"
#include "../../logger/logger.h"
#include "../../utils/utils.h"
#include "../../thread/JThread.h"
#include "../../utils/JExeption.h"

void testQueue()
{
	tvp::JQueue<int> que;
	auto pushF = [&que](int s) {
		tvp::Logger* gLogger = tvp::Logger::getInstance();
		int i = s;
		while (true)
		{
			// Only quit loop by this function.
			tvp::interruptionPoint();
			try
			{
				que.push(i);				
				gLogger->debug(tvp::Utils::getThreadId() 
					+ " push " 
					+ std::to_string(i) 
					+ " into queue (" 
					+ std::to_string(que.size()) + ")\n");
				i++;
			}
			catch (const tvp::JException& e)
			{
				// Throw exeption but must not stop thread.
				// Becuase of only stop when call interrupt
				gLogger->debug(e.what());
			}

			std::this_thread::sleep_for(std::chrono::milliseconds(500));
		}
	};

	auto popF = [&que]() {
		tvp::Logger* gLogger = tvp::Logger::getInstance();
		while (true)
		{
			tvp::interruptionPoint();
			try
			{
				int v;
				que.waitAndPop(v);
				gLogger->debug(tvp::Utils::getThreadId()
					+ " pop "
					+ std::to_string(v)
					+ " out queue ("
					+ std::to_string(que.size()) + ")\n");
			}
			catch (const tvp::JException& e)
			{
				gLogger->debug(e.what());
			}
			std::this_thread::sleep_for(std::chrono::milliseconds(500));
		}
	};

	// All threads must destroy befor queue
	{		
		constexpr int N = 34;
		constexpr int div = 2;
		std::vector<std::unique_ptr<tvp::JThread> > threads;
		for (int i = 0; i < N; ++i)
		{
			int r = tvp::Utils::randomNum(0, 10);
			if (r % div == 0)
				threads.emplace_back(std::make_unique<tvp::JThread>(popF));
			else
				threads.emplace_back(std::make_unique<tvp::JThread>(pushF, i * 1000));
		}

		auto fstop = [&threads, &que]() {
			tvp::Logger* gLogger = tvp::Logger::getInstance();
			std::this_thread::sleep_for(std::chrono::milliseconds(30000));
			que.shutdown();
			for (int i = 0; i < threads.size(); ++i)
			{
				std::this_thread::sleep_for(std::chrono::milliseconds(100));
				gLogger->debug(tvp::Utils::getThreadId() + " STOP THREAD " + std::to_string(i) + "\n");
				threads[i]->interrupt();
			}
			// Queue must be shutdown after all thread join			
		};

		tvp::JThread t(fstop);
		t.join();		
	}
}

void testFreelockQueue()
{
	tvp::lookfree::referencecount::JQueue<int> que;
}

int main()
{
	tvp::Logger* gLogger = tvp::Logger::getInstance();
	try
	{
		//testQueue();
		testFreelockQueue();
	}
	catch (...)
	{
		gLogger->debug("Unknow exception!\n");
	}

	delete (tvp::Logger::getInstance());
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
