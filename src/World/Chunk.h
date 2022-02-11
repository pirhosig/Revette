#pragma once
#include <map>
#include <memory>
#include <vector>

#include "Block.h"
#include "ChunkPos.h"
class HeightMap;
class NoiseSource2D;
class World;



class Chunk
{
public:
	Chunk(ChunkPos _pos);
	Chunk(const Chunk&) = delete;

	void GenerateChunk(const HeightMap& noiseHeightmap);
	void PopulateChunk(const HeightMap& noiseHeightmap, const NoiseSource2D& noiseFoliage, World& world);

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
