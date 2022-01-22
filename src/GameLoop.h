#pragma once
#include <atomic>
#include <memory>

#include "Threading/ThreadQueueMeshes.h"


class GameLoop
{
public:
	void runLoop(std::shared_ptr<std::atomic<bool>> gameShouldClose, std::shared_ptr<ThreadQueueMeshes> threadQueueMeshes);
};