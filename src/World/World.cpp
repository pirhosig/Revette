#include "World.h"
#include <cmath>

#include "../Exceptions.h"

constexpr int RENDER_DISTANCE = 10;
constexpr int RENDER_DISTANCE_VERTICAL = 5;



World::World(std::shared_ptr<ThreadQueueMeshes> meshQueue, const char* settingNoiseHeightmap) :
	loadCentre(0, 0, 0),
	threadQueueMeshes(meshQueue),
	noiseHeightmap(FastNoise::NewFromEncodedNodeTree(settingNoiseHeightmap))
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
		for (int z = loadCentre.z - RENDER_DISTANCE; z < loadCentre.z + RENDER_DISTANCE; ++z)
		{
			for (int y = loadCentre.y - RENDER_DISTANCE_VERTICAL; y < loadCentre.y + RENDER_DISTANCE_VERTICAL; ++y)
			{
				if (loadQueuedChunks.contains(ChunkPos(x, y, z))) continue;
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
	std::queue<std::unique_ptr<MeshDataChunk>> meshQueue;

	constexpr int LOAD_COUNT = 100;
	for (int i = 0; i < LOAD_COUNT; ++i)
	{
		if (loadQueue.empty()) break;
		ChunkPos lPos = loadQueue.top().pos;
		loadQueue.pop();

		auto insertRes = chunkMap.insert({ lPos, std::make_unique<Chunk>(lPos) });
		// Load the chunk
		insertRes.first->second->GenerateChunk(noiseHeightmap);
		// Create a mesh if the chunk is not empty
		if (!getChunk(lPos)->isEmpty()) {
			std::unique_ptr<MeshDataChunk> meshData = std::make_unique<MeshDataChunk>((*this), lPos);
			if (meshData->triangleCount > 0) meshQueue.push(std::move(meshData));
		}
	}

	threadQueueMeshes->mergeQueue(meshQueue);
}



bool World::chunkExists(const ChunkPos& chunkPos) const
{
	return chunkMap.contains(chunkPos);
}



// More convenient access to a chunk object, the returned value is not intended to be stored beyond local scope.
const std::unique_ptr<Chunk>& World::getChunk(const ChunkPos& chunkPos) const
{
	try
	{
		return chunkMap.at(chunkPos);
	}
	catch (std::out_of_range)
	{
		std::string error = "Attempted to access non-existent chunk at ";
		error += std::to_string(chunkPos.x) + " " + std::to_string(chunkPos.y) + " " + std::to_string(chunkPos.z);
		throw EXCEPTION_WORLD::ChunkNonExistence(error);
	}
}
