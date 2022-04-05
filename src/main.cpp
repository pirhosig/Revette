#include <atomic>
#include <exception>
#include <functional>
#include <iostream>
#include <thread>

#include "GameLoop.h"
#include "RenderingLoop.h"
#include "Threading/ThreadPointerQueue.h"


void runGameLoop(std::shared_ptr<std::atomic<bool>> gameShouldClose, std::shared_ptr<ThreadPointerQueue<MeshDataChunk>> threadQueueMeshes) try
{
	GameLoop mainLoop;
	mainLoop.runLoop(gameShouldClose, std::move(threadQueueMeshes));
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



void runRenderingLoop(std::shared_ptr<std::atomic<bool>> gameShouldClose, std::shared_ptr<ThreadPointerQueue<MeshDataChunk>> threadQueueMeshes) try
{
	RenderingLoop mainLoop;
	mainLoop.runLoop(gameShouldClose, std::move(threadQueueMeshes));
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



int main()
{
	std::shared_ptr<std::atomic<bool>> gameShouldClose = std::make_shared<std::atomic<bool>>(false);
	std::shared_ptr<ThreadPointerQueue<MeshDataChunk>> threadQueueMeshes = std::make_shared<ThreadPointerQueue<MeshDataChunk>>();
	
	std::jthread gameThread(runGameLoop, gameShouldClose, threadQueueMeshes);
	std::jthread renderingThread(runRenderingLoop, gameShouldClose, threadQueueMeshes);

	gameThread.join();
	renderingThread.join();

	std::cout << "Application termination" << std::endl;

	return 0;
}