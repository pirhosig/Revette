#include "ChunkPos.h"


inline int ChunkFloor(int x)
{
	if (x < 0) return (x - CHUNK_SIZE + 1) / CHUNK_SIZE;
	else return x / CHUNK_SIZE;
}



ChunkPos::ChunkPos(BlockPos blockPos) :
	x(ChunkFloor(blockPos.x)),
	y(ChunkFloor(blockPos.y)),
	z(ChunkFloor(blockPos.z)) {}



bool ChunkPos::operator<(const ChunkPos& other) const
{
	if (x != other.x) return x < other.x;
	else if (y != other.y) return y < other.y;
	else return z < other.z;
}



BlockPos ChunkLocalBlockPos::asBlockPos(ChunkPos chunkPos) const
{
	return BlockPos(
		chunkPos.x * CHUNK_SIZE + x,
		chunkPos.y * CHUNK_SIZE + y,
		chunkPos.z * CHUNK_SIZE + z
	);
}



bool ChunkLocalBlockPos::operator<(const BlockPos& other) const
{
	if (x != other.x) return x < other.x;
	else if (y != other.y) return y < other.y;
	else return z < other.z;
}