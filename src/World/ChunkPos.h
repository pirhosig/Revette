#pragma once
#include "Block.h"
#include "../Constants.h"



class ChunkPos
{
public:
	int x;
	int y;
	int z;

	ChunkPos(int x, int y, int z) : x(x), y(y), z(z) {}
	ChunkPos(BlockPos blockPos);
	ChunkPos direction(AxisDirection direction) const;

	bool operator<(const ChunkPos& other) const;
	bool operator==(const ChunkPos& other) const;
};



class ChunkLocalBlockPos
{
public:
	int x;
	int y;
	int z;

	ChunkLocalBlockPos(int x, int y, int z) : x(x), y(y), z(z) {}
	ChunkLocalBlockPos(BlockPos blockPos)
	{
		ChunkPos chunkPos(blockPos);
		x = blockPos.x - (chunkPos.x * CHUNK_SIZE);
		y = blockPos.y - (chunkPos.y * CHUNK_SIZE);
		z = blockPos.z - (chunkPos.z * CHUNK_SIZE);
	}

	BlockPos asBlockPos(ChunkPos chunkPos) const;
};