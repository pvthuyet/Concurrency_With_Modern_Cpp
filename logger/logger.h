#pragma once
#include <string>
#include <mutex>
#include <fstream>
#include <ctime>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <time.h>
#include "../utils/utils.h"

namespace tvp
{
	static const std::string DEBUG_LEVEL = "DEBUG";
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
			std::ostringstream os;
			os << tvp::Utils::getDateTime() << "\t" << DEBUG_LEVEL << "\t" << msg;

			mFile.write(os.str().c_str(), os.str().size());
			mFile.flush();
		}
	};
}