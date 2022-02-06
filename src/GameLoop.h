#pragma once
#include <atomic>
#include <memory>

#include "LoopTimer.h"
#include "Threading/ThreadQueueMeshes.h"



class GameLoop
{
public:
	GameLoop() : tickTime(75) {}
	void runLoop(std::shared_ptr<std::atomic<bool>> gameShouldClose, std::shared_ptr<ThreadQueueMeshes> threadQueueMeshes);

private:
	LoopTimer tickTime;
};