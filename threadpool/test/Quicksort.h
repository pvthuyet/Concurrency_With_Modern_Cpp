#pragma once
#include <chrono>
#include <list>
#include "../../utils/Utils.h"
#include "../../logger/logger.h"
#include "../../utils/JExeption.h"

namespace tvp
{
	template<typename T>
	struct sorter
	{
		std::unique_ptr<tvp::JThreadPool> mPool;
		sorter() :
			mPool(std::make_unique<tvp::JThreadPool>(4U))
		{
		}
		void shutdown()
		{
			mPool->shutdown();
		}

		std::list<T> doSort(std::list<T>& chunkData)
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(300));
			if (chunkData.empty() || chunkData.size() == 1)
				return chunkData;

			std::list<T> result;
			result.splice(result.begin(), chunkData, chunkData.begin());
			T const& partitionVal = *result.begin();
			auto divide_point = std::partition(chunkData.begin(), chunkData.end(),
				[&](const T& val) {
				return val < partitionVal;
			});
			// Processing Lower partition
			// move lower partition to newLowerChunk
			std::list<T> newLowerChunk;
			newLowerChunk.splice(newLowerChunk.end(), chunkData, chunkData.begin(), divide_point);
			// push to thread pool queue
			auto cb = std::bind(&sorter::doSort, this, std::placeholders::_1);
			std::future<std::list<T> > newLower;
			bool success = false;
			while(!success)
			{
				try
				{
					// Make sure submit success
					newLower = mPool->submit(cb, newLowerChunk);
					success = true;
				}
				catch (tvp::JException const& e)
				{
					tvp::Logger* gLogger = tvp::Logger::getInstance();
					gLogger->debug(e.what());
				}
			}
			// processing higher partition
			std::list<T> new_higher(doSort(chunkData));
			result.splice(result.end(), new_higher);

			// wait for finish lower
			while (newLower.wait_for(std::chrono::seconds(0)) == std::future_status::timeout) 
				mPool->runPendingTask();

			result.splice(result.begin(), newLower.get());
			return result;
		}
	};

	template<typename T>
	std::list<T> parallelQuickSort(std::list<T> input) 
	{
		if (input.empty() || input.size() == 1)
			return input;

		sorter<T> st;
		auto runPool = [&st, &input]() -> std::list<T> {
			return st.doSort(input);
		};

		auto task = std::async(std::launch::async, runPool);
		return task.get();
	}

	template<typename T>
	void print(std::list<T> const& list, std::string const& msg = "")
	{
		tvp::Logger* gLogger = tvp::Logger::getInstance();
		gLogger->debug(msg);
		for (T v : list)
		{
			gLogger->debug(std::to_string(v) + " ", true);
		}
		gLogger->debug("\n", true);
	}

	void parallelQuickSort()
	{
		tvp::Logger* gLogger = tvp::Logger::getInstance();
		gLogger->debug("*******Quicksort*****\n", true, true);
		int sz = 50;
		std::list<int> v = tvp::Utils::random(sz, -100, 100);
		print(v, "Input data: ");
		std::list<int> result = parallelQuickSort(v);
		print(result, "Resutl: ");
	}
}
