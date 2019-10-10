#pragma once
#include <functional>
#include <chrono>
#include <thread>
#include <utility>
#include <numeric>
#include "../JThreadPool.h"
#include "../../logger/Logger.h"

namespace tvp
{
	template<typename Iterator, typename T>
	struct AccumulateBlock 
	{
		T operator()(Iterator first, Iterator last) 
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(1000));
			print(first, last);
			return std::accumulate(first, last, T());
		}

		void print(Iterator first, Iterator last)
		{
			tvp::Logger* gLogger = tvp::Logger::getInstance();
			T val = std::accumulate(first, last, T());
			gLogger->debug(tvp::Utils::getThreadId() +
				"\t" +
				std::to_string(*first) +
				" + ... + " +
				std::to_string(*(last-1)) +
				"\t= " + std::to_string(val)
				+ "\n",
				false, true);

		}
	};

	template<typename Iterator, typename T>
	T parallelAccumulate(Iterator first, Iterator last, T init) 
	{
		std::size_t const length = std::distance(first, last);
		if (!length)
			return init;

		std::size_t const blockSize = 25;
		std::size_t const numThreads = 4;// std::thread::hardware_concurrency();
		std::size_t const numBlocks = (length + blockSize - 1) / (blockSize);
		std::vector<std::future<T> > futures(numBlocks - 1);
		JThreadPool pool(numThreads);

		Iterator blockStart = first;
		for (std::size_t i = 0; i < (numBlocks - 1); ++i) 
		{
			//T val;
			Iterator blockEnd = blockStart;
			std::advance(blockEnd, blockSize);
			bool success = false;
			while (!success)
			{
				try
				{
					futures[i] = pool.submit(AccumulateBlock<Iterator, T>(), blockStart, blockEnd);
					success = true;
				}
				catch (JException const& e)
				{
					tvp::Logger* gLogger = tvp::Logger::getInstance();
					gLogger->debug(e.what());
				}
			}
			blockStart = blockEnd;
		}
		T lastResult = AccumulateBlock<Iterator, T>()(blockStart, last);
		T result = init;
		for (std::size_t i = 0; i < (numBlocks - 1); ++i) 
		{
			result += futures[i].get();
		}
		result += lastResult;

		return result;
	}

	void parallelAccumulate()
	{
		tvp::Logger* gLogger = tvp::Logger::getInstance();
		constexpr int N = 1000;
		std::vector<int> vt(N);
		std::iota(std::begin(vt), std::end(vt), 1);
		int init = 0;
		int result = parallelAccumulate<std::vector<int>::iterator, int>(std::begin(vt), std::end(vt), init);
		gLogger->debug("Parallel accumulate : " + 
			std::to_string(vt[0]) + 
			" + ... + " + 
			std::to_string(vt[N-1]) + 
			" = " + 
			std::to_string(result) +
			"\n",
			true, 
			true);
	}
}
