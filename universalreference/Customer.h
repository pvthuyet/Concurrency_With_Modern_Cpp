#pragma once
#include <string>
#include <utility>

namespace tvp
{
	class Customer
	{
	private:
		std::string mFirst;
		std::string mLast;
		int mId;

	public:
		// C++11: good but not perfect
		//explicit Customer(std::string f, std::string l = "", int id = 0) :
		//	mFirst(std::move(f)),
		//	mLast(std::move(l)),
		//	mId(id)
		//{}

		// C++17 implementing
		template<typename T1, 
			typename T2 = std::string,
			typename = std::enable_if_t<std::is_convertible_v<T1, std::string> >
		>
		explicit Customer(T1&& f, T2&& l = "", int id = 0) :
			mFirst(std::forward<T1>(f)),
			mLast(std::forward<T2>(l)),
			mId(id)
		{}

		// C++20 implementing
		/*
		template<typename T1, typename T2 = std::string>
		requires std::is_convertible_v<T1, std::string>

			explicit Customer(T1&& f, T2&& l = "", int id = 0) :
			mFirst(std::forward<T1>(f)),
			mLast(std::forward<T2>(l)),
			mId(id)
		{}
		*/

		void print()
		{
			std::cout << "First name: " << mFirst << ",\t" << "Last name: " << mLast << ",\t" << "id = " << mId << std::endl;
		}
	};

	class VIP : public Customer
	{
		using Customer::Customer; // Inheritant all constructors from superclass
	};
}