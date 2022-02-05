#pragma once
#include <map>
#include <queue>
#include <set>

#include "Chunk.h"
#include "ChunkPos.h"
#include "../Rendering/Mesh/MeshDataChunk.h"
#include "../Threading/ThreadQueueMeshes.h"


class ChunkLoadTicket
{
public:
	int priority;
	ChunkPos pos;

	ChunkLoadTicket(int priority, int x, int y, int z) : priority(priority), pos(x, y, z) {}

	bool operator<(const ChunkLoadTicket& other) const
	{
		if (priority != other.priority) return priority < other.priority;
		else return pos < other.pos;
	}
};



class World
{
public:
	World(std::shared_ptr<ThreadQueueMeshes> meshQueue);
	World(const World&) = delete;
	void tick();

	Block getBlock(BlockPos blockPos) const;
	void setBlock(BlockPos blockPos, Block block) const;

private:
	void addLoadQueue();
	void loadChunks();

	bool chunkExists(const ChunkPos& chunkPos) const;
	const std::unique_ptr<Chunk>& getChunk(const ChunkPos& chunkPos) const;

	// Chunk storage
	std::map<ChunkPos, std::unique_ptr<Chunk>> chunkMap;

	// Chunk loading information
	ChunkPos loadCentre;
	bool loadPosUpdated;
	std::priority_queue<ChunkLoadTicket> loadQueue;
	std::set<ChunkPos> loadQueuedChunks;

	// Chunk mesh container
	std::shared_ptr<ThreadQueueMeshes> threadQueueMeshes;
	friend MeshDataChunk;
};