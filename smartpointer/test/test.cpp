// test.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include <vld.h>
#include <iostream>
#include <cstdio>
#include <memory>
#include <vector>

void testDeleter()
{
	// Unique pointer handle FILE
	auto deleter = [](std::FILE* fp) {
		std::fclose(fp);
		fp = nullptr;
		std::cout << "fclose()\n";
	};
	std::FILE* fp = nullptr;
	fopen_s(&fp, "test.txt", "w+");
	std::unique_ptr<std::FILE, decltype(deleter)> upf(fp, deleter);
	const char str[] = "hello world!\n";
	fwrite(str, sizeof(char), sizeof(str), upf.get());

	// Unique pointer and shared pointer handle int
	auto deleter2 = [](int* val) {
		delete val;
		val = nullptr;
	};
	std::shared_ptr<int> spi(new int(1), deleter2);
	std::cout << *spi << std::endl;
	std::unique_ptr<int, decltype(deleter2)> upi(new int(2), deleter2);
	std::cout << *upi << std::endl;
}

class Widget : public std::enable_shared_from_this<Widget>
{
private:
	Widget() = default;

public:
	template<typename... Args>
	static std::shared_ptr<Widget> create(Args&& ...params)
	{
		return std::shared_ptr<Widget>(new Widget(std::forward<Args>(params)...));
	}

	~Widget()
	{
		std::cout << "~Widget\n";
	}

	void process(std::vector<std::shared_ptr<Widget>>& widgets)
	{
		widgets.emplace_back(shared_from_this());
	}
};

void testDuplicateOwnership()
{
	std::vector<std::shared_ptr<Widget>> processedWidget;
	auto spw = Widget::create();
	spw->process(processedWidget);
}

int main()
{
	//testDeleter();
	testDuplicateOwnership();
    //std::cout << "Hello World!\n"; 
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
