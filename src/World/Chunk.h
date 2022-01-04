#pragma once
#include <map>
#include <memory>
#include <vector>

#include "Block.h"

constexpr int CHUNK_SIZE = 32;
constexpr int CHUNK_AREA = CHUNK_SIZE * CHUNK_SIZE;
constexpr int CHUNK_VOLUME = CHUNK_AREA * CHUNK_SIZE;



struct blockComparator
{
	bool operator()(const Block& a, const Block& b) const
	{
		return a.blockType < b.blockType;
	}
};



class ChunkPos
{
public:
	int x;
	int y;
	int z;

	ChunkPos(int x, int y, int z) : x(x), y(y), z(z) {}

	bool operator<(const ChunkPos& other) const
	{
		if (x != other.x) return x < other.x;
		else if (y != other.y) return y < other.y;
		else return z < other.z;
	}
};



class Chunk
{
public:
	Chunk(int x, int y, int z);
	Block getBlock(int x, int y, int z) const;
	void setBlock(int x, int y, int z, Block block);

	const ChunkPos position;

private:
	void createBlockArray();

	std::unique_ptr<uint16_t[]> blockArray;
	std::vector<Block> blockArrayBlocksByIndex;
	std::map<Block, uint16_t, blockComparator> blockArrayIndicesByBlock;
	uint16_t currentIndex;
};
