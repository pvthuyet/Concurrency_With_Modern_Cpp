// test.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include <iostream>
#include "../Stack.h"
#include "../../thread/JThread.h"

void testStack();
int main()
{
	testStack();
	return 0;
}

template<typename T>
void push(tvp::lockfree::hazard::Stack<T>& stack, int start)
{
	using namespace std::chrono_literals;
	constexpr int N = 100000;
	for (int i = start; i < (N + start); ++i)
	{
		stack.push(i);
	}
}

template<typename T>
void pop(tvp::lockfree::hazard::Stack<T>& stack)
{
	using namespace std::chrono_literals;
	while(stack.pop())
	{ 
	}
}

void testStack()
{
	using namespace std::chrono_literals;
	tvp::lockfree::hazard::Stack<int> stack;
	int st = 0;
	{
		tvp::JThread t1(push<int>, std::ref(stack), 0);
		tvp::JThread t2(push<int>, std::ref(stack), 101);
		tvp::JThread t3(push<int>, std::ref(stack), 201);
		tvp::JThread t4(push<int>, std::ref(stack), 301);
		tvp::JThread t5(push<int>, std::ref(stack), 401);
		tvp::JThread t6(pop<int>, std::ref(stack));
		tvp::JThread t7(pop<int>, std::ref(stack));
		tvp::JThread t8(pop<int>, std::ref(stack));
		tvp::JThread t9(pop<int>, std::ref(stack));
		tvp::JThread t10(pop<int>, std::ref(stack));
		tvp::JThread t11(pop<int>, std::ref(stack));
		tvp::JThread t12(pop<int>, std::ref(stack));
		tvp::JThread t13(pop<int>, std::ref(stack));
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
