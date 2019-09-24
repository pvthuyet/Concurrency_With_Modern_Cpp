#pragma once
#include <string>
#include <mutex>
#include <fstream>
#include "../utils/utils.h"

namespace tvp
{
	static const std::string DEBUG_LEVEL	= "debug";
	static const std::string INFO_LEVEL		= "info";
	static const std::string ERROR_LEVEL	= "error";

	class Logger
	{
	private:
		std::mutex mMut;
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

		void debug(std::string const& msg)
		{
			std::string str;
			str = tvp::Utils::getDateTime() + "\t" 
				+ DEBUG_LEVEL + "\t" 
				+ msg;
			mFile.write(str.c_str(), str.size());
			mFile.flush();
		}
	};
}