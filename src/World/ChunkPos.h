#pragma once
#include <functional>
#include "Block.h"
#include "../Constants.h"



class ChunkOffset {
	int x;
	int y;
	int z;

public:
	ChunkOffset(int32_t _x, int32_t _y, int32_t _z);

	int32_t getX() const;
	int32_t getY() const;
	int32_t getZ() const;
};




class ChunkPos {
	int32_t x;
	int32_t y;
	int32_t z;

public:
	ChunkPos(int32_t _x, int32_t _y, int32_t _z);
	ChunkPos(BlockPos blockPos);

	bool operator==(const ChunkPos&) const = default;
	int32_t getX() const;
	int32_t getY() const;
	int32_t getZ() const;

	ChunkPos direction(AxisDirection direction) const;
	double distanceEuclidean(ChunkPos other) const;
	int64_t distanceEuclideanSquared(ChunkPos other) const;
	ChunkOffset offset(ChunkPos other) const;
};



class ChunkPos2D {
	int32_t x;
	int32_t z;

public:
	ChunkPos2D(int32_t _x, int32_t _z);
	ChunkPos2D(ChunkPos chunkPos);

	bool operator==(const ChunkPos2D&) const = default;
	int32_t getX() const;
	int32_t getZ() const;

	int64_t distanceEuclideanSquared(ChunkPos2D other) const;
};



class ChunkLocalBlockPos {
	uint16_t pos;

public:
	ChunkLocalBlockPos(uint16_t _pos);
	ChunkLocalBlockPos(uint16_t x, uint16_t y, uint16_t z);
	ChunkLocalBlockPos(BlockPos blockPos);

	BlockPos asBlockPos(ChunkPos chunkPos) const;
	uint16_t asIndex() const;
};



template <>
struct std::hash<ChunkPos> {
	std::size_t operator()(const ChunkPos& pos) const noexcept
	{
		std::size_t hash = std::hash<int>{}(pos.getX());
		hash ^= std::hash<int>{}(pos.getY()) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
		hash ^= std::hash<int>{}(pos.getZ()) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
		return hash;
	}
};



template <>
struct std::hash<ChunkPos2D> {
	std::size_t operator()(const ChunkPos2D& pos) const noexcept
	{
		std::size_t hash = std::hash<int>{}(pos.getX());
		hash ^= std::hash<int>{}(pos.getZ()) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
		return hash;
	}
};
