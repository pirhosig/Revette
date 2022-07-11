#pragma once
#include <map>

#include "ChunkPos.h"
#include "StatusChunk.h"



class ChunkStatusMap
{
public:
	bool chunkExists(const ChunkPos chunkPos) const;
	bool getChunkStatusCanMesh(const ChunkPos chunkPos) const;
	bool getChunkStatusCanPopulate(const ChunkPos chunkPos) const;
	StatusChunkLoad getChunkStatusLoad(const ChunkPos chunkPos) const;
	StatusChunkMesh getChunkStatusMesh(const ChunkPos chunkPos) const;
	void setChunkStatusLoad(const ChunkPos chunkPos, StatusChunkLoad status);
	void setChunkStatusMesh(const ChunkPos chunkPos, StatusChunkMesh status);

	std::map<ChunkPos, StatusChunk> statusMap;
};