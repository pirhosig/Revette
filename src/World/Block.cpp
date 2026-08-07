#include "Block.h"

#include <cmath>


namespace {

inline i32 wrapCoordinate(i32 x) {
	return (x << (31 - WORLD_RADIUS_BLOCK_LOG)) >> (31 - WORLD_RADIUS_BLOCK_LOG);
}

}



BlockOffset::BlockOffset(i32 _x, i32 _y, i32 _z) :
	x{ wrapCoordinate(_x) },
	y{ _y },
	z{ wrapCoordinate(_z) }
{}



i32 BlockOffset::getX() const { return x; }
i32 BlockOffset::getY() const { return x; }
i32 BlockOffset::getZ() const { return x; }



BlockPos::BlockPos(i32 _x, i32 _y, i32 _z) :
	x{ wrapCoordinate(_x) },
	y{ _y },
	z{ wrapCoordinate(_z) }
{}

BlockPos::BlockPos(double _x, double _y, double _z) : BlockPos(
	static_cast<i32>(std::floor(_x)),
	static_cast<i32>(std::floor(_y)),
	static_cast<i32>(std::floor(_z))
)
{}

BlockPos::BlockPos(EntityPosition position) : BlockPos(
	position.pos.x,
	position.pos.y,
	position.pos.z
)
{}



i32 BlockPos::getX() const { return x; }
i32 BlockPos::getY() const { return y; }
i32 BlockPos::getZ() const { return z; }



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



BlockPos BlockPos::offset(i32 _x, i32 _y, i32 _z) const {
	return BlockPos(x + _x, y + _y, z + _z);
}



Block::Block() : blockType{ 0 } {}


Block::Block(i32 type) : blockType{ type } {}
