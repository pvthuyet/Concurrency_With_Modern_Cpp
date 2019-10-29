// test.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include <iostream>
//#include "StringViewExam.h"
#include "AlignedData.h"
#include "TrackNew.h"
#include <new>
#include <cstdlib>
#include <cassert>
#include <vld.h>

using namespace std;

class alignas(32) Vec3dAVX {
	double x, y, z;
};

void testTrackNew()
{
	TrackNew::trace(true);
	auto p = new Vec3dAVX{};
	TrackNew::status();
	delete p;
}

int main()
{
	//testFromChars();
	//testToChars();
	//riskStrView();
	//string_view sv = "abc"s;
	//cout << sv;
	
	//std::cout << "__STDCPP_DEFAULT_NEW_ALIGNMENT__ is " << __STDCPP_DEFAULT_NEW_ALIGNMENT__ << std::endl;
	//std::cout << "sizeof Vec3dAVX: " << sizeof(Vec3dAVX) << std::endl;
	//std::cout << "aligof Vec3dAVX: " << alignof(Vec3dAVX) << std::endl;
	
	//auto pVec = new Vec3dAVX[10];
	//assert(reinterpret_cast<uintptr_t>(pVec) % alignof(Vec3dAVX) == 0);
	//delete[] pVec;

	//auto p2 = new int[10];
	//delete[] p2;

	testTrackNew();
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
