#pragma once
#include <mutex>
#include <queue>



template <typename T>
class ThreadQueue
{
public:
	void getQueue(std::queue<T>& swapQueue)
	{
		std::lock_guard<std::mutex> lock(queueMutex);
		if (!internalQueue.empty()) std::swap(internalQueue, swapQueue);
	}

	void mergeQueue(std::queue<T>& mergeQueue)
	{
		std::lock_guard<std::mutex> lock(queueMutex);
		// Simply exchange queues if the internal one is empty
		if (internalQueue.empty()) std::swap(internalQueue, mergeQueue);
		// Otherwise move items from mergeQueue to internalQueue
		else
		{
			while (!mergeQueue.empty())
			{
				internalQueue.push(std::move(mergeQueue.front()));
				mergeQueue.pop();
			}
		}
	}

private:
	std::queue<T> internalQueue;
	std::mutex queueMutex;
};