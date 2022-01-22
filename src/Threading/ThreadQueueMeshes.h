#pragma once
#include <memory>
#include <mutex>
#include <queue>
#include "../Rendering/Mesh/MeshDataChunk.h"



class ThreadQueueMeshes
{
public:
	void getQueue(std::queue<std::unique_ptr<MeshDataChunk>>& swapQueue);
	void mergeQueue(std::queue<std::unique_ptr<MeshDataChunk>>& mergeQueue);

private:
	std::queue<std::unique_ptr<MeshDataChunk>> meshQueue;
	std::mutex queueMutex;
};
