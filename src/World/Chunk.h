#pragma once
#include <memory>
#include <unordered_map>
#include <vector>

#include "Block.h"
#include "BlockContainter.h"
#include "ChunkPos.h"

class GeneratorChunkParameters;
class World;
class Structure;
class MeshChunk;



struct BlockChange
{
	BlockPos pos;
	Block block{Block(0)};
	unsigned age{0};
};



class Chunk
{
private:
	bool generated;
	BlockContainer blockContainer;
	std::vector<BlockChange> populationChangesAdjacent;
	std::vector<BlockChange> populationChangesInside;
	const ChunkPos position;

public:
	Chunk(ChunkPos _pos);
	
	Chunk(Chunk&&) = delete;
	Chunk(const Chunk&) = delete;
	Chunk operator=(Chunk&&) = delete;
	Chunk operator=(const Chunk&) = delete;

	void GenerateChunk(const GeneratorChunkParameters& generatorParameters);
	void PopulateChunk(World& world);

	Block getBlock(ChunkLocalBlockPos blockPos) const;
	std::vector<bool> getSolidFaceMask(AxisDirection direction) const;
	void setBlock(ChunkLocalBlockPos blockPos, Block block);
	void setBlockPopulation(BlockPos blockPos, Block block, unsigned age);
	bool isEmpty() const;

private:
	void addAdjacentPopulationChanges(std::unordered_map<BlockPos, std::pair<Block, unsigned>>& changes, ChunkPos pos) const;

	friend class Structure;
	friend class MeshChunk;
};
