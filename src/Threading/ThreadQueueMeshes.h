#pragma once
#include <memory>
#include <mutex>
#include <queue>
#include "../Rendering/Mesh/MeshChunk.h"



class ThreadQueueMeshes
{
public:
	void getQueue(std::queue<std::unique_ptr<MeshChunk>>& swapQueue);
	void mergeQueue(std::queue<std::unique_ptr<MeshChunk>>& mergeQueue);

private:
	std::queue<std::unique_ptr<MeshChunk>> meshQueue;
	std::mutex queueMutex;
};
