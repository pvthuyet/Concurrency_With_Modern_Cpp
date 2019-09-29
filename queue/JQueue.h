#pragma once

#include <memory>
#include <condition_variable>
#include <mutex>
#include <atomic>
#include <chrono>
#include <limits>
#include "../utils/JExeption.h"

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

		std::mutex mTailMux;
		node* mTailNode;

		// Number of elements
		std::atomic_uint mSize;
		// shutdown flag
		std::atomic_bool mShutdown;
		const std::size_t mLimit;

		// APIs
		JQueue<T>::node* getTail() 
		{
			std::lock_guard<std::mutex> lock(mTailMux);
			return mTailNode;
		}

		std::unique_ptr<node> popHead() 
		{
			std::unique_ptr<node> oldHead = std::move(mHeadNode);
			mHeadNode = std::move(oldHead->next);
			--mSize;
			return oldHead;
		}

		std::unique_lock<std::mutex> waitForData() 
		{
			std::unique_lock<std::mutex> lock(mHeadMux);
			while (true)
			{
				bool res = mCV.wait_for(lock, std::chrono::milliseconds(10), [&] { return size() > 0; });
				if (isShutdown())
				{
					throw JException(tvp::ExceptionCode::QUEUE_SHUTDOWN,
						tvp::Utils::getThreadId() + " ThreadPool was shutdown!\n");
				}

				if (res || (size() > 0))
				{
					return std::move(lock);
				}
			}
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
		explicit JQueue(std::size_t limit = 100U) :
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
			mShutdown.store(true);			
		}

		bool isShutdown() const noexcept
		{
			return mShutdown.load();
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
				std::lock_guard<std::mutex> lock(mTailMux);
				mTailNode->data = newData;
				mTailNode->next = std::move(p);
				mTailNode = newTail;
				++mSize;
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
			return mSize.load();			
		}
	};
}