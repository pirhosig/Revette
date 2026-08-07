#pragma once
#include <memory>
#include <unordered_map>
#include <vector>

#include "Core/RevetteCore.h"
#include "Block.h"
#include "BlockContainer.h"
#include "ChunkPos.h"



struct BlockChange {
	BlockPos pos;
	Block block{Block(0)};
	u32 age{0};
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

	void GenerateChunk(const class GeneratorChunkParameters& generatorParameters);
	void PopulateChunk(class World& world);

	Block getBlock(ChunkLocalBlockPos blockPos) const;
	std::vector<bool> getSolidFaceMask(AxisDirection direction) const;
	void setBlock(ChunkLocalBlockPos blockPos, Block block);
	void setBlockPopulation(BlockPos blockPos, Block block, u32 age);
	bool shouldSkipMeshing() const;

private:
	void addAdjacentPopulationChanges(std::unordered_map<BlockPos, std::pair<Block, u32>>& changes, ChunkPos pos) const;

	friend class Structure;
	friend class MeshChunk;
};
