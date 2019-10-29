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
#include "../../utils/Utils.h"

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

void testFileSize()
{
	std::string path{ "E:\\work" };
	auto beg{ std::chrono::steady_clock::now() };
	auto sz = tvp::Utils::calcSysSize(path);
	std::chrono::duration<double, std::milli> diff{ std::chrono::steady_clock::now() - beg };
	std::cout << "Time " << diff.count() << " milli seconds\n";
	
	constexpr uintmax_t b = 1;
	constexpr uintmax_t Kb = 1024 * b;
	constexpr uintmax_t Mb = 1024 * Kb;
	constexpr uintmax_t Gb = 1024 * Mb;

	std::cout << std::setprecision(4);
	if (sz > Gb)
		std::cout << "Total size of " << std::quoted(path) << ": " << static_cast<double>(sz) / Gb << " Gb\n";
	else if (sz > Mb)
		std::cout << "Total size of " << std::quoted(path) << ": " << static_cast<double>(sz) / Mb << " Mb\n";
	else if (sz > Kb)
		std::cout << "Total size of " << std::quoted(path) << ": " << static_cast<double>(sz) / Kb << " Kb\n";
	else
		std::cout << "Total size of " << std::quoted(path) << ": " << sz << " bytes\n";
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

	//testTrackNew();
	testFileSize();
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
