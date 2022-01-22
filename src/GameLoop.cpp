#include "GameLoop.h"

#include <chrono>
#include <iostream>
#include <thread>

#include "World/World.h"


constexpr int TICK_TIME = 100;



void GameLoop::runLoop(std::shared_ptr<std::atomic<bool>> gameShouldClose, std::shared_ptr<ThreadQueueMeshes> threadQueueMeshes)
{
	World zaWarudo;

	while (!gameShouldClose->load())
	{
		const auto tickEnd = std::chrono::steady_clock::now() + std::chrono::milliseconds(TICK_TIME);

		zaWarudo.tick();

		std::this_thread::sleep_until(tickEnd);
	}
	return;
}