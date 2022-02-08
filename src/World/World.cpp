#include "World.h"
#include <cassert>
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
		assert((getChunkStatusLoad(lPos) == StatusChunkLoad::QUEUED) && "Attempted to load already loaded chunk.");

		// Load the chunk
		auto insertRes = chunkMap.insert({ lPos, std::make_unique<Chunk>(lPos) });
		setChunkStatusLoad(lPos, StatusChunkLoad::LOADED);
		// Generate the chunk
		insertRes.first->second->GenerateChunk(noiseHeightmap);
		setChunkStatusLoad(lPos, StatusChunkLoad::GENERATED);
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
		assert((getChunkStatusLoad(mPos) == StatusChunkLoad::GENERATED) && "Attempted to create mesh for chunk that has not finished loading");
		assert((getChunkStatusMesh(mPos) == StatusChunkMesh::QUEUED) && "Attempted to regenerate mesh.");

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
		setChunkStatusMesh(mPos, StatusChunkMesh::MESHED);
	}

	threadQueueMeshes->mergeQueue(meshDataQueue);
}



bool World::chunkExists(const ChunkPos chunkPos) const
{
	return chunkStatusMap.contains(chunkPos);
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
	else return chunkStatusIterator->second.getLoadStatus();
}



StatusChunkMesh World::getChunkStatusMesh(const ChunkPos chunkPos)
{
	auto chunkStatusIterator = chunkStatusMap.find(chunkPos);
	if (chunkStatusIterator == chunkStatusMap.end()) return StatusChunkMesh::NON_EXISTENT;
	else return chunkStatusIterator->second.getMeshStatus();
}



void World::setChunkStatusLoad(const ChunkPos chunkPos, StatusChunkLoad status)
{
	chunkStatusMap[chunkPos].setLoadStatus(status);
	if ((chunkStatusMap[chunkPos].getMeshStatus() == StatusChunkMesh::NON_EXISTENT)&&
		status == StatusChunkLoad::GENERATED &&
		chunkStatusMap[chunkPos].getNeighboursCardinalHaveStatus(StatusChunkLoad::GENERATED)
	) queueChunkMeshing(chunkPos);

	// Also update the neighbours of the chunk
	for (int i = -1; i < 2; ++i)
	{
		for (int j = -1; j < 2; ++j)
		{
			for (int k = -1; k < 2; ++k)
			{
				if (i == 0 && j == 0 && k == 0) continue;
				ChunkPos pos(chunkPos.x + i, chunkPos.y + j, chunkPos.z + k);
				if (chunkExists(pos))
				{
					chunkStatusMap[pos].setNeighbourLoadStatus(-i, -j, -k, status);
					if ((chunkStatusMap[pos].getMeshStatus() == StatusChunkMesh::NON_EXISTENT) &&
						chunkStatusMap[pos].getLoadStatus() == StatusChunkLoad::GENERATED &&
						chunkStatusMap[pos].getNeighboursCardinalHaveStatus(StatusChunkLoad::GENERATED)
					) queueChunkMeshing(pos);
				}
			}
		}
	}
}



void World::setChunkStatusMesh(const ChunkPos chunkPos, StatusChunkMesh status)
{
	chunkStatusMap[chunkPos].setHasMesh(status);
}



void World::queueChunkMeshing(const ChunkPos chunkPos)
{
	int loadCentreDistanceX = std::abs(chunkPos.x - loadCentre.x);
	int loadCentreDistanceY = std::abs(chunkPos.y - loadCentre.y);
	int loadCentreDistanceZ = std::abs(chunkPos.z - loadCentre.z);
	int meshPriority = std::max(100 - (loadCentreDistanceX + loadCentreDistanceY + loadCentreDistanceZ), 0);
	meshQueue.push(ChunkPriorityTicket(meshPriority, chunkPos));
	setChunkStatusMesh(chunkPos, StatusChunkMesh::QUEUED);
}
