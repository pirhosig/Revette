#pragma once
#include "AxisDirection.h"
#include "Entities/EntityPosition.h"



class BlockPos
{
public:
	int x;
	int y;
	int z;

	BlockPos(int x, int y, int z);
	BlockPos(EntityPosition position) : x(static_cast<int>(position.X)), y(static_cast<int>(position.Y)), z(static_cast<int>(position.Z)) {}
	BlockPos direction(AxisDirection direction) const;
	BlockPos offset(int _x, int _y, int _z);

	bool operator<(const BlockPos& other) const;
};



class Block
{
public:
	Block();
	Block(int type);

	int blockType;
};



struct blockComparator
{
	bool operator()(const Block& a, const Block& b) const
	{
		return a.blockType < b.blockType;
	}
};