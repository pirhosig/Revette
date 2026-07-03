#pragma once
#include <functional>

#include "Block.h"



template <>
struct std::hash<BlockPos> {
	std::size_t operator()(const BlockPos& pos) const noexcept
	{
		std::size_t hash = std::hash<int>{}(pos.getX());
		hash ^= std::hash<int>{}(pos.getY()) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
		hash ^= std::hash<int>{}(pos.getZ()) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
		return hash;
	}
};
