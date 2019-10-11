## [JThread](https://github.com/pvthuyet/Modern-Cplusplus/blob/master/thread/JThread.h) ##
Books:  
* C++ Concurrency in Action, Second Edition: [9 Advanced thread management](https://livebook.manning.com/book/c-plus-plus-concurrency-in-action-second-edition/chapter-9/v-7/68)  
* Effective modern C++: [Chapter 8 The Concurrency API](https://www.aristeia.com/EMC++.html)
  
### Features: ###
#### 1. Joinable ####
Acquisition Is Initialization (RAII) idiom
```
		~JThread() noexcept
		{
			if (joinable())
			{
				join();
			}
			mFlag = nullptr;
		}
```
#### 2.Interruptiable:
* How to stop thread?
* How to stop thread while it's waiting on std::condition_variable ?
* How to stop thread while it's waiting on std::future ?
```
  		template<typename Callable, typename... Args>
		explicit JThread(Callable&& func, Args&&... args)
		{
			using return_type = std::result_of_t<Callable(Args...)>;
			auto task = std::make_shared<std::packaged_task<return_type()> >(std::bind(std::forward<Callable>(func), std::forward<Args>(args)...));

			std::promise<InterruptFlag*> p;
			mT = std::thread([task, &p] {
				p.set_value(&gInterruptedFlag);
				try
				{
					(*task)();
				}
				catch (const tvp::ThreadInterrupted& e)
				{
					std::cout << e.what();
				}
			});
			mFlag = p.get_future().get();
		}
```
```
		void interrupt()
		{
			if (mFlag)
			{
				mFlag->set();
			}
		}
```

#### [Test Example](https://github.com/pvthuyet/Modern-Cplusplus/blob/master/thread/test/test.cpp)
```
#include "JThread.h"
void run(std::string const& msg)
{
	while (true)
	{
		tvp::interruptiblePoint();
		std::cout << msg + ": running...\n";
		std::this_thread::sleep_for(std::chrono::milliseconds(1000));
	}
}
void main()
{
	tvp::JThread t(run, "Hello world!");
	std::this_thread::sleep_for(std::chrono::milliseconds(5000));
	t.interrupt();
	return 0;
}
```
