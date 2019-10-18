// test.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include <iostream>
#include "../Customer.h"

int main()
{
	std::string name = "Nicolai";
	tvp::Customer c1{ "Nicolai" };
	c1.print();
	
	tvp::Customer c2{ "Nicolai", "Josuttis" };
	c2.print();
	
	tvp::Customer c3{ "Nicolai", "Josuttis", 3 };
	c3.print();
	
	tvp::Customer c4{ name, "Josuttis", 4 };
	c4.print();

	tvp::Customer c5{ c4 };
	c5.print();

	tvp::VIP v1{ "vip 1" };// = "Nicolai";
	v1.print();

	tvp::VIP v2{ v1 };
	v2.print();

	tvp::Customer c6{ v2 };
	c6.print();
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
