#pragma once
#include <map>
#include <queue>
#include <set>

#include "Chunk.h"
#include "ChunkPos.h"
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



enum class StatusChunkLoad
{
	NON_EXISTENT,
	QUEUED,
	LOADED,
	GENERATED,
};



struct StatusChunk
{
	StatusChunkLoad loadStatus{ StatusChunkLoad::NON_EXISTENT };
	bool hasMesh{ false };
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
	void meshChunks();

	bool chunkExists(const ChunkPos chunkPos) const;
	const std::unique_ptr<Chunk>& getChunk(const ChunkPos chunkPos) const;
	StatusChunkLoad getChunkStatusLoad(const ChunkPos chunkPos) const;
	bool getChunkStatusMesh(const ChunkPos chunkPos);
	void setChunkStatusLoad(const ChunkPos chunkPos, StatusChunkLoad status);
	void setChunkStatusMesh(const ChunkPos chunkPos, bool status);

	// Chunk storage
	std::map<ChunkPos, std::unique_ptr<Chunk>> chunkMap;

	// Chunk loading information
	ChunkPos loadCentre;
	bool loadPosUpdated;
	std::map<ChunkPos, StatusChunk> chunkStatusMap;
	std::priority_queue<ChunkPriorityTicket> loadQueue;
	std::priority_queue<ChunkPriorityTicket> meshQueue;

	// Chunk generation tools
	NoiseSource2D noiseHeightmap;
	NoiseSource2D noiseFoliage;

	// Chunk mesh container
	std::shared_ptr<ThreadQueueMeshes> threadQueueMeshes;
	friend MeshDataChunk;
};