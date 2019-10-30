## IX. C++17 Features
### Part I: Basic Language Features
#### 1. Structured Bindings
```
	struct MyStruct {
		int i = 0;
		std::string s;
	};
	MyStruct ms;
	auto [u,v] = ms;  // as copy
	auto& [k,l] = ms; // as reference
	...
	std::unordered_map myMap;
	for(const auto& [key, val] : myMap) {
		std::cout << key ": " << val << '\n';
	}
```
#### 2. `If` and `switch` with initialization
#### 3. `inline` variables
Since C++17 you can define a variable/object in a header file as `inline` and if this definition is used by multiple translation units, they all refer to `the same unique object`
```
class MyClass {
	static inline std::string name = ""; // OK since C++17
};
inline MyClass myGlobalObj; // OK even if included/defined by multiple CPP files
```
* `constexpr` now implies `inline` for `static` data member
```
struct D {
	static constexpr int n = 5;
	//inline static constexpr int n = 5; // the same as above
}
```
#### 4. Aggregate extensions
#### 5. Mandatory copy elision or passing unmaterialized objects
Think about below:  
```
class Foo {
};
Foo getFoo() {
  return Foo();   // **copy Elision (Mandatory since C++17)**
}
Foo getFoo() {
  Foo fo;
  return fo; // **NRVO: move constructor**
}
Foo getFoo(const Foo& fo) {
  return fo; // **Copy constructor**
}
Foo getFoo(Foo fo) {
  return fo; // **Move constructor**
}
```
#### 6. Lambda extensions
#### 7. New attributes and attribute features
...  
### Part III: New Library Components
#### 1. `std::optional<>`
`std::optional<>` model a nullable instance of an arbitrary type.  
The instance might be a member, an argument, or a return value.  
You could also argue that a `std::optional<>` is a container for zero or one element.
* Optional Return Values
```
// convert string to int if possible:
std::optional<int> asInt(const std::string& s) {
	std::optional<int> ret; // initially no value
	try {
		ret = std::stoi(s);
	}
	catch (...) {
	}
	return ret;
}
```
* Optional Arguments and Data Members
```
class Name {
	std::string mFirst;
	std::optional<std::string> mMid;
	std::string mLast;
}
```
#### 2. `std::variant<>`
With std::variant<> the C++ standard library provides a new `union class`, which among other benefits supports a new way of polymorphism and dealing with inhomogeneous collections.  
* Using `std::variant<>`
```
std::variant<int, std::string> var{"hi"}; // initialized with string alternative
std::cout << var.index(); // prints 1
var = 42; // now holds int alternative
std::cout << var.index(); // prints 0
```
* std::monostate
To support variants, where the first type has no default constructor, a special helper type is provided: `std::monostate`.  
```
std::variant<std::monostate, NoDefConstr> v2; // OK
```
* Visitors
```
std::variant<int, std::string, double> var(42);
auto printvariant = [](const auto& val) {
	std::cout << val;
	};
std::visit(printvariant, var);
```
* Polymorphism and Inhomogeneous Collections with `std::variant`
```
// common type of all geometric object types:
using GeoObj = std::variant<Line, Circle, Rectangle>;
// create and initialize a collection of geometric objects:
std::vector<GeoObj> createFigure(){
	std::vector<GeoObj> f;
	f.push_back(Line{Coord{1,2},Coord{3,4}});
	f.push_back(Circle{Coord{5,5},2});
	f.push_back(Rectangle{Coord{3,3},Coord{6,4}});
	return f;
}
int main() {
	std::vector<GeoObj> figure = createFigure();
	for (const GeoObj& geoobj : figure) {
		std::visit([] (const auto& obj) {
			obj.draw(); // polymorphic call of draw()
			}, geoobj);
	}
}
```
* In general, I would recommend now to program polymorphism with `std::variant<>` by default, because it is usually faster (no new and delete, no virtual functions for non-polymorphic use), a lot safer (no pointers), and usually all types are known at compile-time of all code.  

#### 3. `std::any`
* Using `std::any`
```
std::any a; // a is empty
std::any b = 4.3; // b has value 4.3 of type double
a = 42; // a has value 42 of type int
b = std::string{"hi"}; // b has value "hi" of type std::string
```
#### 4. `std::byte`
* Using std::byte
```
std::byte b1{0x3F};
std::byte b2{0b1111’0000};
std::byte b3[4] {b1, b2, std::byte{1}}; // 4 bytes (last is 0)
```
#### 5. `String Views`
`The class template basic_string_view describes an object that can refer to a constant contiguous sequence of char-like objects`  
A string_view doesn't manage the storage that it refer to. Lifetime management is up to the user.  
* When would you use a `string_view` instead of `string`  
Pass as a parameter to a `pure` function(parameters `const string&`)  
Returning from a function  
A reference to part of long-lived data structure.
* Drawbacks of string_view  
Lifetime management  
not `null-terminated`

**Don’t use `std::string_view` at all unless you know what you do.**
* Don't use `std::string_view` to initialize a `std::string` member
* Don't use initialize a `string_view` as below  
```
std::string getStr() {
	return std::string("long_string_help_to_detect_issues");
}
std::string_view sv1 = getStr(); // RISK
string_view sv = "abc"s; // RISK
```
#### 6. The Filesystem Library
...  
### Part III: Expert Utilities
* `std::from_chars`: converts a given character sequence to a numeric value  
* `std::to_chars`: converts numeric values to a given character sequence
## VIII. C++11 Features
#### 1. Smart Pointers
- `std::unique_ptr`, `std::make_unique`
- `std::shared_ptr`, `std::make_shared`
- `std::weak_ptr`
#### 2. Rvalue references, Move Semantics, and Perfect Forwarding
- `lvalue`: correspond to objects you can refer to, either by name or by following a pointer or lvalue reference.
- `rvalue`: correspond to temporary objects returned from functions.
- `std::move`: performs an unconditionally casts its input into an rvalue reference. It doesn't move anything.
- `std::forward`: casts its argument to an rvalue only if that argument is bound to an rvalue. It doesn't forward anything.  
[The Nightmare of Move Semantics for Trivial Classes](https://www.youtube.com/watch?v=PNRju6_yn3o&list=PLKtBMOPB5ra9DeN_N6jEDg0eY07_sgTtk&index=7&t=10s)  
[Code example](https://github.com/pvthuyet/Concurrency_With_Modern_Cpp/blob/master/universalreference/Customer.h)
#### 3. Lambda Expressions
* Caputure local variable only, no member variables, no static variables.
* Avoid default capture modes  
There are two default capture modes in C++11: by-reference `[&]`, and by-value `[=]`  
Default by-reference capture can lead to dangling reference  
Default by-value capture is susceptible to dangling pointers(especially this), and it misleadingly suggests that lambdas are self-contained.  
* C++14 supported caputure by moving the object (C++11 can use std::bind but a litle complicated)
```
	std::vector<double> data;
	auto func = [data = std::move(data)]{
		// do something
	};
```
#### 4. Concurrency API
- `std::thread`
- `std::async`
- `std::future`
- `std::promise`
- `std::atomic`
- `std::mutex`
- `std::condition_variable`
#### 5. Variadic templates
#### 6. New containers
- [`std::tuple`](https://en.cppreference.com/w/cpp/utility/tuple)
- [`std::array`](https://en.cppreference.com/w/cpp/container/array)
- [`std::forward_list`](https://en.cppreference.com/w/cpp/container/forward_list)
- [`std::unordered_set`](https://en.cppreference.com/w/cpp/container/unordered_set)
- [`std::unordered_map`](https://en.cppreference.com/w/cpp/container/unordered_map)
- [`std::unordered_multiset`](https://en.cppreference.com/w/cpp/container/unordered_multiset)
- [`std::unordered_multimap`](https://en.cppreference.com/w/cpp/container/unordered_multimap)
#### 7. [Utilities library](https://en.cppreference.com/w/cpp/utility)
- [`bitset`](https://en.cppreference.com/w/cpp/utility/bitset)
- [`std::move_if_noexcept`](https://en.cppreference.com/w/cpp/utility/move_if_noexcept)
- [`std:: initializer_list`](https://en.cppreference.com/w/cpp/utility/initializer_list)
- [`std::hash`](https://en.cppreference.com/w/cpp/utility/hash)
- [`std::regex`](https://en.cppreference.com/w/cpp/regex)
- [`std::declval`](https://en.cppreference.com/w/cpp/utility/declval)
  
#### 8. New Keywords
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
## VII. Moving to Modern C++ [by Scott Meyers](http://shop.oreilly.com/product/0636920033707.do?cmp=af-code-books-video-product_cj_0636920033707_7708709)
#### 1. Distinguish between `()` and `{}` when creating objects
* Braced `{}` initialization is the most widely usable initialization syntax, it prevents narrowing conversions, and it’s immune to C++’s most vexing parse.
* During constructor overload resolution, braced initializers are matched to `std::initializer_list` parameters if at all possible, even if other constructors offer seemingly better matches.
* An example of where the choice between parentheses and braces can make a significant difference is creating a `std::vector<numeric type>` with two arguments.
* Choosing between parentheses and braces for object creation inside templates can be challenging.
#### 2. Prefer `nullptr` to `0` and `NULL`.
#### 3. Prefer `alias declarations` to `typedefs`.
#### 4. Prefer `scoped enums` to `unscoped enums`.
#### 5. Prefer `deleted` functions to `private` undefined ones
#### 6. Declare `overriding` functions `override`.
#### 7. Prefer `const_iterators` to `iterators`.
#### 8. Declare functions `noexcept` if they won’t emit exceptions
#### 9. Use `constexpr` whenever possible.
#### 10. Make `const` member functions thread safe.
#### 11. Understand special member function generation
* Default constructor
* Destructor
* Copy constructor
* Copy assignment operator
* Move constructor and move assignment operator
#### 12. Prefer `task-based` programming to `thread-based`
#### 13. Specify `std::launch::async` if asynchronicity is essential
#### 14. Make `std::threads` unjoinable on all paths
#### 15. Be aware of varying thread handle destructor behavior
#### 16. Consider void `futures` for one-shot event communication
#### 17. Use `std::atomic` for concurrency, `volatile` for special memory.
#### 18. Consider pass by value for copyable parameters that are cheap to move and always copied
#### 19. Consider `emplacement` instead of `insertion`

## VI. Smart Pointer
Refer to [C++ Smart Pointers - Usage and Secrets - Nicolai Josuttis](https://www.youtube.com/watch?v=XH4xIyS9B2I&t=1336s)
### 1. std::unique_ptr
* Use std::unique_ptr for exclusive-ownership resource management
* Downcasts do not work for unique pointers
```
	class GeoObj {};
	class Circle : public GeoObj {};
	std::vector< std::unique_ptr<GeoObj> > geoObjs;
	geoObjs.emplace_back( std::make_unique<Circle>(...) ); // Ok, insert circle into collection
	const auto& p = geoObjs[0]; // p is unique_ptr<GeoObj>;
	std::unique_ptr<Circle> cp{p}; // Compile-time Error
	auto cp(dynamic_cast<std::unique_ptr<Circle>>(p); // Compile-time Error
	if (auto cp = dynamic_cast<Circle*>(p.get())) // Ok, use in `if` because of restrict life time
	{
		// use cp as Circle*
	}
```
* Pass std::unique_ptr to function
```
void sink(std::unique_ptr<GeoObj> const& up){} // Ok as normal, reference to unique pointer
void sink(std::unique_ptr<GeoObj> up){} // Ok, pass value only accept move `Herb Sutter` style
void sink(std::unique_ptr<GeoObj>&& up){} // Ok, pass rvalue only accept move `Scott Meyers` style
...  
auto up(std::make_unique<GeoObj>());
sink(std::move(up));
up.release(); // Remember destroy up after using std::move
```
* Custome Deleter
```
	auto deleter = [](std::FILE* fp) {
		std::fclose(fp);
		fp = nullptr;
	};
	std::FILE* fp = nullptr;
	fopen_s(&fp, "test.txt", "w+");
	std::unique_ptr<std::FILE, decltype(deleter)> up(fp, deleter); // deleter as type and argument
	//std::unique_ptr<std::FILE, decltype(deleter)> up(fp); // OK with c++20
	const char str[] = "hello world!\n";
	fwrite(str, sizeof(char), sizeof(str), up.get());
```
### 2. std::shared_ptr
![](https://github.com/pvthuyet/Modern-Cplusplus/blob/master/resources/sharedptr.png)
#### a. Use std::shared_ptr for shared-owenership resource management
* std::shared_ptrs are twice the size of a raw pointer
* Memory for the reference count must be dynamically allocated
* Increments and decrements of the reference count must be atomic
#### b. Custom deleter
```
	auto deleter = [](Widget* pw) {
		delete pw;
		pw = nullptr;
	};
	std::unique_ptr<Widget, decltype(deleter)> upw(new Widget, deleter); // deleter as type and argument
	std::shared_ptr<Widget> spw(new Widget, deleter); // deleter is only at arg
```
#### c. [Cast APIs for std::shared_ptr](https://en.cppreference.com/w/cpp/memory/shared_ptr/pointer_cast)
* std::static_pointer_cast
* std::dynamic_pointer_cast
* std::const_pointer_cast
* std::reinterpret_pointer_cast
#### d. Duplicate `Control block` Issue
![](https://github.com/pvthuyet/Modern-Cplusplus/blob/master/resources/sharedptrerr1.png)  
* First exmaple:  
```
	int* p = new int;
	std::shared_ptr<int> sp1(p);
	std::shared_ptr<int> sp2(p);
```
**(*)Node** : *create a std::shared_ptr from raw pointer is bad idea*  
* Second exmaple:  
Suppose our program uses `std::shared_ptrs` to manage Widget objects, and we have a data structure that keeps track of Widgets that have been processed.
```
	class Widget {
	public:
		void process(std::vector< std::shared_ptr<Widget> > &widgets) {
			widgets.emplace_back(this); // Create new control-block and point to the same Widget object.
		}
	}
	int main() {
		std::vector< std::shared_ptr<Widget> > processedWidgets;
		auto spw = std::make_shared<Widget>(); // Create a control-block and point to new Widget object
		spw->process(processedWidgets);
	}
```
The output of progam  
![](https://github.com/pvthuyet/Modern-Cplusplus/blob/master/resources/sharedptrdup.png)
* The problem is there is 2 control-blocks point to the same Widget object. Hence, the Widget object was destroyed 2 times-> Crash application.
##### **Solution** by [Scott Meyers](https://www.oreilly.com/library/view/effective-modern-c/9781491908419/)
* Use `std::enable_shared_from_this`
* This is [The Curiously Recurring Template Pattern(CRTP)](https://en.wikipedia.org/wiki/Curiously_recurring_template_pattern)
```
	class Widget : public std::enable_shared_from_this<Widget> {
	public:
		void process(std::vector< std::shared_ptr<Widget> > &widgets) {
			widgets.emplace_back(shared_from_this()); // Look-up control-block
		}
	}
	int main() {
		std::vector< std::shared_ptr<Widget> > processedWidgets;
		auto spw = std::make_shared<Widget>();
		spw->process(processedWidgets);
	}
```
Ok, so far so good, `shared_from_this` looks up the control block for the current object, and it creates a new std::shared_ptr tha refers to that control block.  
But in case the control block has not existed, `shared_from_this` will throw exception. For example if change `auto spw = std::make_shared<Widget>();` -> `Widget* spw = new Widget();`  
Therefore, we must make sure the `std::shared_ptr` already existed before call `process()`  
Apply the `factory function` template  
```
	class Widget : public std::enable_shared_from_this<Widget> {
	private:
		Widget() = default; // Invisiable constructor
	public:
		template<typename... Args>
		static std::shared_ptr<Widget> create(Args&&... params) {
			return std::shared_ptr<Widget>(new Widget(std::forward<Args>(params)...));
		}
		void process(std::vector< std::shared_ptr<Widget> > &widgets) {
			widgets.emplace_back(shared_from_this()); // Look-up control-block
		}
	}
	int main() {
		std::vector< std::shared_ptr<Widget> > processedWidgets;
		auto spw = Widget::create();
		spw->process(processedWidgets);
	}
```
**OK so far so good, the issue was sorted**  
#### e. std::shared_ptr overhead
There is overhead if sending std::shared_ptr value to much (pass by argument value of function)  
*Should pass by reference of std::shared_ptr*
#### f. Are std::shared_ptr, std::weak_ptr thread-safe??
[Refer to Rainer Grimm](https://www.modernescpp.com/index.php/atomic-smart-pointers)  
A `std::shared_ptr` consists of a control block and its resource:
* The control block is thread-safe: That means, modifying the reference counter is an atomic operation and you have the guarantee that the resource will be deleted exactly once.
* The access to the resource is not thread-safe.  
* [Atomic smart pointers support by C++20](https://en.cppreference.com/w/cpp/experimental/atomic_shared_ptr)
#### g. Leak memory
* This is circlic references issue of std::shared_ptr  
![](https://github.com/pvthuyet/Modern-Cplusplus/blob/master/resources/sharedptrleak_.png)  
In this case, we cannot use raw pointer because if A is destroyed, B will contain a pointer to A that will dangle.  
B won't be able to detect that, so B may in advertently dereference the dangling pointers.  
Thank to `std::weak_ptr`
### 3. std::weak_ptr
* Allow to share but not own an object or resource
* `Pointer that knows when it dangles`(Scott Meyers)
* Resolve the circlic reference issue of `std::shared_ptr` and `raw pointer`  
![](https://github.com/pvthuyet/Modern-Cplusplus/blob/master/resources/sharedptrleakfix.png)  
* Potential use cases for std::weak_ptr include caching, observer lists.
* Using a std::weak_ptr
```
	1. No pointer interface
	2. Access to the resource through a temporary shared pointer
		std::weak_ptr<R> wp;
		if (auto p = wp.lock())
			p->callMember()
```
* But be aware that `std::weak_ptr` might hold memory if use with `std::make_shared<R>` to create `td::shared_ptr`
## V. Atomic
Refer [Fedor Pikus talked](https://www.youtube.com/watch?v=ZQFzMfHIxng&list=PLKtBMOPB5ra9DeN_N6jEDg0eY07_sgTtk&index=6&t=3143s)
* std::atomic is neither copyable nor movable.
* The primary std::atomic template may be instantiated with any `TriviallyCopyable` type T satisfying both `CopyConstructible` and `CopyAssignable`.  
What is trivially copyable?  
```
	1. Continuous chunk of memory
	2. Copying the object means copying all bits (memcpy)
	3. No virtual function, noexcept constructor
```
* On MSVC: If not define `_ENABLE_ATOMIC_ALIGNMENT_FIX`, the compiler will complain: `std::atomic<T>` with sizeof(T) equal to 2/4/8 and `alignof(T)` < `sizeof(T)`
```
struct Data { // user-defined trivially-copyable type
	int x; // 4 byte
	void* ptr; // 4 byte
	Data() noexcept : x(0), ptr(nullptr)
	{}
};
std::atomic<Data> atm;
...  
struct A { int a[100]; };
std::atomic<A> a;
assert(std::atomic_is_lock_free(&a)); // false: a is not lock-free
```
![](https://github.com/pvthuyet/Modern-Cplusplus/blob/master/resources/atomic.png)  
(*) **Note that doesn't full support for `std::atomic<float>`, `std::atomic<double>` until C++20**
## IV. Memory Model
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
* **Mutexes**  
`std::mutex`  
`std::recursive_mutex`: allows the same thread to lock the mutex many times.  
`std::timed_mutex`  
`std::recursive_timed_mutex`  
![](https://github.com/pvthuyet/Modern-Cplusplus/blob/master/resources/mutexes.png) 
`shared_mutex`: Shared mutexes are usually used in situations when multiple readers can access the same resource at the same time without causing data races, but only one writer can do so.  
`shared_timed_mutex`  
* **Locks**  
`std::lock_guard`  
`std::unique_lock`  
`std::scoped_lock` : lock many mutexes  
`std::shared_lock` : many threads can read but only one thread can write  

#### 3. Thread-Local Data
Thread-local data, also known as thread-local storage, is created for each thread separately. 
#### 4. Condition Variables
`std::condition_variable`: only wait on object of type `std::unique_lock<std::mutex>`  
`std::condition_variable_any`: can wait on an user-supplied lock type that meet the concept of `BasicLockable`.  
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
If the `promise` sets the value or the exception `more than once`, a `std::future_error` exception is thrown.  
If you destroy the `std::promise` without calling the set-method or a `std::packaged_task` before invoking it, a `std::future_error` exception with an error code `std::future_errc::broken_promise` would be stored in the shared state.  
If a future fut asks for the result `more than once`, a `std::future_error` exception is thrown.  
There is a  `One-to-one` relationship between the promise and the future.
* **std::shared_future**  
`One-to-many` relationship between a promise and many futures.
* **Exceptions**  
If the callable used by `std::async` or by `std::packaged_task` throws an error, the exception is store in the shared state.  
When the future fut then calls `fut.get()`, the exception is rethrown, and the future has to handle it.  
* **Notifications**  
`Condition variables` to synchronise threads multiple times.  
A `promise` can send its notification only once.  
`promise` and `future` is the first choice  
![](https://github.com/pvthuyet/Modern-Cplusplus/blob/master/resources/condvstask.png)  
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
  
## II. Challenges
### 1. ABA Problem
ABA means you read a value twice and each time it returns the same value A.  
Therefore you conclude that nothing changed in between.  
However, you missed the fact that the value was updated to B somewhere in between.
### 2. Blocking Issues
It is a victim of a spurious wakeup or lost wakeup.
### 3. Breaking of Program Invariants
...
### 4. Data Race
...
### 5. Livelock
The thread is wating a notification never fire or fired.
### 6. Deadlocks
There are two main reasons for deadlocks:  
* **A mutex has not been unlocked.**  
* **You lock your mutexes in a different order.**
#### 1. Locking a non-recursive mutex more than once
#### 2. Lock Mutexes in Different Order
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
#### 3. Solution
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
### 7. False Sharing
`False sharing` occurs if two threads read at the same time different variables a and b that are located on the same `cache line`.  
* `std::hardware_destructive_interference_size`:  returns the minimum offset between two objects to avoid false sharing.  
* `std::hardware_constructive_interference_size`: returns the maximum size of contiguous memory to promote true sharing.  
```
struct Sum{
alignas(std::hardware_destructive_interference_size) long long a{0};
alignas(std::hardware_destructive_interference_size) long long b{0};
};
```
### 8. Lifetime Issues of Variables
...
### 9. Moving Threads
```
std::thread t([]{std::cout << std::this_thread::get_id();});  
std::thread t2([]{std::cout << std::this_thread::get_id();});  
t = std::move(t2);// Issues: t must be call join() before move  
t.join();  
t2.join();  
```
### 10. Race Conditions
....
  
  
## I. C++11 Books:
[ModernesCpp by Rainer Grimm](http://www.modernescpp.com/index.php)  
![1](https://github.com/pvthuyet/Modern-Cplusplus/blob/master/resources/cpp17.png)  
![1](https://github.com/pvthuyet/Modern-Cplusplus/blob/master/resources/ConcurrencyCoverFrame.png)  
  
![2](https://github.com/pvthuyet/Modern-Cplusplus/blob/master/resources/effectivec%2B%2B.jpg)  
  
![3](https://github.com/pvthuyet/Modern-Cplusplus/blob/master/resources/C-Concurrency-in-Action-2nd-Edition.jpg)    
  

