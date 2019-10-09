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

#### 1. Strong Memory Model (refer to  sequential consistency semantic `memory_order_seq_cst`)
Sequential consistency provides two guarantees:  
* The instructions of a program are executed in source code order.
* There is a global order of all operations on all threads.
  
![1](https://github.com/pvthuyet/Modern-Cplusplus/blob/master/resources/strongmemorymodel.png) 
  
#### 2. Weak Memory Model (refer to relaxed semantic `memory_order_relaxed`)
* The counter-intuitive behaviour is that thread 1 can see the operations of thread 2 in a different orderr, so there is no view of a global clock.  
Fro example, from the perspective of thread 1, it is possible that the operation `res2= x.load()` overtakes `y.store(1)`.
* It is even possible that thread 1 or thread 2 do not perform their operations in the order defined in the source code.  
For example, thread 2 can first execute `res2= x.load()` and then `y.store(1)`.  
This quite very very difficult to understand  
[cppreference](https://en.cppreference.com/w/cpp/atomic/memory_order#Relaxed_ordering)  
[disscus](https://stackoverflow.com/questions/35648936/reordering-and-memory-order-relaxed)
  
## III. Multithreading
#### 1. Threads
#### 2. Shared Data
* Mutexes: `std::mutex`, `std::shared_mutex`, `std::shared_timed_mutex`, `std::recursive_mutex`
* Locks: `std::lock_guard`, `std::unique_lock`, `std::scoped_lock`, `std::shared_lock`
#### 3. Thread-Local Data
Thread-local data, also known as thread-local storage, is created for each thread separately. 
#### 4. Condition Variables
Condition variables enable threads to be synchronised via messages. 
* The Predicate
* Lost Wakeup
* Spurious Wakeup
* The Wait Workflow
```
std::unique_lock<std::mutex> lck(mutex_);
condVar.wait(lck, []{ return dataReady; });
```
Equal
```
std::unique_lock<std::mutex> lck(mutex_);
while ( ![]{ return dataReady; }() {
condVar.wait(lck);
}
```
Even if the shared variable is atomic, it must be modified under the mutex to publish the modification to the waiting thread correctly.  
**Use a mutex to protect the shared variable**  
Even if you make dataReady an atomic, it must be modified under the mutex;  
if not the modification to the waiting thread may be published, but not correctly synchronised.  
This race condition may cause a deadlock.  
What does that mean: published, but not correctly synchronised.  
Let’s have once more a closer look at the wait workflow and assume that `deadReady` is an atomic and is modified not protected by the mutex mutex_.  
```
std::unique_lock<std::mutex> lck(mutex_);
while ( ![]{ return dataReady.load(); }() {
// time window
condVar.wait(lck);
}
```
Let me assume the notification is send while the condition variable condVar is not in the waiting state.  
This means execution of the thread is in the source snippet between line 2 and 4 (see the comment time window).  
The result is that the notification is lost.  
Afterwards the thread goes back in the waiting state and presumably sleeps forever.  
This wouldn’t have happened if dataReady had been protected by a mutex. 
Because of the synchronisation with the mutex, the notification would only be sent if the condition variable the notification would only be sent if the condition variable  
  
#### 5. Tasks
![1](https://github.com/pvthuyet/Modern-Cplusplus/blob/master/resources/task.png)  
  
* **Tasks versus Threads**
* **std::async**
* **std::packaged_task**
* **std::promise and std::future**  
If a future fut asks for the result more than once, a std::future_error exception is thrown.  
There is a  `One-to-one` relationship between the promise and the future.
* **std::shared_future**  
`One-to-many` relationship between a promise and many futures.
* **Exceptions**
* **Notifications**  
`Condition variables` to synchronise threads multiple times.  
A `promise` can send its notification only once.  
`promise` and `future` is the first choice
```
	void waitForWork(std::future<void> && fut)
	{
		std::cout << "Worker: Waiting for work." << std::endl;
		fut.wait();
		std::cout << "work done\n";
	}
	void setReady(std::promise<void> &&pro)
	{
		std::cout << "Send data is ready.\n";
		pro.set_value();
	}
	void test()
	{
		using namespace std::chrono_literals;
		std::promise<void> pro;
		std::future<void> fut = pro.get_future();
		std::thread t1(waitForWork, std::move(fut));
		std::this_thread::sleep_for(2s);
		std::thread t2(setReady, std::move(pro));
		t1.join();
		t2.join();
	}
```
  
## IV. Challenges
### 1. ABA Problem
ABA means you read a value twice and each time it returns the same value A.  
Therefore you conclude that nothing changed in between.  
However, you missed the fact that the value was updated to B somewhere in between.
...
### 2. Blocking Issues
...
### 3. Breaking of Program Invariants
...
### 4. Data Race
...
### 5. Deadlocks
#### 1. Problem: Lock Mutexes in Different Order
![](https://github.com/pvthuyet/Modern-Cplusplus/blob/master/resources/deadlock.png)
```
void deadlock(std::mutex& a, std::mutex& b) {
    std::lock_guard<std::mutex> g1(a);
    std::lock_guard<std::mutex> g2(b);
    // do something here.
}
int main() {
    std::mutex m1, m2;
    std::thread t1(deadlock, std::ref(m1), std::ref(m2));
    std::thread t2(deadlock, std::ref(m2), std::ref(m1));
    return 0;
}
```
#### 2. Solution
* **Keep in mind only lock as soon as needed**
* **Avoid necked Mutex**
* **Avoid nested blocks:**  
Don’t acquire a lock if you already hold one.
* **Avoid calling user-supplied code while holding a lock**  
Because the code is user supplied, you have no idea what it could do; it could do anything, including acquiring a lock.  
* **Aquire locks in a fixed order**  
Using std::lock
* **Use a lock hierarchy**
* **Fix deadlock using std::lock and std::scoped_lock**  
Use std::unique_lock  
```
void fixDeadlock(std::mutex& a, std::mutex& b) {
    std::unique_lock<std::mutex> g1(a, std::defer_lock);
    std::unique_lock<std::mutex> g1(b, std::defer_lock);
    std::lock(g1,g2);
    // do something here.
}
```
or use std::lock_guard  
```
void fixDeadlock(std::mutex& a, std::mutex& b) {
    std::lock(a, b);
    std::lock_guard<std::mutex> g1(a, std::adopt_lock); // to make sure a will be released
    std::lock_guard<std::mutex> g1(b, std::adopt_lock); // to make sure b will be released
    // do something here.
}
```
or use std::scoped_lock
```
void fixDeadlock(std::mutex& a, std::mutex& b) {
    std::scoped_lock scoLock(a, b);
    // do something here.
}
```
### 6. False Sharing
...
### 7. Lifetime Issues of Variables
...
### 8. Moving Threads
...
### 9. Moving Threads
....
  
  
## C++11 Books:
![1](https://github.com/pvthuyet/Modern-Cplusplus/blob/master/resources/ConcurrencyCoverFrame.png)  
  
![2](https://github.com/pvthuyet/Modern-Cplusplus/blob/master/resources/effectivec%2B%2B.jpg)  
  
![3](https://github.com/pvthuyet/Modern-Cplusplus/blob/master/resources/C-Concurrency-in-Action-2nd-Edition.jpg)    
  

