#pragma once
#include <queue>
#include <mutex>



template <typename T>
class ThreadPointerQueue
{
public:
	void getQueue(std::queue<std::unique_ptr<T>>& swapQueue)
	{
		std::lock_guard<std::mutex> lock(queueMutex);
		if (!internalQueue.empty()) std::swap(internalQueue, swapQueue);
	}

	void mergeQueue(std::queue<std::unique_ptr<T>>& mergeQueue)
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
	std::queue<std::unique_ptr<T>> internalQueue;
	std::mutex queueMutex;
};