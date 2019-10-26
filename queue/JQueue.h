#pragma once

#include <memory>
#include <condition_variable>
#include <mutex>
#include <atomic>
#include <chrono>
#include <limits>
#include "../utils/JExeption.h"
#include "../SpinMutex/SpinMutex.h"

namespace tvp 
{
	template<typename T>
	class JQueue 
	{
	private:
		struct node 
		{
			std::shared_ptr<T> data;
			std::unique_ptr<node> next;
		};

		std::condition_variable mCV;
		std::mutex mHeadMux;
		std::unique_ptr<node> mHeadNode;

		//std::mutex mTailMux;
		tvp::SpinMutex mTailMux; // Using lock-free
		node* mTailNode;

		// Number of elements
		std::atomic_uint mSize;
		// shutdown flag
		std::atomic_bool mShutdown;
		const std::size_t mLimit;

		// APIs
		JQueue<T>::node* getTail() 
		{
			//std::lock_guard<std::mutex> lock(mTailMux);
			std::lock_guard<tvp::SpinMutex> lock(mTailMux); // Using lock-free
			return mTailNode;
		}

		std::unique_ptr<node> popHead() 
		{
			std::unique_ptr<node> oldHead = std::move(mHeadNode);
			mHeadNode = std::move(oldHead->next);
			mSize.fetch_sub(1, std::memory_order_relaxed);
			return oldHead;
		}

		std::unique_lock<std::mutex> waitForData() 
		{
			std::unique_lock<std::mutex> lock(mHeadMux);
			tvp::interruptibleWait(mCV, lock, [&]() { return (size() > 0 || isShutdown()); });
			if (isShutdown())
			{
				throw JException(tvp::ExceptionCode::QUEUE_SHUTDOWN,
					tvp::Utils::getThreadId() + " Queue was shutdown!\n");
			}
			return std::move(lock);
		}

		std::unique_ptr<node> tryPopHead() 
		{
			std::lock_guard<std::mutex> lock(mHeadMux);
			if (mHeadNode.get() == getTail()) 
				return nullptr;

			return popHead();
		}

		std::unique_ptr<node> tryPopHead(T& value) 
		{
			std::lock_guard<std::mutex> lock(mHeadMux);
			if (mHeadNode.get() == getTail()) 
				return nullptr;

			value = std::move(*mHeadNode->data);
			return popHead();
		}

		std::unique_ptr<node> waitPopHead() 
		{
			std::unique_lock<std::mutex> lock(waitForData());
			return popHead();
		}

		std::unique_ptr<node> waitPopHead(T& value) 
		{
			std::unique_lock<std::mutex> lock(waitForData());
			value = std::move(*mHeadNode->data);
			return popHead();
		}

	public:
		explicit JQueue(std::size_t limit = 1000000U) :
			mLimit(limit),
			mHeadNode(std::make_unique<node>()), 
			mTailNode(mHeadNode.get()), 
			mSize(0),
			mShutdown(false)
		{}

		~JQueue()
		{
			shutdown();
			clear();
		}

		void shutdown() noexcept
		{
			mShutdown.store(true, std::memory_order_relaxed);			
		}

		bool isShutdown() const noexcept
		{
			return mShutdown.load(std::memory_order_relaxed);
		}

		void clear()
		{
			// Clean all data in queue
			while (tryPop())
			{}
		}

		// No copiable
		JQueue(const JQueue&) = delete;
		JQueue& operator=(const JQueue&) = delete;

		bool empty()
		{
			std::lock_guard<std::mutex> lock(mHeadMux);
			return (mHeadNode.get() == getTail());
		}

		void push(T newVal)
		{
			if (isShutdown())
			{
				throw JException(tvp::ExceptionCode::QUEUE_SHUTDOWN,
					tvp::Utils::getThreadId() + " ThreadPool was shutdown!\n");
			}
			if (size() >= mLimit)
			{
				throw JException(tvp::ExceptionCode::QUEUE_LIMIT,
					tvp::Utils::getThreadId() + " Queue is fulfill!\n");
			}

			std::shared_ptr<T> newData(std::make_shared<T>(std::move(newVal)));
			std::unique_ptr<node> p(std::make_unique<node>());
			node* const newTail = p.get();
			{
				//std::lock_guard<std::mutex> lock(mTailMux);
				std::lock_guard<tvp::SpinMutex> lock(mTailMux); // Using lock-free
				mTailNode->data = newData;
				mTailNode->next = std::move(p);
				mTailNode = newTail;
				mSize.fetch_add(1, std::memory_order_relaxed);
			}
			mCV.notify_one();
		}

		std::shared_ptr<T> tryPop()
		{
			auto oldHead = tryPopHead();
			return oldHead ? oldHead->data : nullptr;
		}

		bool tryPop(T& value)
		{
			auto oldHead = tryPopHead(value);
			return oldHead ? true : false;
		}

		// Wait until pop success or queue was shutdown
		// throw ShutdownQueueException: must handle in code
		std::shared_ptr<T> waitAndPop()
		{
			std::unique_ptr<node> const oldHead = waitPopHead();
			return oldHead->data;
		}

		// Wait until pop success or queue was shutdown
		// throw ShutdownQueueException: must handle in code
		void waitAndPop(T& value)
		{
			std::unique_ptr<node> const oldHead = waitPopHead(value);
		}

		std::size_t size()
		{
			return mSize.load(std::memory_order_relaxed);
		}
	};

	namespace lookfree
	{
		namespace referencecount
		{
			template<typename T>
			class JQueue
			{
			private:
				struct Node;
				struct CountedNodePtr
				{
					int mExternalCount;
					Node* mPtr;
					CountedNodePtr() noexcept :
						mExternalCount(0),
						mPtr(nullptr)
					{}
				};

				std::atomic<CountedNodePtr> mHead;
				std::atomic<CountedNodePtr> mTail;

				struct NodeCounter
				{
					unsigned mInternalCount : 30;
					unsigned mExternalCounters : 2;
				};

				struct Node
				{
					std::atomic<T*> mData;
					std::atomic<NodeCounter> mCount;
					std::atomic<CountedNodePtr> mNext;
					Node()
					{
						NodeCounter newCounter;
						newCounter.mInternalCount = 0;
						newCounter.mExternalCounters = 2;
						mCount.store(newCounter);
					}

					void releaseRef()
					{
						NodeCounter oldCounter = mCount.load(std::memory_order_relaxed);
						NodeCounter newCounter;
						do
						{
							newCounter = oldCounter;
							--(newCounter.mInternalCount);
						} while (!mCount.compare_exchange_strong(oldCounter, newCounter, std::memory_order_acquire, std::memory_order_relaxed));
						
						if (!newCounter.mInternalCount && !newCounter.mExternalCounters)
						{
							delete this;
						}
					}
				};

				static void increaseExternalCount(std::atomic<CountedNodePtr>& counter, CountedNodePtr& oldCounter)
				{
					CountedNodePtr newCounter;
					do
					{
						newCounter = oldCounter;
						++(newCounter.mExternalCount);
					} while (!counter.compare_exchange_strong(oldCounter, newCounter, std::memory_order_acquire, std::memory_order_relaxed));

					oldCounter.mExternalCount = newCounter.mExternalCount;
				}

				static void freeExternalCounter(CountedNodePtr& oldNodePtr)
				{
					Node* const ptr = oldNodePtr.mPtr;
					int const countIncrease = oldNodePtr.mExternalCount - 2;
					NodeCounter oldCounter = ptr->mCount.load(std::memory_order_relaxed);
					NodeCounter newCounter;
					do
					{
						newCounter = oldCounter;
						--(newCounter.mExternalCounters);
						newCounter.mInternalCount += countIncrease;
					} while (!ptr->mCount.compare_exchange_strong(oldCounter, newCounter, std::memory_order_acquire, std::memory_order_relaxed));
					
					if (!newCounter.mInternalCount && !newCounter.mExternalCounters)
					{
						delete ptr;
					}
				}

				void setNewTail(CountedNodePtr &oldTail, CountedNodePtr const &newTail)
				{
					Node* const currentTailPtr = oldTail.mPtr;
					while (!mTail.compare_exchange_weak(oldTail, newTail) && oldTail.mPtr == currentTailPtr);

					if (oldTail.mPtr == currentTailPtr)
					{
						freeExternalCounter(oldTail);
					}
					else
					{
						currentTailPtr->releaseRef();
					}
				}

			public:
				JQueue()
				{
					CountedNodePtr draf;
					draf.mPtr = new Node;
					draf.mExternalCount = 1;
					mHead.store(draf);
					mTail.store(draf);
				}

				void push(T newVal)
				{
					auto newData = std::make_unique<T>(newVal);
					CountedNodePtr newNext;
					newNext.mPtr = new Node;
					newNext.mExternalCount = 1;
					CountedNodePtr oldTail = mTail.load();
					for (;;)
					{
						increaseExternalCount(mTail, oldTail);
						T* oldData = nullptr;
						if (oldTail.mPtr->mData.compare_exchange_strong(oldData, newData.get()))
						{
							CountedNodePtr oldNext;
							if (!oldTail.mPtr->mNext.compare_exchange_strong(oldNext, newNext))
							{
								delete newNext.mPtr;
								newNext = oldNext;
							}
							setNewTail(oldTail, newNext);
							newData.release();
							break;
						}
						else
						{
							CountedNodePtr oldNext;
							if (oldTail.mPtr->mNext.compare_exchange_strong(oldNext, newNext))
							{
								oldNext = newNext;
								newNext.mPtr = new Node;
							}
							setNewTail(oldTail, oldNext);
						}	
					}
				}

				std::unique_ptr<T> pop()
				{
					CountedNodePtr oldHead = mHead.load(std::memory_order_relaxed);
					for (;;)
					{
						increaseExternalCount(mHead, oldHead);
						Node* const ptr = oldHead.mPtr;
						if (ptr == mTail.load().mPtr)
						{
							return std::unique_ptr<T>();
						}
						CountedNodePtr next = ptr->mNext.load();
						if (mHead.compare_exchange_strong(oldHead, next))
						{
							T* const res = ptr->mData.exchange(nullptr);
							freeExternalCounter(oldHead);
							return std::unique_ptr<T>(res);
						}
						ptr->releaseRef();
					}
				}
			};
		}
	}
}