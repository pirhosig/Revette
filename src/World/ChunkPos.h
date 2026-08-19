#pragma once
#include <functional>
#include "Core/RevetteCore.h"
#include "Block.h"




class ChunkOffset {
	int x;
	int y;
	int z;

public:
	ChunkOffset(i32 _x, i32 _y, i32 _z);

	i32 getX() const;
	i32 getY() const;
	i32 getZ() const;
};




class ChunkPos {
	i32 x;
	i32 y;
	i32 z;

public:
	ChunkPos(i32 _x, i32 _y, i32 _z);
	ChunkPos(BlockPos blockPos);

	bool operator==(const ChunkPos&) const = default;
	i32 getX() const;
	i32 getY() const;
	i32 getZ() const;

	ChunkPos direction(AxisDirection direction) const;
	double distanceEuclidean(ChunkPos other) const;
	i64 distanceEuclideanSquared(ChunkPos other) const;
	ChunkOffset offset(ChunkPos other) const;
};



class ChunkPos2D {
	i32 x;
	i32 z;

public:
	ChunkPos2D(i32 _x, i32 _z);
	ChunkPos2D(ChunkPos chunkPos);

	bool operator==(const ChunkPos2D&) const = default;
	i32 getX() const;
	i32 getZ() const;

	i64 distanceEuclideanSquared(ChunkPos2D other) const;
};



class ChunkLocalBlockPos {
	u16 pos;

public:
	explicit ChunkLocalBlockPos(u16 _pos);
	ChunkLocalBlockPos(u16 x, u16 y, u16 z);
	explicit ChunkLocalBlockPos(BlockPos blockPos);

	BlockPos asBlockPos(ChunkPos chunkPos) const;
	u16 asIndex() const;
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
