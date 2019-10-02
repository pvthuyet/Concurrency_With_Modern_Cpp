#pragma once
#include <mutex>
#include <vld.h>
#include "../utils/Utils.h"
#include "../logger/Logger.h"

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
				static Singleton instance(11);
				//volatile int dummy{}; // for without optimization
				return instance;
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
				static Singleton instance(13);
				return instance;
			}
		};
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
				mInstance = new Singleton(15);
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
		};

		// Initialize for static data members
		Singleton* Singleton::mInstance = nullptr;
		std::once_flag Singleton::mOnceFlag;
	}
}