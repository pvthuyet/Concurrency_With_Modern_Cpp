## [JThread](https://github.com/pvthuyet/Modern-Cplusplus/blob/master/thread/JThread.h) ##
Books:  
* [C++ Concurrency in Action, Second Edition](https://livebook.manning.com/book/c-plus-plus-concurrency-in-action-second-edition/chapter-9/v-7/68)  
  
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
* `thread_local InterruptFlag gInterruptedFlag` : Each thread has its own instance of `InterruptFlag` object
* Support wait on std::condition_variable
* Support wait on std::condition_variable_any
* Support wait on std::future<T>
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

### [Test Example](https://github.com/pvthuyet/Modern-Cplusplus/blob/master/thread/test/test.cpp)
