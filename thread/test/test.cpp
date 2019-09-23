// test.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include <iostream>
#include <string>
#include "../JoiningThread.h"
#include "../InterruptibleThread.h"

void print(std::string const& msg, std::string const& auth, int& num)
{
	std::cout << msg << " " << auth;
	num = 1;
}

class X
{
public:
	void print(std::string const& msg)
	{
		std::cout << msg;
	}
};

void test_joining_thread()
{
	// Thread function
	int num = 0;
	tvp::JoiningThread t1(print, "Hello World", "from tvp!\n", std::ref(num));
	t1.join();
	std::cout << "num = " << num << std::endl;

	// Thread class function
	X x;
	tvp::JoiningThread t2(&X::print, &x, "Hello Wold from class X!");
}

void loop()
{
	std::mutex mut;
	std::condition_variable cv;
	std::unique_lock<std::mutex> lk(mut);

	while (true)
	{
		tvp::InterruptibleThread::interruptibleWait(cv, lk);
		std::cout << std::this_thread::get_id() << " running ...\n" << std::flush;
		std::this_thread::sleep_for(std::chrono::milliseconds(2000));
	}
}

void test_interrupted_thread()
{
	tvp::InterruptibleThread t1(loop);
	tvp::InterruptibleThread t2(loop);
	// Stop
	while (true)
	{
		std::string stop;
		getline(std::cin, stop);
		if (stop == "e" || stop == "E")
		{
			return;
		}

		if (stop == "1")
		{
			t1.interrupt();
		}
		else if (stop == "2")
		{
			t2.interrupt();
		}
	}
}

int main()
{
	//test_joining_thread();
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
