#pragma once
#include <map>
#include <queue>
#include <set>

#include "Chunk.h"
#include "ChunkPos.h"
#include "StatusChunk.h"
#include "Generation/HeightMap.h"
#include "Generation/NoiseSource.h"
#include "../Rendering/Mesh/MeshDataChunk.h"
#include "../Threading/ThreadQueueMeshes.h"



class ChunkPriorityTicket
{
public:
	int priority;
	ChunkPos pos;

	ChunkPriorityTicket(int _priority, ChunkPos _pos) : priority{ _priority }, pos{ _pos } {}

	bool operator<(const ChunkPriorityTicket& other) const
	{
		if (priority != other.priority) return priority < other.priority;
		else return pos < other.pos;
	}
};



class World
{
public:
	World(std::shared_ptr<ThreadQueueMeshes> meshQueue, const char* settingNoiseHeightmap, const char* settingNoiseFoliage);
	World(const World&) = delete;
	void tick();

	Block getBlock(BlockPos blockPos) const;
	void setBlock(BlockPos blockPos, Block block) const;

private:

	void addLoadQueue();
	void loadChunks();
	void populateChunks();
	void meshChunks();

	bool chunkExists(const ChunkPos chunkPos) const;
	const std::unique_ptr<Chunk>& getChunk(const ChunkPos chunkPos) const;
	StatusChunkLoad getChunkStatusLoad(const ChunkPos chunkPos) const;
	StatusChunkMesh getChunkStatusMesh(const ChunkPos chunkPos) const;
	void setChunkStatusLoad(const ChunkPos chunkPos, StatusChunkLoad status);
	void setChunkStatusMesh(const ChunkPos chunkPos, StatusChunkMesh status);
	bool getChunkStatusCanMesh(const ChunkPos chunkPos) const;
	bool getChunkStatusCanPopulate(const ChunkPos chunkPos) const;

	void queueChunkMeshing(const ChunkPos chunkPos);
	void queueChunkPopulation(const ChunkPos chunkPos);

	const HeightMap& getHeightMap(const ChunkPos2D noisePos);

	// Chunk storage
	std::map<ChunkPos, std::unique_ptr<Chunk>> chunkMap;

	// Chunk loading information
	ChunkPos loadCentre;
	bool loadPosUpdated;
	std::map<ChunkPos, StatusChunk> chunkStatusMap;
	std::priority_queue<ChunkPriorityTicket> loadQueue;
	std::priority_queue<ChunkPriorityTicket> populateQueue;
	std::priority_queue<ChunkPriorityTicket> meshQueue;

	// Chunk generation tools
	std::map<ChunkPos2D, HeightMap> noiseHeightCache;
	NoiseSource2D noiseHeightmap;
	NoiseSource2D noiseFoliage;

	// Chunk mesh container
	std::shared_ptr<ThreadQueueMeshes> threadQueueMeshes;
	friend MeshDataChunk;
};