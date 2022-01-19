#include "Block.h"


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


bool BlockPos::operator<(const BlockPos& other) const
{
	if (x != other.x) return x < other.x;
	else if (y != other.y) return y < other.y;
	else return z < other.z;
}



Block::Block() : blockType{ 0 } {}


Block::Block(int type) : blockType{ type } {}
