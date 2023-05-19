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
class MeshDataChunk;



struct BlockChange
{
	BlockPos pos;
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
	std::vector<bool> getSolidFaceMask(AxisDirection direction) const;
	void setBlock(ChunkLocalBlockPos blockPos, Block block);
	void setBlockPopulation(BlockPos blockPos, Block block, unsigned long long age);
	bool isEmpty() const;

	const ChunkPos position;

private:
	void addAdjacentPopulationChanges(std::unordered_map<BlockPos, std::pair<Block, unsigned long long>>& changes, ChunkPos pos) const;

	bool generated;
	BlockContainer blockContainer;
	std::vector<BlockChange> populationChangesAdjacent;
	std::vector<BlockChange> populationChangesInside;

	friend class Structure;
	friend class MeshDataChunk;
	friend class MeshChunkLOD;
};
