#include "pch.h"
#include "TestThreadPool.h"
#include "../../logger/Logger.h"
#include "../../threadpool/JThreadPool.h"

namespace tvp
{
	void testThreadPool()
	{
		tvp::Logger* gLogger = tvp::Logger::getInstance();
		gLogger->debug("******* ThreadPool is nonstop(Press `q` to stop) *****\n", true, true);
		tvp::JThreadPool pool(4U);

		auto fun = [](int i) {
			tvp::Logger* gLogger = tvp::Logger::getInstance();
			gLogger->debug(tvp::Utils::getThreadId() + " processing " + std::to_string(i) + "\n");
			std::this_thread::sleep_for(std::chrono::milliseconds(2000));
		};
		auto push = [&fun, &pool]() {
			int i = 0;
			while (true)
			{
				try
				{
					pool.submit(fun, i);
					i++;
				}
				catch (tvp::JException const& e)
				{
					tvp::Logger* gLogger = tvp::Logger::getInstance();
					gLogger->debug(e.what());
					switch (e.getCode())
					{
					case tvp::ExceptionCode::QUEUE_LIMIT:
						std::this_thread::sleep_for(std::chrono::milliseconds(2000));
						break;
					case tvp::ExceptionCode::QUEUE_SHUTDOWN:
					case tvp::ExceptionCode::THREADPOOL_SHUTDOWN:
						return;
					default:
						break;
					}
				}

				std::this_thread::yield();
			}
		};
		auto stop = [&pool]() {
			std::string s;
			while (!pool.isShutdown())
			{
				tvp::Logger* gLogger = tvp::Logger::getInstance();
				gLogger->debug("Press 'q' to exit: \n", false, true);
				std::cin >> s;
				if (s == "q" || s == "q")
				{
					pool.shutdown();
					break;
				}
			}
		};

		tvp::JThread t1(push);
		tvp::JThread t2(stop);
		t1.join();
		t2.join();
	}
}