#pragma once
#include <string>
#include <mutex>
#include <fstream>
#include <iostream>
#include "../utils/utils.h"

namespace tvp
{
	static const std::string DEBUG_LEVEL	= "debug";
	static const std::string INFO_LEVEL		= "info";
	static const std::string ERROR_LEVEL	= "error";

	class Logger
	{
	private:
		std::mutex mMux;
		std::ofstream mFile;

	public:
		Logger()
		{
			mFile.open("log.log", std::ofstream::out | std::ofstream::trunc);
		}
		
		~Logger()
		{
			mFile.close();
		}

		void debug(std::string const& msg, bool msgOnly = false)
		{
			std::string str;
			str = msgOnly ? msg : (tvp::Utils::getDateTime() + "\t" + DEBUG_LEVEL + "\t" + msg);
			{
				std::lock_guard<std::mutex> lk(mMux);
				mFile.write(str.c_str(), str.size());
				mFile.flush();
				std::cout << str << std::flush;
			}
		}
	};
}