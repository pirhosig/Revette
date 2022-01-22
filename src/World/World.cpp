#include "World.h"
#include <cmath>

constexpr int RENDER_DISTANCE = 10;



World::World(std::shared_ptr<ThreadQueueMeshes> meshQueue) : loadCentre(0, 0, 0), threadQueueMeshes(meshQueue)
{
	loadPosUpdated = true;
}



void World::tick()
{
	if (loadPosUpdated) addLoadQueue();

	loadChunks();
}



Block World::getBlock(BlockPos blockPos) const
{
	return getChunk(ChunkPos(blockPos))->getBlock(ChunkLocalBlockPos(blockPos));
}



void World::setBlock(BlockPos blockPos, Block block) const
{
	getChunk(ChunkPos(blockPos))->setBlock(ChunkLocalBlockPos(blockPos), block);
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

		chunkMap[lPos] = std::make_unique<Chunk>(lPos);
	}
}




// More convenient access to a chunk object, the returned value is not intended to be stored beyond local scope.
const std::unique_ptr<Chunk>& World::getChunk(const ChunkPos chunkPos) const
{
	return chunkMap.at(chunkPos);
}
