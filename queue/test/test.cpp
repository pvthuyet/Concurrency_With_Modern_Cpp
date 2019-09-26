// test.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include <iostream>
#include "../JQueue.h"
#include "../../logger/logger.h"
#include "../../utils/utils.h"
#include "../../thread/JThread.h"
#include <vld.h>

static tvp::Logger gLogger;

void testQueue()
{
	tvp::JQueue<int> que;
	auto pushF = [&que](int s) {
		std::mutex mut;
		std::condition_variable cv;
		std::unique_lock<std::mutex> lk(mut);

		int i = s;
		while (true)
		{
			tvp::interruptibleWait(cv, lk);
			try
			{
				que.push(i);
			}
			catch (const tvp::JQueue<int>::QueueException& e)
			{
				gLogger.debug(e.what());
				return;
			}
			i++;
			gLogger.debug(tvp::Utils::getThreadId() 
				+ " push " 
				+ std::to_string(i) 
				+ " into queue (" 
				+ std::to_string(que.size()) + ")\n");
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
		}
	};

	auto popF = [&que]() {
		std::mutex mut;
		std::condition_variable cv;
		std::unique_lock<std::mutex> lk(mut);

		while (true)
		{
			tvp::interruptibleWait(cv, lk);
			int v;
			try
			{
				que.waitAndPop(v);
			}
			catch (const tvp::JQueue<int>::QueueException& e)
			{
				gLogger.debug(e.what());
				return;
			}
			gLogger.debug(tvp::Utils::getThreadId()
				+ " pop "
				+ std::to_string(v)
				+ " out of queue ("
				+ std::to_string(que.size()) + ")\n");
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
		}
	};

	// All threads must destroy befor queue
	{
		constexpr std::size_t N = 32;
		constexpr std::size_t M = 17;
		std::vector<std::unique_ptr<tvp::JThread> > threads;
		for (unsigned int i = 0; i < N; ++i)
		{
			if (i % 3 == 0)
				threads.emplace_back(std::make_unique<tvp::JThread>(popF));
			else
				threads.emplace_back(std::make_unique<tvp::JThread>(pushF, i * 1000));
		}

		for (unsigned int i = 0; i < M; ++i)
		{
			if (i % 2 == 0)
				threads.emplace_back(std::make_unique<tvp::JThread>(popF));
			else
				threads.emplace_back(std::make_unique<tvp::JThread>(pushF, (i + N) * 1000));
		}

		auto fstop = [&threads, &que]() {
			std::this_thread::sleep_for(std::chrono::milliseconds(30000));
			que.shutdown();
			for (unsigned int i = 0; i < threads.size(); ++i)
			{
				std::this_thread::sleep_for(std::chrono::milliseconds(100));
				gLogger.debug(tvp::Utils::getThreadId() + " STOP THREAD " + std::to_string(i) + "\n");
				threads[i]->interrupt();
			}
			for (unsigned int i = 0; i < threads.size(); ++i)
			{
				threads[i]->join();
			}
			// Queue must be shutdown before all thread join
			
		};

		tvp::JThread t(fstop);
		t.join();
	}
}

int main()
{
	testQueue();
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
