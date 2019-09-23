// Test_Tet.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include <iostream>
#include "tet.h"
#include <string>

void test_queue()
{
	tet::lockbase::Queue<int> q;
	q.push(1);
	q.push(2);
	q.push(3);
	q.push(4);
	// check if empty
	std::cout << std::boolalpha << q.empty() << std::endl;
	// pop1
	std::cout << *(q.tryPop()) << std::endl;
	//pop2
	int pop2;
	q.tryPop(pop2);
	std::cout << pop2 << std::endl;
	//pop3;
	std::cout << *(q.waitAndPop()) << std::endl;
	//pop4
	int pop4;
	q.waitAndPop(pop4);
	std::cout << pop4 << std::endl;

}
void test_thread()
{
	auto f = [](std::string const& name)
	{
		std::cout << "hello thread " << name << std::endl;
	};
	tet::JoiningThread t1(tet::JoiningThread::ThreadType::join, f, "abc");
}
int main()
{
	test_queue();
	test_thread();
    std::cout << "End!\n"; 
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
