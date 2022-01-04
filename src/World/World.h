#pragma once
#include <queue>
#include <map>
#include <set>

#include "Chunk.h"



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

private:
	void addLoadQueue();
	void loadChunks();

	std::map<ChunkPos, std::unique_ptr<Chunk>> chunkMap;

	ChunkPos loadCentre;
	bool loadPosUpdated;
	std::priority_queue<ChunkLoadTicket> loadQueue;
	std::set<ChunkPos> loadQueuedChunks;
};