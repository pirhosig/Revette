#include "World.h"
#include <cmath>

#include "../Exceptions.h"

constexpr int LOAD_DISTANCE = 10;
constexpr int LOAD_DISTANCE_VERTICAL = 5;
constexpr int RENDER_DISTANCE = 9;
constexpr int RENDER_DISTANCE_VERTICAL = 4;



World::World(std::shared_ptr<ThreadQueueMeshes> meshQueue, const char* settingNoiseHeightmap, const char* settingNoiseFoliage) :
	loadCentre(0, 0, 0),
	threadQueueMeshes(meshQueue),
	noiseHeightmap(settingNoiseHeightmap, 0.00625f, 42),
	noiseFoliage(settingNoiseFoliage, 1.0, 42),
	loadPosUpdated{ true }
{}



void World::tick()
{
	if (loadPosUpdated) addLoadQueue();

	loadChunks();
	meshChunks();
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
	for (int x = loadCentre.x - LOAD_DISTANCE; x <= loadCentre.x + LOAD_DISTANCE; ++x)
	{
		for (int z = loadCentre.z - LOAD_DISTANCE; z <= loadCentre.z + LOAD_DISTANCE; ++z)
		{
			for (int y = loadCentre.y - LOAD_DISTANCE_VERTICAL; y <= loadCentre.y + LOAD_DISTANCE_VERTICAL; ++y)
			{
				ChunkPos loadPos(x, y, z);
				// Make sure chunks queued to be loaded do not exist yet
				if (getChunkStatusLoad(loadPos) != StatusChunkLoad::NON_EXISTENT) continue;
				int loadCentreDistanceX = std::abs(x - loadCentre.x);
				int loadCentreDistanceY = std::abs(y - loadCentre.y);
				int loadCentreDistanceZ = std::abs(z - loadCentre.z);
				int priority = std::max(100 - (loadCentreDistanceX + loadCentreDistanceY + loadCentreDistanceZ), 0);
				// Update chunk state
				setChunkStatusLoad(loadPos, StatusChunkLoad::QUEUED);
				loadQueue.push(ChunkPriorityTicket(priority, loadPos));
			}
		}
	}
}



void World::loadChunks()
{
	constexpr int MAX_LOAD_COUNT = 150;
	for (int i = 0; i < MAX_LOAD_COUNT; ++i)
	{
		if (loadQueue.empty()) break;
		ChunkPos lPos = loadQueue.top().pos;
		loadQueue.pop();

		// Make sure that the chunk is queued for loading (something has gone horribly wrong if it isn't)
		if (getChunkStatusLoad(lPos) == StatusChunkLoad::NON_EXISTENT) continue;

		// Load the chunk
		auto insertRes = chunkMap.insert({ lPos, std::make_unique<Chunk>(lPos) });
		setChunkStatusLoad(lPos, StatusChunkLoad::LOADED);
		// Generate the chunk
		insertRes.first->second->GenerateChunk(noiseHeightmap);
		setChunkStatusLoad(lPos, StatusChunkLoad::GENERATED);

		int loadCentreDistanceX = std::abs(lPos.x - loadCentre.x);
		int loadCentreDistanceY = std::abs(lPos.y - loadCentre.y);
		int loadCentreDistanceZ = std::abs(lPos.z - loadCentre.z);
		if (loadCentreDistanceX <= RENDER_DISTANCE && loadCentreDistanceZ <= RENDER_DISTANCE && loadCentreDistanceY <= RENDER_DISTANCE_VERTICAL)
		{
			int meshPriority = std::max(100 - (loadCentreDistanceX + loadCentreDistanceY + loadCentreDistanceZ), 0);
			meshQueue.push(ChunkPriorityTicket(meshPriority, lPos));
		}
	}
}



void World::meshChunks()
{
	std::queue<std::unique_ptr<MeshDataChunk>> meshDataQueue;

	constexpr int MAX_MESH_COUNT = 100;
	for (int i = 0; i < MAX_MESH_COUNT; ++i)
	{
		if (meshQueue.empty()) break;
		ChunkPos mPos = meshQueue.top().pos;

		// Make sure chunk is generated but does not have a mesh (the universe is broken if it isn't)
		if ((getChunkStatusLoad(mPos) != StatusChunkLoad::GENERATED) || (getChunkStatusMesh(mPos))) break;

		// Only continue if all chunks on cardinal directions are already generated
		bool generated = true;
		for (int j = 0; j < 6; ++j)
		{
			if (getChunkStatusLoad(mPos.direction(static_cast<AxisDirection>(j))) != StatusChunkLoad::GENERATED)
			{
				generated = false;
				break;
			}
		}
		if (!generated) break;

		meshQueue.pop();

		// Create a mesh if the chunk is not empty
		if (!getChunk(mPos)->isEmpty()) {
			std::unique_ptr<MeshDataChunk> meshData = std::make_unique<MeshDataChunk>((*this), mPos);
			if (meshData->triangleCount > 0) meshDataQueue.push(std::move(meshData));
		}
		setChunkStatusMesh(mPos, true);
	}

	threadQueueMeshes->mergeQueue(meshDataQueue);
}



bool World::chunkExists(const ChunkPos chunkPos) const
{
	return chunkMap.contains(chunkPos);
}



// More convenient access to a chunk object, the returned value is not intended to be stored beyond local scope.
const std::unique_ptr<Chunk>& World::getChunk(const ChunkPos chunkPos) const
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



StatusChunkLoad World::getChunkStatusLoad(const ChunkPos chunkPos) const
{
	auto chunkStatusIterator = chunkStatusMap.find(chunkPos);
	if (chunkStatusIterator == chunkStatusMap.end()) return StatusChunkLoad::NON_EXISTENT;
	else return chunkStatusIterator->second.loadStatus;
}



bool World::getChunkStatusMesh(const ChunkPos chunkPos)
{
	auto chunkStatusIterator = chunkStatusMap.find(chunkPos);
	if (chunkStatusIterator == chunkStatusMap.end()) return false;
	else return chunkStatusIterator->second.hasMesh;
}



void World::setChunkStatusLoad(const ChunkPos chunkPos, StatusChunkLoad status)
{
	chunkStatusMap[chunkPos].loadStatus = status;
}



void World::setChunkStatusMesh(const ChunkPos chunkPos, bool status)
{
	chunkStatusMap[chunkPos].hasMesh = true;
}
