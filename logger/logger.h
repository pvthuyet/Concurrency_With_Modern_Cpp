#pragma once
#include <string>
#include <fstream>
#include <iostream>
#include <atomic>
#include "../utils/utils.h"
#include "../SpinMutex/SpinMutex.h"

namespace tvp
{
	static const std::string DEBUG_LEVEL	= "debug";
	static const std::string INFO_LEVEL		= "info";
	static const std::string ERROR_LEVEL	= "error";

	// Logger with singleton pattern
	class Logger
	{
	private:
		// Sync for writing
		tvp::SpinMutex mFileSpin;
		std::ofstream mFile;

		// Singleton class
		static tvp::SpinMutex mInitSpin;
		static std::atomic< tvp::Logger* > mInst;

		explicit Logger(std::string const& fileName)
		{
			mFile.open(fileName, std::ofstream::out | std::ofstream::trunc);
		}
		
	public:
		~Logger()
		{
			mFile.close();
		}

		Logger(const Logger&) = delete;
		Logger& operator=(const Logger&) = delete;

		static tvp::Logger* getInstance()
		{
			tvp::Logger* log = mInst.load(std::memory_order_acquire);
			if (!log)
			{
				std::lock_guard<tvp::SpinMutex> lock(mInitSpin);
				log = mInst.load(std::memory_order_relaxed);
				if (!log)
				{
					log = new tvp::Logger("log.log");
					mInst.store(log, std::memory_order_release);
				}
			}
			return log;
		}


		void debug(std::string const& msg, bool msgOnly = false, bool console = true)
		{
			std::string str;
			str = msgOnly ? msg : (tvp::Utils::getDateTime() + "\t" + DEBUG_LEVEL + "\t" + msg);
			{
				std::lock_guard<tvp::SpinMutex> lock(mFileSpin);
				mFile.write(str.c_str(), str.size());
				mFile.flush();
				if (console)
				{
					std::cout << str << std::flush;
				}
			}
		}
	};

	tvp::SpinMutex tvp::Logger::mInitSpin;
	std::atomic<tvp::Logger*> tvp::Logger::mInst(nullptr);
}