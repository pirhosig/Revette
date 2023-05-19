#include "Logger.h"



void Logger::Write(const char* message)
{
	std::lock_guard<std::mutex> lock(fileMutex);
	file << message << "\n";
	file.flush();
}



void Logger::Write(const std::string& message)
{
	std::lock_guard<std::mutex> lock(fileMutex);
	file << message << "\n";
	file.flush();
}