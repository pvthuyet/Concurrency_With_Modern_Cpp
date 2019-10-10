## Pattern
"Each pattern is a three-part rule, which express a relation between a certain context, a problem, and a solution.” Christopher Alexander  
#### Creational
* Abstract Factory
* Builder
* Factory Method
* Object Pool
* Prototype
* Singleton
#### Structural
* Adapter
* Bridge
* Composite
* Decorator
* Facade
* Flyweight
* Private Class Data
* Proxy
#### Behavioural
* Chain of responsibility
* Command
* Interpreter
* Iterator
* Mediator
* Memento
* Null Object
* Observer
* State
* Strategy
* Template method
* Visitor
### 1. Singleton
The performace some ways to implement singleton pattern:  
* Visual studio 2017
* Run 12 threads
* 10M times per thread.
* With single thread run 120M times  
  
| Single thread | Scott Meyers | Acquire-release with Spinlock | Acquire-release with mutex | Sequential consistency | Once flag | Spinlock  | Mutex      |
|---------------|--------------|-------------------------------|----------------------------|------------------------|-----------|-----------|------------|
| 0.060396      | 0.118227     | 0.120404                      | 0.121473                   | 0.122035               | 2.997471  | 10.172581 | 242.204793 |

![1](https://github.com/pvthuyet/Concurrency_With_Modern_Cpp/blob/master/resources/singleton_.png)
