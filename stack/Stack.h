#pragma once
#include <atomic>
#include <thread>
#include <functional>

namespace tvp
{
	namespace lockfree
	{
		namespace pending
		{
			template<typename T>
			class Stack
			{
			private:
				struct Node
				{
					std::shared_ptr<T> mData;
					Node* mNext;
					Node(T const& data) : mData(std::make_shared<T>(data)), mNext(nullptr)
					{}
				};

				std::atomic< Node* > mHead;
				std::atomic<unsigned> mThreadInPop;
				std::atomic< Node* > mTobeDeleted;

			private:
				static void deleteNodes(Node* nodes)
				{
					while (nodes)
					{
						Node* next = nodes->mNext;
						delete nodes;
						nodes = next;
					}
				}

				void chainPendingNodes(Node* nodes)
				{
					Node* last = nodes;
					while (Node* const next = last->mNext)
					{
						last = next;
					}
					chainPendingNodes(nodes, last);
				}
				void chainPendingNodes(Node* first, Node* last)
				{
					last->mNext = mTobeDeleted;
					while (!mTobeDeleted.compare_exchange_weak(last->mNext, first));
				}
				void chainPendingNode(Node* n)
				{
					chainPendingNodes(n, n);
				}

				void tryReclaim(Node* oldHead)
				{
					if (mThreadInPop.load() == 1)
					{
						Node* nodesToDelete = mTobeDeleted.exchange(nullptr);
						if (!(--mThreadInPop))
						{
							deleteNodes(nodesToDelete);
						}
						else if (nodesToDelete)
						{
							chainPendingNodes(nodesToDelete);
						}
						delete oldHead;
					}
					else
					{
						chainPendingNode(oldHead);
						--mThreadInPop;
					}
				}

			public:
				Stack() : mThreadInPop(0)
				{
					mHead.store(nullptr);
					mTobeDeleted.store(nullptr);
				}

				~Stack()
				{
					while (pop());
					deleteNodes(mTobeDeleted.load());
				}

				void push(T const& data)
				{
					try
					{
						Node* newNode = new Node(data);
						newNode->mNext = mHead.load();
						while (!mHead.compare_exchange_weak(newNode->mNext, newNode));
					}
					catch (...)
					{
						std::cout << "Unknow exception\n";
					}
				}

				std::shared_ptr<T> pop()
				{
					++mThreadInPop;
					Node* oldHead = mHead.load();
					while (oldHead && !mHead.compare_exchange_weak(oldHead, oldHead->mNext));

					std::shared_ptr<T> res(nullptr);
					if (oldHead)
					{
						res.swap(oldHead->mData);
						tryReclaim(oldHead);
					}

					return res;
				}
			};
		}

		namespace hazard
		{
			unsigned constexpr MAX_HAZARD_POINTERS = 100;
			struct HazardPointer
			{
				std::atomic<std::thread::id> mId;
				std::atomic<void*> mPointer;
			};
			HazardPointer gHazardPointers[MAX_HAZARD_POINTERS];

			class HpOwner
			{
			private:
				HazardPointer* mHp;

			public:
				HpOwner(HpOwner const&) = delete;
				HpOwner& operator=(HpOwner const&) = delete;
				HpOwner() : mHp(nullptr)
				{
					for (unsigned i = 0; i < MAX_HAZARD_POINTERS; ++i)
					{
						std::thread::id oldId;
						if (gHazardPointers[i].mId.compare_exchange_strong(oldId, std::this_thread::get_id()))
						{
							mHp = &gHazardPointers[i];
							break;
						}
					}
					if (!mHp)
					{
						throw std::runtime_error("No hazard pointer available");
					}
				}
				~HpOwner()
				{
					mHp->mPointer.store(nullptr);
					mHp->mId.store(std::this_thread::get_id());
				}

				std::atomic<void*>& getPoiter()
				{
					return mHp->mPointer;
				}
			};

			std::atomic<void*>& getHazardPointerForCurrentThread()
			{
				thread_local static HpOwner hazard;
				return hazard.getPoiter();
			}

			bool outstandingHazardPointersFor(void* p)
			{
				for (unsigned i = 0; i < MAX_HAZARD_POINTERS; ++i)
				{
					if (gHazardPointers[i].mPointer.load() == p)
					{
						return true;
					}
				}
				return false;
			}

			template<typename T>
			void doDelete(void *p)
			{
				delete static_cast<T*>(p);
			}
			struct DataToReclaim
			{
				void* mData;
				std::function<void(void*)> mDeleter;
				DataToReclaim* mNext;

				template<typename T>
				DataToReclaim(T* p) :
					mData(p),
					mDeleter(&doDelete<T>),
					mNext(nullptr)
				{}
				~DataToReclaim()
				{
					mDeleter(mData);
				}
			};

			std::atomic<DataToReclaim*> gNodesToReclaim(nullptr);

			void addToReclaimList(DataToReclaim* node)
			{
				node->mNext = gNodesToReclaim.load();
				while (!gNodesToReclaim.compare_exchange_weak(node->mNext, node));
			}

			template<typename T>
			void reclaimLater(T* data)
			{
				addToReclaimList(new DataToReclaim(data));
			}

			void deleteNodesWithNoHazards()
			{
				DataToReclaim* current = gNodesToReclaim.exchange(nullptr);
				while (current)
				{
					DataToReclaim* const next = current->mNext;
					if (!outstandingHazardPointersFor(current->mData))
					{
						delete current;
					}
					else
					{
						addToReclaimList(current);
					}
					current = next;
				}
			}

			template<typename T>
			class Stack
			{
			private:
				struct Node
				{
					std::shared_ptr<T> mData;
					Node* mNext;
					Node(T const& data) : mData(std::make_shared<T>(data)), mNext(nullptr)
					{}
				};
				std::atomic< Node* > mHead;

			public:
				Stack()
				{
					mHead.store(nullptr);
				}

				~Stack()
				{
					while (pop());
				}

				void push(T const& data)
				{
					try
					{
						Node* newNode = new Node(data);
						newNode->mNext = mHead.load();
						while (!mHead.compare_exchange_weak(newNode->mNext, newNode));
					}
					catch (...)
					{
						std::cout << "Unknow exception\n";
					}
				}

				std::shared_ptr<T> pop()
				{
					std::atomic<void*> &hp = getHazardPointerForCurrentThread();
					Node* oldHead = mHead.load();
					do
					{
						Node* tmp;
						do
						{
							tmp = oldHead;
							hp.store(oldHead);
							oldHead = mHead.load();
						} while (oldHead != tmp);
					} while (oldHead && !mHead.compare_exchange_strong(oldHead, oldHead->mNext));

					hp.store(nullptr);
					std::shared_ptr<T> res = nullptr;
					if (oldHead)
					{
						res.swap(oldHead->mData);
						if (outstandingHazardPointersFor(oldHead))
						{
							reclaimLater(oldHead);
						}
						else
						{
							delete oldHead;
						}
						deleteNodesWithNoHazards();
					}

					return res;
				}
			};
		}

		namespace sharedptr
		{
			template<typename T>
			class Stack
			{
			private:
				struct Node
				{
					std::shared_ptr<T> mData;
					std::shared_ptr<Node> mNext;
					Node(T const& data) : 
						mData(std::make_shared<T>(data)),
						mNext(nullptr)
					{}
				};
				std::shared_ptr<Node> mHead;

			public:
				~Stack()
				{
					while (pop());
				}

				void push(T const& data)
				{
					std::shared_ptr<Node> const newNode = std::make_shared<Node>(data);
					newNode->mNext = std::atomic_load(&mHead);
					while (!std::atomic_compare_exchange_weak(&mHead, &newNode->mNext, newNode));
				}

				std::shared_ptr<T> pop()
				{
					std::shared_ptr<Node> oldHead = std::atomic_load(&mHead);
					while (oldHead && !std::atomic_compare_exchange_weak(&mHead, &oldHead, std::atomic_load(&oldHead->mNext)));
					if (oldHead)
					{
						std::atomic_store(&oldHead->mNext, std::shared_ptr<Node>());
						return oldHead->mData;
					}

					return std::shared_ptr<T>();
				}
			};
		}

		namespace referencecount
		{
			template<typename T>
			class Stack
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

				struct Node
				{
					std::shared_ptr<T> mData;
					std::atomic_int mInternalCount;
					CountedNodePtr mNext;
					Node(T const& data) :
						mData(std::make_shared<T>(data)),
						mInternalCount(0)
					{}
				};
				std::atomic<CountedNodePtr> mHead;

			private:
				void increaseHeadCount(CountedNodePtr& oldCounter)
				{
					CountedNodePtr newCounter;
					do
					{
						newCounter = oldCounter;
						++(newCounter.mExternalCount);
					} while (!mHead.compare_exchange_strong(oldCounter, newCounter, std::memory_order_acquire, std::memory_order_relaxed));
					oldCounter.mExternalCount = newCounter.mExternalCount;
				}

			public:
				~Stack()
				{
					while (pop());
				}

				void push(T const& data)
				{
					CountedNodePtr newNode;
					newNode.mPtr = new Node(data);
					newNode.mExternalCount = 1;
					newNode.mPtr->mNext = mHead.load(std::memory_order_relaxed);
					while (!mHead.compare_exchange_weak(newNode.mPtr->mNext, newNode, std::memory_order_release, std::memory_order_relaxed));
				}

				std::shared_ptr<T> pop()
				{
					CountedNodePtr oldHead = mHead.load(std::memory_order_relaxed);
					for (;;)
					{
						increaseHeadCount(oldHead);
						Node* const ptr = oldHead.mPtr;
						if (!ptr)
							return std::shared_ptr<T>();

						if (mHead.compare_exchange_strong(oldHead, ptr->mNext, std::memory_order_relaxed))
						{
							std::shared_ptr<T> res;
							res.swap(ptr->mData);
							int const countIncrease = oldHead.mExternalCount - 2;
							if (ptr->mInternalCount.fetch_add(countIncrease, std::memory_order_release) == -countIncrease)
							{
								delete ptr;
							}
							return res;
						}
						else if(ptr->mInternalCount.fetch_add(-1, std::memory_order_relaxed) == 1)
						{
							ptr->mInternalCount.load(std::memory_order_acquire);
							delete ptr;
						}
					}
					
					return nullptr;
				}
			};
		}
	}
}