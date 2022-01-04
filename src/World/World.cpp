#include "World.h"
#include <cmath>

constexpr int RENDER_DISTANCE = 10;



World::World() : loadCentre(0, 0, 0)
{
	loadPosUpdated = true;
}



void World::tick()
{
	if (loadPosUpdated) addLoadQueue();

	loadChunks();
}



void World::addLoadQueue()
{
	loadPosUpdated = false;
	for (int x = loadCentre.x - RENDER_DISTANCE; x < loadCentre.x + RENDER_DISTANCE; ++x)
	{
		for (int y = loadCentre.x - RENDER_DISTANCE; y < loadCentre.x + RENDER_DISTANCE; ++y)
		{
			for (int z = loadCentre.x - RENDER_DISTANCE; z < loadCentre.x + RENDER_DISTANCE; ++z)
			{
				if (loadQueuedChunks.count(ChunkPos(x, y, z))) continue;
				int distance = std::abs(x - loadCentre.x) + std::abs(y - loadCentre.y) + std::abs(z - loadCentre.z);
				int priority = std::max(100 - distance, 0);

				loadQueuedChunks.emplace(x, y, z);
				loadQueue.emplace(priority, x, y, z);
			}
		}
	}
}



void World::loadChunks()
{
	constexpr int LOAD_COUNT = 100;
	for (int i = 0; i < LOAD_COUNT; ++i)
	{
		if (loadQueue.empty()) break;
		ChunkPos lPos = loadQueue.top().pos;
		loadQueue.pop();

		chunkMap[lPos] = std::make_unique<Chunk>(lPos.x, lPos.y, lPos.z);
	}
}
