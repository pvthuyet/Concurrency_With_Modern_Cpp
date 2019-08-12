#pragma once
#include <memory>
#include <mutex>
#include <condition_variable>
#include <vld.h>
namespace tet
{
	namespace lockbase {
		template<typename T>
		class Queue
		{
		private:
			// Data structure of Queue
			struct node
			{
				std::shared_ptr<T> mData;
				std::unique_ptr<node> mNext;
			};

			// data Queue
			std::unique_ptr<node> mHead;
			node* mTail;

			// Thread safe data
			std::condition_variable mCV;
			std::mutex mHeadMutex;
			std::mutex mTailMutex;

			// Private methods
			node* getTail()
			{
				std::lock_guard<std::mutex> lock(mTailMutex);
				return mTail;
			}
			std::unique_ptr<node> popHead() {
				std::unique_ptr<node> oldHead = std::move(mHead);
				mHead = std::move(oldHead->mNext);
				return oldHead;
			}
			std::unique_lock<std::mutex> waitForData() {
				std::unique_lock<std::mutex> lock(mHeadMutex);
				mCV.wait(lock, [&] { return mHead.get() != getTail(); });
				return std::move(lock);
			}
			std::unique_ptr<node> tryPopHead() {
				std::lock_guard<std::mutex> lock(mHeadMutex);
				if (mHead.get() == getTail()) {
					return nullptr;
				}
				return popHead();
			}
			std::unique_ptr<node> tryPopHead(T& val) {
				std::lock_guard<std::mutex> lock(mHeadMutex);
				if (mHead.get() == getTail()) {
					return nullptr;
				}
				val = std::move(*mHead->mData);
				return popHead();
			}
			std::unique_ptr<node> waitPopHead() {
				std::unique_lock<std::mutex> lock(waitForData());
				return popHead();
			}
			std::unique_ptr<node> waitPopHead(T& val) {
				std::unique_lock<std::mutex> lock(waitForData());
				val = std::move(*mHead->mData);
				return popHead();
			}

		public:
			Queue();
			Queue(const Queue&) = delete;
			Queue& operator=(const Queue&) = delete;
			virtual ~Queue() noexcept {}

			bool empty();
			void push(T Val);
			std::shared_ptr<T> tryPop();
			bool tryPop(T& val);
			std::shared_ptr<T> waitAndPop();
			void waitAndPop(T& val);
		};

		// Implementing for public methods
		template<typename T>
		Queue<T>::Queue() :
			mHead(std::make_unique<node>()),
			mTail(mHead.get())
		{}

		template<typename T>
		bool Queue<T>::empty()
		{
			std::lock_guard<std::mutex> lock(mHeadMutex);
			return (mHead.get() == getTail());
		}

		template<typename T>
		void Queue<T>::push(T val) {
			std::shared_ptr<T> newData(std::make_shared<T>(std::move(val)));
			std::unique_ptr<node> newNode(std::make_unique<node>());
			node* const newTail = newNode.get();
			{
				std::lock_guard<std::mutex> lock(mTailMutex);
				mTail->mData = newData;
				mTail->mNext = std::move(newNode);
				mTail = newTail;
			}
			mCV.notify_one();
		}
		
		template<typename T>
		std::shared_ptr<T> Queue<T>::tryPop() {
			auto oldHead = tryPopHead();
			return oldHead ? oldHead->mData : nullptr;
		}

		template<typename T>
		bool Queue<T>::tryPop(T& val) {
			auto oldHead = tryPopHead(val);
			return oldHead ? true : false;
		}

		template<typename T>
		std::shared_ptr<T> Queue<T>::waitAndPop() {
			std::unique_ptr<node> const oldHead = waitPopHead();
			return oldHead->mData;
		}

		template<typename T>
		void Queue<T>::waitAndPop(T& val) {
			waitPopHead(val);
		}
	}
}
