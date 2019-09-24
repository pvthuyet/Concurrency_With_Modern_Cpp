// test.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include <iostream>
#include <sstream>
#include <string>
#include <memory>
#include "../JThread.h"

std::mutex gMutex;

void print(std::string const& msg)
{
	std::lock_guard<std::mutex> lk(gMutex);
	std::cout << msg << std::flush;
}
void doSomeThing()
{
	std::ostringstream os;
	os << "\t" << std::this_thread::get_id() << " running ...\n";
	print(os.str());
}

void worker_wait_cv(std::string const& msg)
{
	print(msg);
	std::mutex mut;
	std::condition_variable cv;
	std::unique_lock<std::mutex> lk(mut);

	while (true)
	{
		tvp::interruptibleWait(cv, lk);
		doSomeThing();
		std::this_thread::sleep_for(std::chrono::milliseconds(2000));
	}
}

void worker_wait_cv_any(std::string const& msg)
{
	print(msg);
	std::mutex mut;
	std::condition_variable_any cv;
	std::unique_lock<std::mutex> lk(mut);
	while (true)
	{
		tvp::interruptibleWait(cv, lk);
		doSomeThing();
		std::this_thread::sleep_for(std::chrono::milliseconds(2000));
	}
}


void worker_wait_future(std::string const& msg)
{
	print(msg);
	std::future<int> future = std::async(std::launch::async, []() {
		std::this_thread::sleep_for(std::chrono::seconds(3));
		return 8;
	});

	while (true)
	{
		tvp::interruptibleWait(future);
		doSomeThing();
		std::this_thread::sleep_for(std::chrono::milliseconds(2000));
	}
}

void test_interrupted_thread()
{
	constexpr unsigned int N = 10;
	std::vector<std::unique_ptr<tvp::JThread> > threads;

	for (unsigned int i = 0; i < N; ++i)
	{
		switch (i % 3)
		{
		case 0:
			threads.emplace_back(std::make_unique<tvp::JThread>(worker_wait_cv, "worker_wait_cv: Starting\n"));
			break;
		case 1:
			threads.emplace_back(std::make_unique<tvp::JThread>(worker_wait_cv_any, "worker_wait_cv_any: Starting\n"));
			break;
		case 2:
			threads.emplace_back(std::make_unique<tvp::JThread>(worker_wait_future, "worker_wait_future: Starting\n"));
			break;
		default:
			break;
		}
	}

	auto fstop = [&threads]() {
		std::this_thread::sleep_for(std::chrono::milliseconds(5000));
		for (unsigned int i = 0; i < threads.size(); ++i)
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(1000));
			print("STOP THREAD " + std::to_string(i) + "\n");
			threads[i]->interrupt();
		}
	};
	tvp::JThread t(fstop);
	t.join();
}

int main()
{
	test_interrupted_thread();
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
