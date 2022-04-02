#pragma once
#include <map>
#include <memory>
#include <vector>

#include "Block.h"
#include "ChunkPos.h"
class GeneratorChunkParameters;
class NoiseSource2D;
class World;



enum class BlockArrayType
{
	NONE,
	COMPACT,
	EXTENDED
};



class Chunk
{
public:
	Chunk(ChunkPos _pos);
	Chunk(const Chunk&) = delete;

	void GenerateChunk(const GeneratorChunkParameters& generatorParameters);
	void PopulateChunk(const GeneratorChunkParameters& generatorParameters, const NoiseSource2D& noiseFoliage, World& world);

	Block getBlock(ChunkLocalBlockPos blockPos) const;
	void setBlock(ChunkLocalBlockPos blockPos, Block block);
	bool isEmpty() const;

	const ChunkPos position;
	bool generated;

private:
	void blockArrayCreate();
	void blockArrayDelete();
	void blockArrayExtend();

	void setBlockRaw(int arrayIndex, int blockIndex);

	std::unique_ptr<uint8_t[]> blockArrayCompact;
	std::unique_ptr<uint16_t[]> blockArrayExtended;
	BlockArrayType blockArrayType;

	std::vector<Block> blockArrayBlocksByIndex;
	std::map<Block, uint16_t, blockComparator> blockArrayIndicesByBlock;
	uint16_t currentIndex;
};
