#include "GameLoop.h"

#include <chrono>
#include <iostream>
#include <thread>

#include "World/World.h"


constexpr int TICK_TIME = 40;
const char* NOISE_HEIGHTMAP = "GQAbAA0AAwAAAM3MDEATAAAAAD4JAACamRk/AAAAAAAAAACAPgEkAAMAAAATAM3MzD0WAAEAAAD//wEA";
const char* NOISE_FOLIAGE = "AQA=";



void GameLoop::runLoop(
	std::atomic<bool>& gameShouldClose,
	std::shared_ptr<ThreadPointerQueue<MeshDataChunk>> threadQueueMeshes,
	std::shared_ptr<ThreadQueue<ChunkPos>> threadQueueMeshDeletion,
	std::atomic<PlayerState>& playerState
)
{
	World zaWarudo(std::move(threadQueueMeshes), std::move(threadQueueMeshDeletion), NOISE_HEIGHTMAP, NOISE_FOLIAGE);

	while (!gameShouldClose.load())
	{
		auto tickTimeBegin = std::chrono::steady_clock::now();
		const auto tickTimeEnd = tickTimeBegin + std::chrono::milliseconds(TICK_TIME);

		zaWarudo.tick(playerState);

		std::this_thread::sleep_until(tickTimeEnd);
		const auto tickDuration = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - tickTimeBegin);
		tickTime.addTime(tickDuration.count());
	}
}