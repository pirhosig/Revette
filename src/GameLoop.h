#pragma once
#include <atomic>
#include <memory>

#include "LoopTimer.h"
#include "Rendering/Mesh/MeshDataChunk.h"
#include "Threading/ThreadPointerQueue.h"



class GameLoop
{
public:
	GameLoop() : tickTime(75) {}
	void runLoop(std::atomic<bool>& gameShouldClose, std::shared_ptr<ThreadPointerQueue<MeshDataChunk>> threadQueueMeshes);

private:
	LoopTimer tickTime;
};