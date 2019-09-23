// test.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include <iostream>
#include <string>
#include "../JoiningThread.h"

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

int main()
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

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
