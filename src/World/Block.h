#pragma once
#include "Core/RevetteCore.h"
#include "AxisDirection.h"
#include "Entities/EntityPosition.h"



class BlockOffset {
	i32 x;
	i32 y;
	i32 z;

public:
	BlockOffset(i32 _x, i32 _y, i32 _z);

	i32 getX() const;
	i32 getY() const;
	i32 getZ() const;
};



class BlockPos {
	i32 x;
	i32 y;
	i32 z;

public:
	BlockPos(i32 x, i32 y, i32 z);
	BlockPos(double x, double y, double z);
	BlockPos(EntityPosition position);

	bool operator==(const BlockPos&) const = default;
	i32 getX() const;
	i32 getY() const;
	i32 getZ() const;

	BlockPos direction(AxisDirection direction) const;
	BlockOffset distance(BlockPos other) const;
	BlockPos offset(i32 _x, i32 _y, i32 _z) const;
};



struct Block {
	i32 blockType;

public:
	Block();
	Block(i32 type);

	bool operator==(const Block&) const = default;
};
