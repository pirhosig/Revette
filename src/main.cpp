#include <atomic>
#include <exception>
#include <functional>
#include <iostream>
#include <thread>

#include "GameLoop.h"
#include "RenderingLoop.h"



void runGameLoop(std::atomic<bool>& gameShouldClose) try
{
	GameLoop mainLoop;
	mainLoop.runLoop(gameShouldClose);
}
catch (const std::exception& error)
{
	std::cout << "Rendering exception: " << error.what() << std::endl;
	gameShouldClose.store(true);
}
catch (...)
{
	std::cout << "What the fuck." << std::endl;
	gameShouldClose.store(true);
}



void runRenderingLoop(std::atomic<bool>& gameShouldClose) try
{
	RenderingLoop mainLoop;
	mainLoop.runLoop(gameShouldClose);
}
catch (const std::exception& error)
{
	std::cout << "Game exception: " << error.what() << std::endl;
	gameShouldClose.store(true);
}
catch (...)
{
	std::cout << "What the fuck." << std::endl;
	gameShouldClose.store(true);
}



int main()
{
	std::atomic<bool> gameShouldClose;
	gameShouldClose.store(false);

	std::jthread gameThread(runGameLoop, std::ref(gameShouldClose));
	std::jthread renderingThread(runRenderingLoop, std::ref(gameShouldClose));

	gameThread.join();
	renderingThread.join();

	std::cout << "Application termination" << std::endl;

	return 0;
}