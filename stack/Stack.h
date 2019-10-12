#pragma once
#include <atomic>
#include <thread>
#include <functional>

namespace tvp
{
	namespace lockfree
	{
		namespace counting
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
					//clean();
				}

				void clean()
				{
					while (pop())
					{
					}
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
				}

				void clean()
				{
					while (pop())
					{}
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
	}
}