## [JQueue](https://github.com/pvthuyet/Modern-Cplusplus/blob/master/queue/JQueue.h) ##
Books:  
* C++ Concurrency in Action, Second Edition: [6 Designing lock-based concurrent data structures](https://livebook.manning.com/book/c-plus-plus-concurrency-in-action-second-edition/chapter-6/v-7/57)  
* Effective modern C++: [Chapter 8 The Concurrency API](https://www.aristeia.com/EMC++.html)
  
### Features: ###
* Thread safe
* Bounded
* concurrent data structure: queue is single linked list with head and tail  
![1](https://github.com/pvthuyet/Modern-Cplusplus/blob/master/queue/06_01.png)
  
### Note
* If `JThread` is an interruptiable thread, it's only exit at the tvp::interruptibleWait(cv, lk). Even JQueue is shutdowned, the JThread must not be stopped.
