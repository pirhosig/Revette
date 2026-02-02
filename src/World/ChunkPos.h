#pragma once
#include <functional>
#include "Block.h"
#include "../Constants.h"



class ChunkOffset
{
public:
	int x;
	int y;
	int z;

	ChunkOffset(int _x, int _y, int _z);
};




class ChunkPos
{
public:
	int x;
	int y;
	int z;

	ChunkPos(int _x, int _y, int _z);
	ChunkPos(BlockPos blockPos);
	ChunkPos direction(AxisDirection direction) const;
	double distance(ChunkPos other) const;
	ChunkOffset offset(ChunkPos other) const;

	bool operator<(const ChunkPos& other) const;
	bool operator==(const ChunkPos& other) const = default;
};



class ChunkPos2D
{
public:
	int x;
	int z;

	ChunkPos2D(int _x, int _z);
	ChunkPos2D(ChunkPos chunkPos) : x(chunkPos.x), z(chunkPos.z) {}

	bool operator<(const ChunkPos2D& other) const;
	bool operator==(const ChunkPos2D& other) const = default;
};



class ChunkLocalBlockPos
{
public:
	int x;
	int y;
	int z;

	ChunkLocalBlockPos(int x, int y, int z) : x(x), y(y), z(z) {}
	ChunkLocalBlockPos(BlockPos blockPos)
	{
		ChunkPos chunkPos(blockPos);
		x = blockPos.x - (chunkPos.x * CHUNK_SIZE);
		y = blockPos.y - (chunkPos.y * CHUNK_SIZE);
		z = blockPos.z - (chunkPos.z * CHUNK_SIZE);
	}

	BlockPos asBlockPos(ChunkPos chunkPos) const;
};



template <>
struct std::hash<ChunkPos>
{
	std::size_t operator()(const ChunkPos& pos) const noexcept
	{
		std::size_t hash = std::hash<int>{}(pos.x);
		hash ^= std::hash<int>{}(pos.y) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
		hash ^= std::hash<int>{}(pos.z) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
		return hash;
	}
};



template <>
struct std::hash<ChunkPos2D>
{
	std::size_t operator()(const ChunkPos2D& pos) const noexcept
	{
		std::size_t hash = std::hash<int>{}(pos.x);
		hash ^= std::hash<int>{}(pos.z) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
		return hash;
	}
};
