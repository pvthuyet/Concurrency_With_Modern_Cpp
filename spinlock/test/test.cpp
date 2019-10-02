// test.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include <iostream>
#include "../Spinlock.h"
#include <thread>

tvp::lockfree::Spinlock spin;
int data = 0;

void workOnResource(int v)
{
	for (int i = 0; i < 10; ++i)
	{
		{
			//tvp::lockfree::LockGuard lk(spin);
			data += v;
			std::cout << data << std::endl;
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(1000));
	}
}

void print()
{
	while (data > 0)
	{
		{
			tvp::lockfree::LockGuard lk(spin);
			std::cout << data << std::endl;
			--data;
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(2000));
	}
}

int main()
{
	std::thread t1(workOnResource,1);
	std::thread t2(workOnResource, 1);
	std::thread t3(workOnResource, 1);
	std::thread t4(workOnResource, 1);
	std::thread t5(workOnResource, 1);
	std::thread t6(workOnResource, 1);
	std::thread t7(workOnResource, 1);
	std::thread t8(workOnResource, 1);
	std::thread t9(workOnResource, 1);
	std::thread t10(workOnResource, 1);
	std::thread t11(workOnResource, 1);
	std::thread t12(workOnResource, 1);

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
