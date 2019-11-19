// test.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include <iostream>
#include <algorithm>
#include "../include/gsl/span"
#include "../include/gsl/gsl_util"
#include "../../utils/utils.h"
#include "../include/gsl/gsl"

// GSL.view: Views
void printSpan(gsl::span<int> spn)
{
	for (const auto& e : spn)
		std::cout << e << ' ';
	std::cout << std::endl;
}

void testView()
{
	int arr1[] = { 1, 2, 3 };
	int arr2[] = { 3, 4, 5 };
	std::copy_n(arr1, 3, arr2);
	tvp::Utils::printElements(arr2);

	int arr3[] = { 1, 2, 3 };
	int arr4[] = {4, 5, 6};
	gsl::copy(gsl::make_span(arr3, 3), gsl::make_span(arr4, 3));
	//tvp::Utils::printElements(arr4);
	printSpan(gsl::make_span(arr3, 10));
}

int main()
{
	testView();
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
