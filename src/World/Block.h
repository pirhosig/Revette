#pragma once
#include "AxisDirection.h"
#include "Entities/EntityPosition.h"



struct BlockOffset {
public:
	int x;
	int y;
	int z;

	BlockOffset(int _x, int _y, int _z);
};



struct BlockPos {
public:
	int x;
	int y;
	int z;

	BlockPos(int x, int y, int z);
	BlockPos(double x, double y, double z);
	BlockPos(EntityPosition position);
	BlockPos direction(AxisDirection direction) const;
	BlockOffset distance(BlockPos other) const;
	BlockPos offset(int _x, int _y, int _z) const;

	bool operator<(const BlockPos& other) const;
	bool operator==(const BlockPos& other) const;
};



struct Block {
public:
	Block();
	Block(int type);

	int blockType;

	bool operator==(const Block& other) const { return blockType == other.blockType; }
};
