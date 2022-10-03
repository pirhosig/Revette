#include "ChunkPos.h"
#include <cmath>



inline int ChunkFloor(int x)
{
	if (x < 0) return ((x - CHUNK_SIZE + 1) / CHUNK_SIZE);
	else return (x / CHUNK_SIZE);
}



inline int wrapCoordinate(int x)
{
	if (-WORLD_RADIUS_CHUNK <= x && x < WORLD_RADIUS_CHUNK) return x;
	x %= WORLD_DIAMETER_CHUNK;
	if (x < -WORLD_RADIUS_CHUNK)      x += WORLD_DIAMETER_CHUNK;
	else if (WORLD_RADIUS_CHUNK <= x) x -= WORLD_DIAMETER_CHUNK;
	return x;
}



ChunkOffset::ChunkOffset(int _x, int _y, int _z) : x(wrapCoordinate(_x)), y(_y), z(wrapCoordinate(_z)) {}



ChunkPos::ChunkPos(int _x, int _y, int _z) : x(wrapCoordinate(_x)), y(_y), z(wrapCoordinate(_z)) {}



ChunkPos::ChunkPos(BlockPos blockPos) :
	x(ChunkFloor(blockPos.x)),
	y(ChunkFloor(blockPos.y)),
	z(ChunkFloor(blockPos.z)) {}



ChunkPos ChunkPos::direction(AxisDirection direction) const
{
	const int directionValues[6][3]
	{
		{ 0,  1,  0},
		{ 0, -1,  0},
		{ 1,  0,  0},
		{-1,  0,  0},
		{ 0,  0,  1},
		{ 0,  0, -1}
	};

	const int directionIndex = static_cast<int>(direction);

	return ChunkPos(
		x + directionValues[directionIndex][0],
		y + directionValues[directionIndex][1],
		z + directionValues[directionIndex][2]
	);
}



double ChunkPos::distance(ChunkPos other) const
{
	ChunkOffset _offset = offset(other);
	return std::hypot(_offset.x, _offset.y, _offset.z);
}



ChunkOffset ChunkPos::offset(ChunkPos other) const
{
	return ChunkOffset(other.x - x, other.y - y, other.z - z);
}



bool ChunkPos::operator<(const ChunkPos& other) const
{
	if (x != other.x) return x < other.x;
	else if (y != other.y) return y < other.y;
	else return z < other.z;
}


ChunkPos2D::ChunkPos2D(int _x, int _z) : x(wrapCoordinate(_x)), z(wrapCoordinate(_z)) {}



bool ChunkPos2D::operator<(const ChunkPos2D& other) const
{
	if (x != other.x) return x < other.x;
	return z < other.z;
}



BlockPos ChunkLocalBlockPos::asBlockPos(ChunkPos chunkPos) const
{
	return BlockPos(
		chunkPos.x * CHUNK_SIZE + x,
		chunkPos.y * CHUNK_SIZE + y,
		chunkPos.z * CHUNK_SIZE + z
	);
}

