#include <atomic>
#include <exception>
#include <iostream>
#include <mutex>
#include <thread>

#include "GameLoop.h"
#include "RenderingLoop.h"
#include "Threading/PlayerState.h"
#include "Threading/ThreadPointerQueue.h"



void runGameLoop(
	std::atomic<bool>& gameShouldClose,
	std::shared_ptr<ThreadPointerQueue<MeshDataChunk>> threadQueueMeshes,
	std::shared_ptr<ThreadQueue<ChunkPos>> threadQueueMeshDeletion,
	std::atomic<PlayerState>& playerState
) try
{
	GameLoop mainLoop;
	mainLoop.runLoop(
		gameShouldClose,
		std::move(threadQueueMeshes),
		std::move(threadQueueMeshDeletion),
		playerState
	);
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



void runRenderingLoop(
	std::atomic<bool>& gameShouldClose,
	std::shared_ptr<ThreadPointerQueue<MeshDataChunk>> threadQueueMeshes,
	std::shared_ptr<ThreadQueue<ChunkPos>> threadQueueMeshDeletion,
	std::atomic<PlayerState>& playerState
) try
{
	RenderingLoop mainLoop;
	mainLoop.runLoop(
		gameShouldClose,
		std::move(threadQueueMeshes),
		std::move(threadQueueMeshDeletion),
		playerState
	);
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



int main()
{
	std::atomic<bool> gameShouldClose{};
	std::shared_ptr<ThreadPointerQueue<MeshDataChunk>> threadQueueMeshes = std::make_shared<ThreadPointerQueue<MeshDataChunk>>();
	std::shared_ptr<ThreadQueue<ChunkPos>> threadQueueMeshDeletion = std::make_shared<ThreadQueue<ChunkPos>>();
	std::atomic<PlayerState> playerState{PlayerState(EntityPosition())};

	std::jthread gameThread(
		runGameLoop,
		std::ref(gameShouldClose),
		threadQueueMeshes,
		threadQueueMeshDeletion,
		std::ref(playerState)
	);
	std::jthread renderingThread(
		runRenderingLoop,
		std::ref(gameShouldClose),
		threadQueueMeshes,
		threadQueueMeshDeletion,
		std::ref(playerState)
	);

	gameThread.join();
	renderingThread.join();

	std::cout << "Application termination" << std::endl;

	return 0;
}