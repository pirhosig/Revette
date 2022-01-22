#pragma once
#include <map>
#include <memory>
#include <vector>

#include "Block.h"
#include "ChunkPos.h"



class Chunk
{
public:
	Chunk(ChunkPos _pos);
	Chunk(const Chunk&) = delete;
	Block getBlock(ChunkLocalBlockPos blockPos) const;
	void setBlock(ChunkLocalBlockPos blockPos, Block block);

	const ChunkPos position;

private:
	void createBlockArray();

	std::unique_ptr<uint16_t[]> blockArray;
	std::vector<Block> blockArrayBlocksByIndex;
	std::map<Block, uint16_t, blockComparator> blockArrayIndicesByBlock;
	uint16_t currentIndex;
};
