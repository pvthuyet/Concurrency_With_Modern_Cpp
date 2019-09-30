// test.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include <iostream>
#include "../Spinlock.h"
#include <thread>

tvp::Spinlock spin;

void workOnResource()
{
	tvp::LockGuard lk(spin);
	std::this_thread::sleep_for(std::chrono::milliseconds(5000));
}

int main()
{
	std::thread t1(workOnResource);
	std::thread t2(workOnResource);
	std::thread t3(workOnResource);
	std::thread t4(workOnResource);
	std::thread t5(workOnResource);
	std::thread t6(workOnResource);
	std::thread t7(workOnResource);
	std::thread t8(workOnResource);
	std::thread t9(workOnResource);
	std::thread t10(workOnResource);
	std::thread t11(workOnResource);
	std::thread t12(workOnResource);

	t1.join();
	t2.join();
	t3.join();
	t4.join();
	t5.join();
	t6.join();
	t7.join();
	t8.join();
	t9.join();
	t10.join();
	t11.join();
	t12.join();
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
