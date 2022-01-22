#include <atomic>
#include <exception>
#include <functional>
#include <iostream>
#include <thread>

#include "GameLoop.h"
#include "RenderingLoop.h"
#include "Threading/ThreadQueueMeshes.h"


void runGameLoop(std::shared_ptr<std::atomic<bool>> gameShouldClose, std::shared_ptr<ThreadQueueMeshes> threadQueueMeshes) try
{
	GameLoop mainLoop;
	mainLoop.runLoop(std::move(gameShouldClose), std::move(threadQueueMeshes));
}
catch (const std::exception& error)
{
	std::cout << "Rendering exception: " << error.what() << std::endl;
	gameShouldClose->store(true);
}
catch (...)
{
	std::cout << "What the fuck." << std::endl;
	gameShouldClose->store(true);
}



void runRenderingLoop(std::shared_ptr<std::atomic<bool>> gameShouldClose, std::shared_ptr<ThreadQueueMeshes> threadQueueMeshes) try
{
	RenderingLoop mainLoop;
	mainLoop.runLoop(std::move(gameShouldClose), std::move(threadQueueMeshes));
}
catch (const std::exception& error)
{
	std::cout << "Game exception: " << error.what() << std::endl;
	gameShouldClose->store(true);
}
catch (...)
{
	std::cout << "What the fuck." << std::endl;
	gameShouldClose->store(true);
}



int main()
{
	std::shared_ptr<std::atomic<bool>> gameShouldClose = std::make_shared<std::atomic<bool>>(false);
	std::shared_ptr<ThreadQueueMeshes> threadQueueMeshes = std::make_shared<ThreadQueueMeshes>();
	
	std::jthread gameThread(runGameLoop, gameShouldClose, threadQueueMeshes);
	std::jthread renderingThread(runRenderingLoop, gameShouldClose, threadQueueMeshes);

	gameThread.join();
	renderingThread.join();

	std::cout << "Application termination" << std::endl;

	return 0;
}