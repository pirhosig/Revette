#pragma once
#include "AxisDirection.h"



class BlockPos
{
public:
	int x;
	int y;
	int z;

	BlockPos(int x, int y, int z) : x(x), y(y), z(z) {}
	BlockPos direction(AxisDirection direction) const;

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