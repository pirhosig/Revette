#include "ThreadQueueMeshes.h"



// Swap the internal queue with "swapQueue" if not empty
void ThreadQueueMeshes::getQueue(std::queue<std::unique_ptr<MeshChunk>>& swapQueue)
{
	std::lock_guard<std::mutex> lock(queueMutex);
	if (!meshQueue.empty()) std::swap(meshQueue, swapQueue);
}



// Insert all elements from "mergeQueue" into the internal queue, if the internal queue is empty then the queues are swapped
void ThreadQueueMeshes::mergeQueue(std::queue<std::unique_ptr<MeshChunk>>& mergeQueue)
{
	std::lock_guard<std::mutex> lock(queueMutex);

	if (meshQueue.empty()) std::swap(meshQueue, mergeQueue);
	else
	{
		while (!mergeQueue.empty())
		{
			meshQueue.push(std::move(mergeQueue.front()));
			mergeQueue.pop();
		}
	}
}
