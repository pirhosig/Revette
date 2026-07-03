#include "Block.h"

#include <cmath>

#include "../Constants.h"


namespace {

inline int wrapCoordinate(int32_t x) {
	if (-WORLD_RADIUS_BLOCK <= x && x < WORLD_RADIUS_BLOCK) return x;
	x %= WORLD_DIAMETER_BLOCK;
	if (x < -WORLD_RADIUS_BLOCK)      x += WORLD_DIAMETER_BLOCK;
	else if (WORLD_RADIUS_BLOCK <= x) x -= WORLD_DIAMETER_BLOCK;
	return x;
}

}



BlockOffset::BlockOffset(int32_t _x, int32_t _y, int32_t _z) :
	x{ wrapCoordinate(_x) },
	y{ _y },
	z{ wrapCoordinate(_z) }
{}



int32_t BlockOffset::getX() const { return x; }
int32_t BlockOffset::getY() const { return x; }
int32_t BlockOffset::getZ() const { return x; }



BlockPos::BlockPos(int32_t _x, int32_t _y, int32_t _z) :
	x{ wrapCoordinate(_x) },
	y{ _y },
	z{ wrapCoordinate(_z) }
{}

BlockPos::BlockPos(double _x, double _y, double _z) : BlockPos(
	static_cast<int32_t>(std::floor(_x)),
	static_cast<int32_t>(std::floor(_y)),
	static_cast<int32_t>(std::floor(_z))
)
{}

BlockPos::BlockPos(EntityPosition position) : BlockPos(
	position.pos.x,
	position.pos.y,
	position.pos.z
)
{}



int32_t BlockPos::getX() const { return x; }
int32_t BlockPos::getY() const { return y; }
int32_t BlockPos::getZ() const { return z; }



BlockPos BlockPos::direction(AxisDirection direction) const {
	static constexpr int directionValues[6][3]
	{
		{ 0,  1,  0},
		{ 0, -1,  0},
		{ 1,  0,  0},
		{-1,  0,  0},
		{ 0,  0,  1},
		{ 0,  0, -1}
	};

	const int directionIndex = static_cast<int>(direction);

	return BlockPos(
		x + directionValues[directionIndex][0],
		y + directionValues[directionIndex][1],
		z + directionValues[directionIndex][2]
	);
}



BlockOffset BlockPos::distance(BlockPos other) const {
	return BlockOffset(other.x - x, other.y - y, other.z - z);
}



BlockPos BlockPos::offset(int32_t _x, int32_t _y, int32_t _z) const {
	return BlockPos(x + _x, y + _y, z + _z);
}



Block::Block() : blockType{ 0 } {}


Block::Block(int32_t type) : blockType{ type } {}
