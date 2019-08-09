#pragma once
#include <memory>
#include <mutex>
#include <condition_variable>
#include <vld.h>
namespace tet
{
	namespace concurrency {
		template<typename T>
		class queue
		{
		private:
			// Data structure of queue
			struct node
			{
				std::shared_ptr<T> m_data;
				std::unique_ptr<node> m_next;
			};

			// data queue
			std::unique_ptr<node> m_head;
			node* m_tail;

			// Thread safe data
			std::condition_variable m_cv;
			std::mutex m_headMutex;
			std::mutex m_tailMutex;

			// Private methods
			node* getTail()
			{
				std::lock_guard<std::mutex> lock(m_tailMutex);
				return m_tail;
			}
			std::unique_ptr<node> popHead() {
				std::unique_ptr<node> oldHead = std::move(m_head);
				m_head = std::move(oldHead->m_next);
				return oldHead;
			}
			std::unique_lock<std::mutex> waitForData() {
				std::unique_lock<std::mutex> lock(m_headMutex);
				m_cv.wait(lock, [&] { return m_head.get() != getTail(); });
				return std::move(lock);
			}
			std::unique_ptr<node> tryPopHead() {
				std::lock_guard<std::mutex> lock(m_headMutex);
				if (m_head.get() == getTail()) {
					return nullptr;
				}
				return popHead();
			}
			std::unique_ptr<node> tryPopHead(T& val) {
				std::lock_guard<std::mutex> lock(m_headMutex);
				if (m_head.get() == getTail()) {
					return nullptr;
				}
				val = std::move(*m_head->m_data);
				return popHead();
			}
			std::unique_ptr<node> waitPopHead() {
				std::unique_lock<std::mutex> lock(waitForData());
				return popHead();
			}
			std::unique_ptr<node> waitPopHead(T& val) {
				std::unique_lock<std::mutex> lock(waitForData());
				val = std::move(*m_head->m_data);
				return popHead();
			}

		public:
			queue();
			queue(const queue&) = delete;
			queue& operator=(const queue&) = delete;
			virtual ~queue() noexcept {}

			bool empty();
			void push(T Val);
			std::shared_ptr<T> tryPop();
			bool tryPop(T& val);
			std::shared_ptr<T> waitAndPop();
			void waitAndPop(T& val);
		};

		// Implementing for public methods
		template<typename T>
		queue<T>::queue() :
			m_head(std::make_unique<node>()),
			m_tail(m_head.get())
		{}

		template<typename T>
		bool queue<T>::empty()
		{
			std::lock_guard<std::mutex> lock(m_headMutex);
			return (m_head.get() == getTail());
		}

		template<typename T>
		void queue<T>::push(T val) {
			std::shared_ptr<T> newData(std::make_shared<T>(std::move(val)));
			std::unique_ptr<node> newNode(std::make_unique<node>());
			node* const newTail = newNode.get();
			{
				std::lock_guard<std::mutex> lock(m_tailMutex);
				m_tail->m_data = newData;
				m_tail->m_next = std::move(newNode);
				m_tail = newTail;
			}
			m_cv.notify_one();
		}
		
		template<typename T>
		std::shared_ptr<T> queue<T>::tryPop() {
			auto oldHead = tryPopHead();
			return oldHead ? oldHead->m_data : nullptr;
		}

		template<typename T>
		bool queue<T>::tryPop(T& val) {
			auto oldHead = tryPopHead(val);
			return oldHead ? true : false;
		}

		template<typename T>
		std::shared_ptr<T> queue<T>::waitAndPop() {
			std::unique_ptr<node> const oldHead = waitPopHead();
			return oldHead->m_data;
		}

		template<typename T>
		void queue<T>::waitAndPop(T& val) {
			waitPopHead(val);
		}
	}
}
