#pragma once
#include <queue>
#include <map>
#include <set>

#include "Chunk.h"
#include "ChunkPos.h"



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
	World();
	void tick();

	Block getBlock(BlockPos blockPos) const;
	void setBlock(BlockPos blockPos, Block block) const;

private:
	void addLoadQueue();
	void loadChunks();

	const std::unique_ptr<Chunk>& getChunk(const ChunkPos chunkPos) const;

	std::map<ChunkPos, std::unique_ptr<Chunk>> chunkMap;

	ChunkPos loadCentre;
	bool loadPosUpdated;
	std::priority_queue<ChunkLoadTicket> loadQueue;
	std::set<ChunkPos> loadQueuedChunks;
};