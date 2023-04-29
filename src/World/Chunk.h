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
class MeshDataChunk;



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

	void GenerateChunk(const GeneratorChunkParameters& generatorParameters);
	void PopulateChunk(World& world);

	Block getBlock(ChunkLocalBlockPos blockPos) const;
	void setBlock(ChunkLocalBlockPos blockPos, Block block);
	void setBlockPopulation(BlockPos blockPos, Block block, unsigned long long age);
	bool isEmpty() const;

	const ChunkPos position;

private:
	void addAdjacentPopulationChanges(Chunk& _chunk) const;

	bool generated;
	BlockContainer blockContainer;
	std::map<BlockPos, BlockChange> populationChanges;

	friend class Structure;
	friend class MeshDataChunk;
};
