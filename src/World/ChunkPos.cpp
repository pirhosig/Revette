#include "ChunkPos.h"
#include <cmath>



namespace {

inline i32 wrapChunkCoordinate(i32 x) {
	return (x << (31 - WORLD_RADIUS_CHUNK_LOG)) >> (31 - WORLD_RADIUS_CHUNK_LOG);
}

}



ChunkOffset::ChunkOffset(i32 _x, i32 _y, i32 _z) :
	x(wrapChunkCoordinate(_x)),
	y(_y),
	z(wrapChunkCoordinate(_z))
{}



i32 ChunkOffset::getX() const { return x; }
i32 ChunkOffset::getY() const { return y; }
i32 ChunkOffset::getZ() const { return z; }



ChunkPos::ChunkPos(i32 _x, i32 _y, i32 _z) :
	x(wrapChunkCoordinate(_x)),
	y(_y),
	z(wrapChunkCoordinate(_z))
{}



ChunkPos::ChunkPos(BlockPos blockPos) :
	x{blockPos.getX() >> CHUNK_SIZE_LOG},
	y{blockPos.getY() >> CHUNK_SIZE_LOG},
	z{blockPos.getZ() >> CHUNK_SIZE_LOG}
{}



i32 ChunkPos::getX() const { return x; }
i32 ChunkPos::getY() const { return y; }
i32 ChunkPos::getZ() const { return z; }



ChunkPos ChunkPos::direction(AxisDirection direction) const {
	static constexpr i32 directionValues[6][3] {
		{ 0,  1,  0},
		{ 0, -1,  0},
		{ 1,  0,  0},
		{-1,  0,  0},
		{ 0,  0,  1},
		{ 0,  0, -1}
	};

	const i32 directionIndex = static_cast<i32>(direction);

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



i64 ChunkPos::distanceEuclideanSquared(ChunkPos other) const {
	ChunkOffset _offset = offset(other);
	return (
		static_cast<i64>(_offset.getX()) * _offset.getX() +
		static_cast<i64>(_offset.getY()) * _offset.getY() +
		static_cast<i64>(_offset.getZ()) * _offset.getZ()
	);
}



ChunkOffset ChunkPos::offset(ChunkPos other) const {
	return ChunkOffset(other.x - x, other.y - y, other.z - z);
}


ChunkPos2D::ChunkPos2D(i32 _x, i32 _z) : x(wrapChunkCoordinate(_x)), z(wrapChunkCoordinate(_z)) {}



ChunkPos2D::ChunkPos2D(ChunkPos chunkPos) : x(chunkPos.getX()), z(chunkPos.getZ()) {}



i32 ChunkPos2D::getX() const { return x; }
i32 ChunkPos2D::getZ() const { return z; }



i64 ChunkPos2D::distanceEuclideanSquared(ChunkPos2D other) const {
	i64 offsetX = wrapChunkCoordinate(other.x - x);
	i64 offsetZ = wrapChunkCoordinate(other.z - z);
	return offsetX * offsetX + offsetZ * offsetZ;
}



ChunkLocalBlockPos::ChunkLocalBlockPos(u16 x, u16 y, u16 z) {
	// Truncate the range of each position to [0, 32)
	x &= (32 - 1);
	y &= (32 - 1);
	z &= (32 - 1);
	pos = (x << 10) | (y << 5) | z;
}



ChunkLocalBlockPos::ChunkLocalBlockPos(u16 _pos) {
	pos = _pos & ((1u << 15) - 1);
}



ChunkLocalBlockPos::ChunkLocalBlockPos(BlockPos blockPos) {
	ChunkPos chunkPos(blockPos);
	u16 x = static_cast<u16>(blockPos.getX() - (chunkPos.getX() * CHUNK_SIZE));
	u16 y = static_cast<u16>(blockPos.getY() - (chunkPos.getY() * CHUNK_SIZE));
	u16 z = static_cast<u16>(blockPos.getZ() - (chunkPos.getZ() * CHUNK_SIZE));
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



u16 ChunkLocalBlockPos::asIndex() const {
	return pos;
}

