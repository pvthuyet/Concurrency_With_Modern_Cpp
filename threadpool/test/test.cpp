// test.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include <iostream>
#include "../JThreadPool.h"
#include "../../logger/logger.h"
#include "../../utils/utils.h"
#include <chrono>
#include <list>
#include <vld.h>

tvp::Logger gLogger;

template<typename T>
struct sorter
{
	std::unique_ptr<tvp::JThreadPool> mPool;
	sorter() :
		mPool(std::make_unique<tvp::JThreadPool>(4))
	{
	}

	std::list<T> doSort(std::list<T>& chunkData) {
		gLogger.debug(tvp::Utils::getThreadId() + " sorting...\n");

		std::this_thread::sleep_for(std::chrono::milliseconds(1000));
		if (chunkData.empty() || chunkData.size() == 1) {
			return chunkData;
		}

		std::list<T> result;
		result.splice(result.begin(), chunkData, chunkData.begin());
		T const& partition_val = *result.begin();
		auto divide_point = std::partition(chunkData.begin(), chunkData.end(),
			[&](const T& val) {
			return val < partition_val;
		});
		// Processing Lower partition
		// move lower partition to new_lower_chunk
		std::list<T> new_lower_chunk;
		new_lower_chunk.splice(new_lower_chunk.end(), chunkData, chunkData.begin(), divide_point);
		// push to thread pool queue
		auto cb = std::bind(&sorter::doSort, this, std::placeholders::_1);
		std::future<std::list<T> > new_lower = mPool->submit(cb, new_lower_chunk);

		// processing higher partition
		std::list<T> new_higher(doSort(chunkData));
		result.splice(result.end(), new_higher);

		// wait for finish lower
		while (new_lower.wait_for(std::chrono::seconds(0)) == std::future_status::timeout) {
			mPool->runPendingTask();
		}

		result.splice(result.begin(), new_lower.get());
		return result;
	}
};

template<typename T>
std::list<T> parallelQuickSort(std::list<T> input) {
	if (input.empty() || input.size() == 1) {
		return input;
	}
	sorter<T> a;
	return a.doSort(input);
}

template<typename T>
void print(std::list<T> const& list, std::string const& msg = "")
{
	gLogger.debug(msg);
	for (T v : list)
	{
		gLogger.debug(std::to_string(v) + " ", true);
	}
	gLogger.debug("\n", true);
}

void testThreadpool()
{
	int sz = 50;
	std::list<int> v = tvp::Utils::random(sz, -100, 100);
	print(v, "Input data: ");
	std::list<int> result = parallelQuickSort(v);
	print(result, "Resutl: ");
}

int main()
{
	testThreadpool();
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
