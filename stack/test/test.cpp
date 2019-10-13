// test.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include <iostream>
#include "../Stack.h"
#include "../../thread/JThread.h"

constexpr int N = 100000;
void testPending();
void testHazard();
void testSharedPtr();
void testRefCount();

int main()
{
	tvp::Logger* gLogger = tvp::Logger::getInstance();
	while (true)
	{
		gLogger->debug("************\n\
				1:\tPending deleted nodes\n\
				2:\tHazard nodes\n\
				3:\tShared pointer nodes\n\
				4:\tReference counting nodes\n\
				q:\tQuit\n\
				Choose: ", true, true);
		std::string s;
		std::cin >> s;
		if (s == "1")
		{
			testPending();
		}
		else if (s == "2")
		{
			testHazard();
		}
		else if (s == "3")
		{
			testSharedPtr();
		}
		else if (s == "4")
		{
			testRefCount();
		}
		else if (s == "q" || s == "Q")
		{
			break;
		}
	}

	delete (tvp::Logger::getInstance());
	return 0;
}

void testPending()
{
	using namespace std::chrono_literals;
	tvp::lockfree::pending::Stack<int> stack;
	auto push = [&stack](int start) {
		for (int i = start; i < (N + start); ++i)
		{
			stack.push(i);
		}
	};
	auto pop = [&stack]() {
		while (stack.pop());
	};


	int st = 0;
	{
		tvp::JThread t1(push, 0);
		tvp::JThread t2(push, 101);
		tvp::JThread t3(push, 201);
		tvp::JThread t4(push, 301);
		tvp::JThread t5(push, 401);
		std::this_thread::sleep_for(1s);
		tvp::JThread t6(pop);
		tvp::JThread t7(pop);
		tvp::JThread t8(pop);
		tvp::JThread t9(pop);
		tvp::JThread t10(pop);
		tvp::JThread t11(pop);
		tvp::JThread t12(pop);
		tvp::JThread t13(pop);
	}
}

void testHazard()
{
	using namespace std::chrono_literals;
	tvp::lockfree::hazard::Stack<int> stack;
	auto push = [&stack](int start) {
		for (int i = start; i < (N + start); ++i)
		{
			stack.push(i);
		}
	};
	auto pop = [&stack]() {
		while (stack.pop());
	};


	int st = 0;
	{
		tvp::JThread t1(push,0);
		tvp::JThread t2(push,101);
		tvp::JThread t3(push,201);
		tvp::JThread t4(push, 301);
		tvp::JThread t5(push, 401);
		std::this_thread::sleep_for(1s);
		tvp::JThread t6(pop);
		tvp::JThread t7(pop);
		tvp::JThread t8(pop);
		tvp::JThread t9(pop);
		tvp::JThread t10(pop);
		tvp::JThread t11(pop);
		tvp::JThread t12(pop);
		tvp::JThread t13(pop);
	}	
}

void testSharedPtr()
{
	using namespace std::chrono_literals;
	tvp::lockfree::sharedptr::Stack<int> stack;
	auto push = [&stack](int start) {
		for (int i = start; i < (N + start); ++i)
		{
			stack.push(i);
		}
	};
	auto pop = [&stack]() {
		while (stack.pop());
	};


	int st = 0;
	{
		tvp::JThread t1(push, 0);
		tvp::JThread t2(push, 101);
		tvp::JThread t3(push, 201);
		tvp::JThread t4(push, 301);
		tvp::JThread t5(push, 401);
		std::this_thread::sleep_for(1s);
		tvp::JThread t6(pop);
		tvp::JThread t7(pop);
		tvp::JThread t8(pop);
		tvp::JThread t9(pop);
		tvp::JThread t10(pop);
		tvp::JThread t11(pop);
		tvp::JThread t12(pop);
		tvp::JThread t13(pop);
	}
}

void testRefCount()
{
	using namespace std::chrono_literals;
	tvp::lockfree::referencecount::Stack<int> stack;
	auto push = [&stack](int start) {
		for (int i = start; i < (N + start); ++i)
		{
			stack.push(i);
		}
	};
	auto pop = [&stack]() {
		while (stack.pop());
	};


	int st = 0;
	{
		tvp::JThread t1(push, 0);
		tvp::JThread t2(push, 101);
		tvp::JThread t3(push, 201);
		tvp::JThread t4(push, 301);
		tvp::JThread t5(push, 401);
		std::this_thread::sleep_for(1s);
		tvp::JThread t6(pop);
		tvp::JThread t7(pop);
		tvp::JThread t8(pop);
		tvp::JThread t9(pop);
		tvp::JThread t10(pop);
		tvp::JThread t11(pop);
		tvp::JThread t12(pop);
		tvp::JThread t13(pop);
	}
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
