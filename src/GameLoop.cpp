#include "GameLoop.h"

#include <chrono>
#include <iostream>
#include <thread>

#include "World/World.h"


constexpr int TICK_TIME = 100;



void GameLoop::runLoop(std::atomic<bool>& gameShouldClose)
{
	World zaWarudo;

	while (!gameShouldClose)
	{
		const auto tickEnd = std::chrono::steady_clock::now() + std::chrono::milliseconds(TICK_TIME);

		zaWarudo.tick();

		std::this_thread::sleep_until(tickEnd);
	}
	return;
}