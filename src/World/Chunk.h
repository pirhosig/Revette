#pragma once
#include <map>
#include <memory>
#include <vector>

#include "Block.h"
#include "BlockContainter.h"
#include "ChunkPos.h"

class GeneratorChunkParameters;
class NoiseSource2D;
class World;
class Structure;



struct BlockChange
{
	Block block{Block(0)};
	unsigned long long age{0};
};



class Chunk
{
public:
	Chunk(ChunkPos _pos);
	Chunk(const Chunk&) = delete;

	void GenerateChunk(const GeneratorChunkParameters& generatorParameters, const NoiseSource2D& noiseFoliage);
	void PopulateChunk(World& world);

	Block getBlock(ChunkLocalBlockPos blockPos) const;
	void setBlock(ChunkLocalBlockPos blockPos, Block block);
	bool isEmpty() const;
	bool isGenerated() const;

	const ChunkPos position;

private:
	bool containsPosition(BlockPos blockPos) const;
	void setBlockPopulation(BlockPos blockPos, Block block, unsigned long long age);
	void addAdjacentPopulationChanges(Chunk& _chunk) const;

	bool generated;
	BlockContainer blockContainer;
	std::map<BlockPos, BlockChange> populationChanges;

	friend class Structure;
};
