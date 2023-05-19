#pragma once
#include "ChunkPos.h"



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
