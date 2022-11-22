#include <atomic>
#include <exception>
#include <mutex>
#include <thread>

#include "GlobalLog.h"
#include "MainLoop.h"



void runGame()
try
{
	MainLoop mainLoop;
	mainLoop.run();
}
catch (const std::exception& error)
{
	GlobalLog.Write(std::string("Exception occurred: ") + error.what());
}
catch (...)
{
	GlobalLog.Write("What the fuck. Something has gone horribly wrong.");
}



int main()
{
	runGame();

	GlobalLog.Write("Application termination");

	return 0;
}