#pragma once
#include <map>
#include <memory>
#include <vector>

#include <FastNoise/FastNoise.h>

#include "Block.h"
#include "ChunkPos.h"



class Chunk
{
public:
	Chunk(ChunkPos _pos);
	Chunk(const Chunk&) = delete;

	void GenerateChunk(FastNoise::SmartNode<>& noiseHeightmap);

	Block getBlock(ChunkLocalBlockPos blockPos) const;
	void setBlock(ChunkLocalBlockPos blockPos, Block block);
	bool isEmpty() const;

	const ChunkPos position;
	bool generated;

private:
	void createBlockArray();

	std::unique_ptr<uint16_t[]> blockArray;
	std::vector<Block> blockArrayBlocksByIndex;
	std::map<Block, uint16_t, blockComparator> blockArrayIndicesByBlock;
	uint16_t currentIndex;
};
