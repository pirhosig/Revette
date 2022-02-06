#pragma once
#include <map>
#include <memory>
#include <vector>

#include "Block.h"
#include "ChunkPos.h"
class NoiseSource2D;



class Chunk
{
public:
	Chunk(ChunkPos _pos);
	Chunk(const Chunk&) = delete;

	void GenerateChunk(NoiseSource2D& noiseHeightmap);

	Block getBlock(ChunkLocalBlockPos blockPos) const;
	void setBlock(ChunkLocalBlockPos blockPos, Block block);
	bool isEmpty() const;

	const ChunkPos position;
	bool generated;

private:
	void createBlockArray();
	void deleteBlockArray();

	std::unique_ptr<uint16_t[]> blockArray;
	std::vector<Block> blockArrayBlocksByIndex;
	std::map<Block, uint16_t, blockComparator> blockArrayIndicesByBlock;
	uint16_t currentIndex;
};
