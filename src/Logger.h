#pragma once
#include <fstream>
#include <mutex>
#include <string>



class Logger
{
public:
	Logger(const char* filePath) : file(filePath) {};

	void Write(const char* message);
	void Write(const std::string& message);

private:
	std::ofstream file;
	std::mutex fileMutex;
};
