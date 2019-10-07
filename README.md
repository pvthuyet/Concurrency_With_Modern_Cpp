## I. C++11 Features
##### 1. Smart Pointers
- `std::unique_ptr`, `std::make_unique`
- `std::shared_ptr`, `std::make_shared`
- `std::weak_ptr`
##### 2. Rvalue references, Move Semantics, and Perfect Forwarding
- `std::move`
- `std::forward`
##### 3. Lambda Expressions
##### 4. Concurrency API
- `std::thread`
- `std::async`
- `std::future`
- `std::promise`
- `std::atomic`
- `std::mutex`
- `std::condition_variable`
##### 5. Variadic templates
##### 6. New containers
- [`std::tuple`](https://en.cppreference.com/w/cpp/utility/tuple)
- [`std::array`](https://en.cppreference.com/w/cpp/container/array)
- [`std::forward_list`](https://en.cppreference.com/w/cpp/container/forward_list)
- [`std::unordered_set`](https://en.cppreference.com/w/cpp/container/unordered_set)
- [`std::unordered_map`](https://en.cppreference.com/w/cpp/container/unordered_map)
- [`std::unordered_multiset`](https://en.cppreference.com/w/cpp/container/unordered_multiset)
- [`std::unordered_multimap`](https://en.cppreference.com/w/cpp/container/unordered_multimap)
##### 7. [Utilities library](https://en.cppreference.com/w/cpp/utility)
- [`std::move_if_noexcept`](https://en.cppreference.com/w/cpp/utility/move_if_noexcept)
- [`std:: initializer_list`](https://en.cppreference.com/w/cpp/utility/initializer_list)
- [`std::hash`](https://en.cppreference.com/w/cpp/utility/hash)
- [`std::regex`](https://en.cppreference.com/w/cpp/regex)
- [`std::declval`](https://en.cppreference.com/w/cpp/utility/declval)
  
##### 8. New Keywords
| New Keyword     | Explain                     | 
| :-------------- | --------------------------- |
| `delete`        | to prevent users call a particular function |
| `default`       |                             |
| `override`      |                             |
| `final`         |                             |
| `noexcept`      |                             | 
| `auto`          |                             |  
| `constexpr`     |                             |  
| `nullptr`       |                             |
| `thread_local`  |                             |
| `using` alias   |                             |
| `decltype`      |                             |
| `enum class`    |                             |

  
![](https://github.com/pvthuyet/Modern-Cplusplus/blob/master/resources/timeline.png)
  
## II. Memory Model
Before C++11, there was only one contract. The C++ language specification did not include multithreading or atomics. There was no memory model.  
With C++11 everything has changed. C++11 is the first standard aware of multiple threads. The reason for the well-defined behaviour of threads is the C++ memory model that was heavily inspired by the [Java memory model](https://en.wikipedia.org/wiki/Java_memory_model)  
```
enum memory_order{
memory_order_relaxed,
memory_order_consume,
memory_order_acquire,
memory_order_release,
memory_order_acq_rel,
memory_order_seq_cst
}
```
* Read operation: `memory_order_acquire` and `memory_order_consume`
* Write operation: `memory_order_release`
* Read-modify-write operation: `memory_order_acq_rel` and `memory_order_seq_cst`
* Relaxed operation: `memory_order_relaxed`, there are no synchronization or ordering constraints imposed on other reads or writes, only this operation's atomicity is guaranteed.  

![1](https://github.com/pvthuyet/Modern-Cplusplus/blob/master/resources/memorymodel_.png)  
![2](https://github.com/pvthuyet/Modern-Cplusplus/blob/master/resources/expertlevel_.png)  

#### Strong Memory Model (refer to  sequential consistency semantic `memory_order_seq_cst`)
Sequential consistency provides two guarantees:  
* The instructions of a program are executed in source code order.
* There is a global order of all operations on all threads.
  
![1](https://github.com/pvthuyet/Modern-Cplusplus/blob/master/resources/strongmemorymodel.png) 
  
#### Weak Memory Model (refer to relaxed semantic `memory_order_relaxed`)
* The counter-intuitive behaviour is that thread 1 can see the operations of thread 2 in a different orderr, so there is no view of a global clock.  
Fro example, from the perspective of thread 1, it is possible that the operation res2= x.load() overtakes y.store(1).
* It is even possible that thread 1 or thread 2 do not perform their operations in the order defined in the source code.  
For example, thread 2 can first execute res2= x.load() and then y.store(1).

## II. Challenges
#### ABA Problem
ABA means you read a value twice and each time it returns the same value A.  
Therefore you conclude that nothing changed in between.  
However, you missed the fact that the value was updated to B somewhere in between.
...
#### Blocking Issues
...
#### Breaking of Program Invariants
...
#### Data Race
...
#### Deadlocks
...
#### False Sharing
...
#### Lifetime Issues of Variables
...
#### Moving Threads
...
#### Moving Threads
....
  
  
## C++11 Books:
![1](https://github.com/pvthuyet/Modern-Cplusplus/blob/master/resources/ConcurrencyCoverFrame.png)  
  
![2](https://github.com/pvthuyet/Modern-Cplusplus/blob/master/resources/effectivec%2B%2B.jpg)  
  
![3](https://github.com/pvthuyet/Modern-Cplusplus/blob/master/resources/C-Concurrency-in-Action-2nd-Edition.jpg)    
  

