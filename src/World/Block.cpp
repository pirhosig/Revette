#include "Block.h"
#include "../Constants.h"



inline int wrapCoordinate(int x)
{
	if (-WORLD_RADIUS_BLOCK <= x && x < WORLD_RADIUS_BLOCK) return x;
	x %= WORLD_DIAMETER_BLOCK;
	if (x < -WORLD_RADIUS_BLOCK)      x += WORLD_DIAMETER_BLOCK;
	else if (WORLD_RADIUS_BLOCK <= x) x -= WORLD_DIAMETER_BLOCK;
	return x;
}



BlockOffset::BlockOffset(int _x, int _y, int _z) : x{ wrapCoordinate(_x) }, y{ _y }, z{ wrapCoordinate(_z) } {}



BlockPos::BlockPos(int x, int y, int z) : x(wrapCoordinate(x)), y(y), z(wrapCoordinate(z)) {}

BlockPos::BlockPos(EntityPosition position) :
	x(wrapCoordinate(static_cast<int>(position.pos.X))),
	y(static_cast<int>(position.pos.Y)),
	z(wrapCoordinate(static_cast<int>(position.pos.Z)))
{}



BlockPos BlockPos::direction(AxisDirection direction) const
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

	return BlockPos(
		x + directionValues[directionIndex][0],
		y + directionValues[directionIndex][1],
		z + directionValues[directionIndex][2]
	);
}



BlockOffset BlockPos::distance(BlockPos other) const { return BlockOffset(other.x - x, other.y - y, other.z - z); }



BlockPos BlockPos::offset(int _x, int _y, int _z) const { return BlockPos(x + _x, y + _y, z + _z); }



bool BlockPos::operator<(const BlockPos& other) const
{
	if (x != other.x) return x < other.x;
	else if (y != other.y) return y < other.y;
	else return z < other.z;
}



bool BlockPos::operator==(const BlockPos& other) const
{
	return x == other.x && y == other.y && z == other.z;
}



Block::Block() : blockType{ 0 } {}


Block::Block(int type) : blockType{ type } {}
