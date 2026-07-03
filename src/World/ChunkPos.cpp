#include "ChunkPos.h"
#include <cmath>



namespace {

inline int32_t chunkFloor(int32_t x)
{
	if (x < 0) return ((x - CHUNK_SIZE + 1) / CHUNK_SIZE);
	else return (x / CHUNK_SIZE);
}



inline int32_t wrapCoordinate(int32_t x)
{
	if (-WORLD_RADIUS_CHUNK <= x && x < WORLD_RADIUS_CHUNK) return x;
	x %= WORLD_DIAMETER_CHUNK;
	if (x < -WORLD_RADIUS_CHUNK)      x += WORLD_DIAMETER_CHUNK;
	else if (WORLD_RADIUS_CHUNK <= x) x -= WORLD_DIAMETER_CHUNK;
	return x;
}

}



ChunkOffset::ChunkOffset(int32_t _x, int32_t _y, int32_t _z) :
	x(wrapCoordinate(_x)),
	y(_y),
	z(wrapCoordinate(_z))
{}



int32_t ChunkOffset::getX() const { return x; }
int32_t ChunkOffset::getY() const { return y; }
int32_t ChunkOffset::getZ() const { return z; }



ChunkPos::ChunkPos(int32_t _x, int32_t _y, int32_t _z) :
	x(wrapCoordinate(_x)),
	y(_y),
	z(wrapCoordinate(_z))
{}



ChunkPos::ChunkPos(BlockPos blockPos) :
	x(chunkFloor(blockPos.getX())),
	y(chunkFloor(blockPos.getY())),
	z(chunkFloor(blockPos.getZ()))
{}



int32_t ChunkPos::getX() const { return x; }
int32_t ChunkPos::getY() const { return y; }
int32_t ChunkPos::getZ() const { return z; }



ChunkPos ChunkPos::direction(AxisDirection direction) const {
	static constexpr int32_t directionValues[6][3] {
		{ 0,  1,  0},
		{ 0, -1,  0},
		{ 1,  0,  0},
		{-1,  0,  0},
		{ 0,  0,  1},
		{ 0,  0, -1}
	};

	const int32_t directionIndex = static_cast<int32_t>(direction);

	return ChunkPos(
		x + directionValues[directionIndex][0],
		y + directionValues[directionIndex][1],
		z + directionValues[directionIndex][2]
	);
}



double ChunkPos::distanceEuclidean(ChunkPos other) const {
	ChunkOffset _offset = offset(other);
	return std::hypot(_offset.getX(), _offset.getY(), _offset.getZ());
}



int64_t ChunkPos::distanceEuclideanSquared(ChunkPos other) const {
	ChunkOffset _offset = offset(other);
	return (
		static_cast<int64_t>(_offset.getX()) * _offset.getX() +
		static_cast<int64_t>(_offset.getY()) * _offset.getY() +
		static_cast<int64_t>(_offset.getZ()) * _offset.getZ()
	);
}



ChunkOffset ChunkPos::offset(ChunkPos other) const {
	return ChunkOffset(other.x - x, other.y - y, other.z - z);
}


ChunkPos2D::ChunkPos2D(int32_t _x, int32_t _z) : x(wrapCoordinate(_x)), z(wrapCoordinate(_z)) {}



ChunkPos2D::ChunkPos2D(ChunkPos chunkPos) : x(chunkPos.getX()), z(chunkPos.getZ()) {}



int32_t ChunkPos2D::getX() const { return x; }
int32_t ChunkPos2D::getZ() const { return z; }



int64_t ChunkPos2D::distanceEuclideanSquared(ChunkPos2D other) const {
	int64_t offsetX = wrapCoordinate(other.x - x);
	int64_t offsetZ = wrapCoordinate(other.z - z);
	return offsetX * offsetX + offsetZ * offsetZ;
}



ChunkLocalBlockPos::ChunkLocalBlockPos(uint16_t x, uint16_t y, uint16_t z) {
	// Truncate the range of each position to [0, 32)
	x &= (32 - 1);
	y &= (32 - 1);
	z &= (32 - 1);
	pos = (x << 10) | (y << 5) | z;
}



ChunkLocalBlockPos::ChunkLocalBlockPos(uint16_t _pos) {
	pos = _pos & ((1u << 15) - 1);
}



ChunkLocalBlockPos::ChunkLocalBlockPos(BlockPos blockPos) {
	ChunkPos chunkPos(blockPos);
	uint16_t x = static_cast<uint16_t>(blockPos.getX() - (chunkPos.getX() * CHUNK_SIZE));
	uint16_t y = static_cast<uint16_t>(blockPos.getY() - (chunkPos.getY() * CHUNK_SIZE));
	uint16_t z = static_cast<uint16_t>(blockPos.getZ() - (chunkPos.getZ() * CHUNK_SIZE));
	pos = (x << 10) | (y << 5) | z;
}



BlockPos ChunkLocalBlockPos::asBlockPos(ChunkPos chunkPos) const
{
	int x = pos >> 10;
	int y = (pos >> 5) & (32 - 1);
	int z = pos & (32 - 1);
	return BlockPos(
		chunkPos.getX() * CHUNK_SIZE + x,
		chunkPos.getY() * CHUNK_SIZE + y,
		chunkPos.getZ() * CHUNK_SIZE + z
	);
}



uint16_t ChunkLocalBlockPos::asIndex() const {
	return pos;
}

