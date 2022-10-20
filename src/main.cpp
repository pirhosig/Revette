#include <atomic>
#include <exception>
#include <mutex>
#include <thread>

#include "GameLoop.h"
#include "GlobalLog.h"
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
	GlobalLog.Write(std::string("Game exception: ") + error.what());
	gameShouldClose.store(true);
}
catch (...)
{
	GlobalLog.Write("What the fuck. Something has gone horribly wrong on the game loop thread.");
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
	GlobalLog.Write(std::string("Rendering exception: ") + error.what());
	gameShouldClose.store(true);
}
catch (...)
{
	GlobalLog.Write("What the fuck. The rendering thread has fucked up horribly.");
	gameShouldClose.store(true);
}



int main()
{
	std::atomic<bool> gameShouldClose{};
	std::shared_ptr<ThreadPointerQueue<MeshDataChunk>> threadQueueMeshes = std::make_shared<ThreadPointerQueue<MeshDataChunk>>();
	std::shared_ptr<ThreadQueue<ChunkPos>> threadQueueMeshDeletion = std::make_shared<ThreadQueue<ChunkPos>>();
	std::atomic<PlayerState> playerState{PlayerState(EntityPosition(0.0, 50.0, 0.0))};

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

	GlobalLog.Write("Application termination");

	return 0;
}