#pragma once
#include "AxisDirection.h"
#include "Entities/EntityPosition.h"



class BlockOffset {
	int32_t x;
	int32_t y;
	int32_t z;

public:
	BlockOffset(int32_t _x, int32_t _y, int32_t _z);

	int32_t getX() const;
	int32_t getY() const;
	int32_t getZ() const;
};



class BlockPos {
	int32_t x;
	int32_t y;
	int32_t z;

public:
	BlockPos(int32_t x, int32_t y, int32_t z);
	BlockPos(double x, double y, double z);
	BlockPos(EntityPosition position);

	bool operator==(const BlockPos&) const = default;
	int32_t getX() const;
	int32_t getY() const;
	int32_t getZ() const;

	BlockPos direction(AxisDirection direction) const;
	BlockOffset distance(BlockPos other) const;
	BlockPos offset(int32_t _x, int32_t _y, int32_t _z) const;
};



struct Block {
	int32_t blockType;

public:
	Block();
	Block(int32_t type);

	bool operator==(const Block&) const = default;
};
