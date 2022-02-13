#include "World.h"
#include <cassert>
#include <cmath>

#include "../Exceptions.h"

constexpr int LOAD_DISTANCE = 20;
constexpr int LOAD_DISTANCE_VERTICAL = 5;



World::World(std::shared_ptr<ThreadQueueMeshes> meshQueue, const char* settingNoiseHeightmap, const char* settingNoiseFoliage) :
	loadCentre(0, 0, 0),
	threadQueueMeshes(meshQueue),
	noiseHeightmap(settingNoiseHeightmap, 0.0125, 42),
	noiseFoliage(settingNoiseFoliage, 1.0, 42),
	loadPosUpdated{ true }
{}



void World::tick()
{
	if (loadPosUpdated) addLoadQueue();

	loadChunks();
	populateChunks();
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
				if (chunkStatusMap.getChunkStatusLoad(loadPos) != StatusChunkLoad::NON_EXISTENT) continue;
				int dist = static_cast<int>(std::hypot(loadPos.x - loadCentre.x, loadPos.y - loadCentre.y, loadPos.z - loadCentre.z));
				int priority = std::max(100 - dist, 0);
				// Update chunk state
				chunkStatusMap.setChunkStatusLoad(loadPos, StatusChunkLoad::QUEUED_LOAD);
				loadQueue.push(ChunkPriorityTicket(priority, loadPos));
			}
		}
	}
}



void World::loadChunks()
{
	constexpr int MAX_LOAD_COUNT = 200;
	for (int i = 0; i < MAX_LOAD_COUNT; ++i)
	{
		if (loadQueue.empty()) break;
		ChunkPos lPos = loadQueue.top().pos;
		loadQueue.pop();

		// Make sure that the chunk is queued for loading (something has gone horribly wrong if it isn't)
		assert((chunkStatusMap.getChunkStatusLoad(lPos) == StatusChunkLoad::QUEUED_LOAD) && "Attempted to load already loaded chunk.");

		// Load the chunk
		auto insertRes = chunkMap.insert({ lPos, std::make_unique<Chunk>(lPos) });
		chunkStatusMap.setChunkStatusLoad(lPos, StatusChunkLoad::LOADED);

		// Generate the chunk
		insertRes.first->second->GenerateChunk(
			getHeightMap(ChunkPos2D(lPos))
		);
		chunkStatusMap.setChunkStatusLoad(lPos, StatusChunkLoad::GENERATED);
		// Check if it, or it's neighbours can populate
		for (int ni = -1; ni < 2; ++ni)
		{
			for (int nj = -1; nj < 2; ++nj)
			{
				for (int nk = -1; nk < 2; ++nk)
				{
					ChunkPos _pos(lPos.x + ni, lPos.y + nj, lPos.z + nk);
					if (chunkStatusMap.getChunkStatusCanPopulate(_pos)) queueChunkPopulation(_pos);
				}
			}
		}
	}
}



void World::populateChunks()
{
	constexpr int MAX_POPULATE_COUNT = 200;
	for (int i = 0; i < MAX_POPULATE_COUNT; ++i)
	{
		if (populateQueue.empty()) break;
		ChunkPos _pos = populateQueue.top().pos;
		populateQueue.pop();

		// Make sure that the chunk is queued for population
		assert((chunkStatusMap.getChunkStatusLoad(_pos) == StatusChunkLoad::QUEUED_POPULATE) && "Attempted to load already loaded chunk.");

		getChunk(_pos)->PopulateChunk(
			getHeightMap(ChunkPos2D(_pos)),
			noiseFoliage,
			(*this)
		);

		chunkStatusMap.setChunkStatusLoad(_pos, StatusChunkLoad::POPULATED);
		// Check if this chunk or any cardinal neighbours can generate meshes
		if (chunkStatusMap.getChunkStatusCanMesh(_pos)) queueChunkMeshing(_pos);

		const int NEIGHBOURS[7][3] = {
			{  0,  0,  0 },
			{  1,  0,  0 },
			{ -1,  0,  0 },
			{  0,  1,  0 },
			{  0, -1,  0 },
			{  0,  0,  1 },
			{  0,  0, -1 },
		};
		for (int j = 0; j < 7; ++j)
		{
			ChunkPos meshPos(_pos.x + NEIGHBOURS[j][0], _pos.y + NEIGHBOURS[j][1], _pos.z + NEIGHBOURS[j][2]);
			if (chunkStatusMap.getChunkStatusCanMesh(meshPos)) queueChunkMeshing(meshPos);
		}
	}
}



void World::meshChunks()
{
	std::queue<std::unique_ptr<MeshDataChunk>> meshDataQueue;

	constexpr int MAX_MESH_COUNT = 200;
	for (int i = 0; i < MAX_MESH_COUNT; ++i)
	{
		if (meshQueue.empty()) break;
		ChunkPos mPos = meshQueue.top().pos;
		meshQueue.pop();

		// Make sure chunk is generated but does not have a mesh (the universe is broken if it isn't)
		assert((chunkStatusMap.getChunkStatusLoad(mPos) == StatusChunkLoad::POPULATED) && "Attempted to create mesh for chunk that has not finished loading");
		assert((chunkStatusMap.getChunkStatusMesh(mPos) == StatusChunkMesh::QUEUED) && "Attempted to regenerate mesh.");

		// Create a mesh if the chunk is not empty
		if (!getChunk(mPos)->isEmpty()) {
			std::unique_ptr<MeshDataChunk> meshData = std::make_unique<MeshDataChunk>((*this), mPos);
			if (meshData->triangleCount > 0) meshDataQueue.push(std::move(meshData));
		}
		chunkStatusMap.setChunkStatusMesh(mPos, StatusChunkMesh::MESHED);
	}

	threadQueueMeshes->mergeQueue(meshDataQueue);
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



void World::queueChunkMeshing(const ChunkPos chunkPos)
{
	assert(chunkStatusMap.getChunkStatusCanMesh(chunkPos) && "Attempted to queue mesh that cannot be meshed");
	int dist = static_cast<int>(std::hypot(chunkPos.x - loadCentre.x, chunkPos.y - loadCentre.y, chunkPos.z - loadCentre.z));
	int meshPriority = std::max(100 - dist, 0);
	meshQueue.push(ChunkPriorityTicket(meshPriority, chunkPos));
	chunkStatusMap.setChunkStatusMesh(chunkPos, StatusChunkMesh::QUEUED);
}



void World::queueChunkPopulation(const ChunkPos chunkPos)
{
	assert(chunkStatusMap.getChunkStatusCanPopulate(chunkPos) && "Attempted to populate chunk that cannot be populated");
	int dist = static_cast<int>(std::hypot(chunkPos.x - loadCentre.x, chunkPos.y - loadCentre.y, chunkPos.z - loadCentre.z));
	int priority = std::max(100 - dist, 0);
	populateQueue.push(ChunkPriorityTicket(priority, chunkPos));
	chunkStatusMap.setChunkStatusLoad(chunkPos, StatusChunkLoad::QUEUED_POPULATE);
}



const HeightMap& World::getHeightMap(const ChunkPos2D noisePos)
{
	// Generate a heightmap if it doesn't exist yet
	if (!noiseHeightCache.contains(noisePos)) noiseHeightCache.try_emplace(noisePos, noisePos, noiseHeightmap);
	// Return a reference to the heightmap
	return noiseHeightCache.at(noisePos);
}
