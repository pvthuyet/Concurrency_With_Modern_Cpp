#pragma once
#include <mutex>
#include <vld.h>
#include <atomic>
#include "../utils/Utils.h"
#include "../logger/Logger.h"
#include "../spinlock/Spinlock.h"

namespace tvp
{
	namespace singlethread
	{
		class Singleton
		{
		private:
			unsigned int mId;
			Singleton() = default;
			explicit Singleton(unsigned int id) :
				mId(id)
			{
				std::cout << "Single thread Singleton(id = " << id << ")\t";
			}

		public:
			~Singleton() noexcept
			{
				std::cout << "Single thread ~Singleton\n";
			}

			Singleton(const Singleton&) = delete;
			Singleton& operator=(const Singleton&) = delete;
			static Singleton& getInstance()
			{
				static Singleton instance(1);
				//volatile int dummy{}; // for without optimization
				return instance;
			}

			bool test() const
			{
				return true;
			}
		};
	}

	namespace scottmayer
	{
		class Singleton
		{
		private:
			unsigned int mId;

		private:
			explicit Singleton(unsigned int id) noexcept
				: mId(id)
			{
				std::cout << "Scott Mayer Singleton(id = " << id << ")\t";
			}

		public:
			~Singleton() noexcept
			{
				std::cout << "Scott Mayer ~Singleton\n";
			}

			Singleton(const Singleton&) = delete;
			Singleton& operator=(const Singleton&) = delete;
			static Singleton& getInstance()
			{
				static Singleton instance(2);
				return instance;
			}

			bool test() const
			{
				return true;
			}
		};
	}

	namespace acqrel
	{
		class Singleton
		{
		private:
			static std::atomic<Singleton*> mInstance;
			static std::mutex mMutex;

			unsigned int mId;

			explicit Singleton(unsigned int id) noexcept :
				mId(id)
			{
				std::cout << "Acquire release Singleton(id = " << id << ")\t";
			}

		public:
			~Singleton() noexcept
			{
				std::cout << "Acquire release ~Singleton\n";
			}

			Singleton(const Singleton&) = delete;
			Singleton& operator=(const Singleton&) = delete;

			static Singleton* getInstance()
			{
				Singleton* sin = mInstance.load(std::memory_order_acquire);
				if (!sin)
				{
					std::lock_guard<std::mutex> lk(mMutex);
					sin = mInstance.load(std::memory_order_relaxed); //double - checked locking pattern.
					if (!sin)
					{
						sin = new Singleton(4);
						mInstance.store(sin, std::memory_order_release);
					}

				}
				return sin;
			}

			bool test() const
			{
				return true;
			}
		};
		std::atomic<Singleton*> Singleton::mInstance(nullptr);
		std::mutex Singleton::mMutex;
	}

	namespace acqrelspin
	{
		class Singleton
		{
		private:
			static std::atomic<Singleton*> mInstance;
			static tvp::lockfree::Spinlock mSpin;

			unsigned int mId;

			explicit Singleton(unsigned int id) noexcept :
				mId(id)
			{
				std::cout << "Acquire release Spin Singleton(id = " << id << ")\t";
			}

		public:
			~Singleton() noexcept
			{
				std::cout << "Acquire release ~Singleton\n";
			}

			Singleton(const Singleton&) = delete;
			Singleton& operator=(const Singleton&) = delete;

			static Singleton* getInstance()
			{
				Singleton* sin = mInstance.load(std::memory_order_acquire);
				if (!sin)
				{
					tvp::lockfree::LockGuard lk(mSpin);
					sin = mInstance.load(std::memory_order_relaxed); //double - checked locking pattern.
					if (!sin)
					{
						sin = new Singleton(4);
						mInstance.store(sin, std::memory_order_release);
					}
				}
				return sin;
			}

			bool test() const
			{
				return true;
			}
		};
		std::atomic<Singleton*> Singleton::mInstance(nullptr);
		tvp::lockfree::Spinlock Singleton::mSpin;
	}

	namespace seqcst
	{
		class Singleton
		{
		private:
			static std::atomic<Singleton*> mInstance;
			static std::mutex mMutex;

			unsigned int mId;

			explicit Singleton(unsigned int id) noexcept :
				mId(id)
			{
				std::cout << "Sequential consistency Singleton(id = " << id << ")\t";
			}

		public:
			~Singleton() noexcept
			{
				std::cout << "Sequential consistency ~Singleton\n";
			}

			Singleton(const Singleton&) = delete;
			Singleton& operator=(const Singleton&) = delete;

			static Singleton* getInstance()
			{
				Singleton* sin = mInstance.load();
				if (!sin)
				{
					std::lock_guard<std::mutex> lk(mMutex);
					sin = mInstance.load(); //double - checked locking pattern.
					if (!sin)
					{
						sin = new Singleton(5);
						mInstance.store(sin);
					}

				}
				return sin;
			}

			bool test() const
			{
				return true;
			}
		};
		std::atomic<Singleton*> Singleton::mInstance(nullptr);
		std::mutex Singleton::mMutex;
	}

	namespace onceflag
	{
		class Singleton
		{
		private:
			static std::once_flag mOnceFlag;
			static Singleton* mInstance;
			unsigned int mId;
			Singleton() noexcept :
				mId(0)
			{
			}
			explicit Singleton(unsigned int id) noexcept :
				mId(id)
			{
				std::cout << "Once Flag Singleton(id = " << id << ")\t";
			}
			static void initSingleton() noexcept
			{
				mInstance = new Singleton(6);
			}

		public:
			~Singleton() noexcept
			{
				std::cout << "Once Flag ~Singleton\n";
			}
			Singleton(const Singleton&) = delete;
			Singleton& operator=(const Singleton&) = delete;
			static Singleton& getInstance()
			{
				std::call_once(mOnceFlag, Singleton::initSingleton);
				//volatile int dummy{};
				return *mInstance;
			}
			unsigned int getId() const noexcept
			{
				return mId;
			}

			bool test() const
			{
				return true;
			}
		};

		// Initialize for static data members
		Singleton* Singleton::mInstance = nullptr;
		std::once_flag Singleton::mOnceFlag;
	}

	namespace spinlock
	{
		class Singleton
		{
		private:
			static Singleton* mInstance;
			unsigned int mId;
			static tvp::lockfree::Spinlock mSpin;

		private:
			explicit Singleton(unsigned int id) noexcept :
				mId(id)
			{
				std::cout << "Spin lock Singleton(id = " << id << ")\t";
			}

		public:
			~Singleton() noexcept
			{
				std::cout << "Spin lock ~Singleton\n";
			}

			Singleton(const Singleton&) = delete;
			Singleton& operator=(const Singleton&) = delete;

			static Singleton& getInstance()
			{
				tvp::lockfree::LockGuard lk(mSpin);
				if (!mInstance)
				{
					mInstance = new Singleton(8);
				}
				return *mInstance;
			}

			bool test() const
			{
				return true;
			}
		};
		Singleton* Singleton::mInstance = nullptr;
		tvp::lockfree::Spinlock Singleton::mSpin;
	}

	namespace lock
	{
		class Singleton
		{
		private:
			static Singleton* mInstance;
			unsigned int mId;
			static std::mutex mMutex;

		private:
			explicit Singleton(unsigned int id) noexcept :
				mId(id)
			{
				std::cout << "Lock mutex Singleton(id = " << id << ")\t";
			}

		public:
			~Singleton() noexcept
			{
				std::cout << "Lock mutex ~Singleton\n";
			}

			Singleton(const Singleton&) = delete;
			Singleton& operator=(const Singleton&) = delete;

			static Singleton& getInstance()
			{
				std::lock_guard<std::mutex> lk(mMutex);
				if (!mInstance)
				{
					mInstance = new Singleton(7);
				}
				return *mInstance;
			}

			bool test() const
			{
				return true;
			}
		};
		Singleton* Singleton::mInstance = nullptr;
		std::mutex Singleton::mMutex;
	}
}