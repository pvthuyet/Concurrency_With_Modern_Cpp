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
  
  ##### 9. Some issues
  * C++’s most vexing parse
  
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
![3](https://github.com/pvthuyet/Modern-Cplusplus/blob/master/resources/ConcurrencyCoverFrame.png)  
  
![1](https://github.com/pvthuyet/Modern-Cplusplus/blob/master/resources/effectivec%2B%2B.jpg)  
  
![2](https://github.com/pvthuyet/Modern-Cplusplus/blob/master/resources/C-Concurrency-in-Action-2nd-Edition.jpg)    
  

