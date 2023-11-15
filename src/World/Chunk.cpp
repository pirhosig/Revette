#include "Chunk.h"
#include <array>
#include <cassert>
#include <string>

#include "BlockHash.h"
#include "Generation/ChunkPRNG.h"
#include "Generation/GeneratorChunkParameters.h"
#include "Generation/Structures/StructurePlants.h"
#include "Generation/Structures/StructuresRuins.h"
#include "World.h"
#include "../Constants.h"
#include "../Exceptions.h"
#include "../Math/ProbabilityTable.h"



inline int blockPositionIsInside(int x, int y, int z)
{
	return (x >= 0) && (x < CHUNK_SIZE) && (y >= 0) && (y < CHUNK_SIZE) && (z >= 0) && (z < CHUNK_SIZE);
}



Chunk::Chunk(ChunkPos _pos) : position(_pos), generated{ false } {}



void Chunk::GenerateChunk(const GeneratorChunkParameters& genParameters)
{
	if (generated) throw EXCEPTION_WORLD::ChunkRegeneration("Attempted to re-generate chunk");
	generated = true;

	const int _chunkBottom = position.y * CHUNK_SIZE;
	const int _chunkTop = _chunkBottom + CHUNK_SIZE - 1;

	// Return if all of the chunk falls above the terrain height
	if (genParameters.heightMap.heightMax + 1 < _chunkBottom && _chunkBottom > 0) return;

	// Fill the chunk if all of the chunk falls below the terrain height
	// This code is sort of horrible, but it runs hella fast compared to what was here before
	// Nvm this code is now even faster, and also looks okay
	if (_chunkTop < genParameters.heightMap.heightMin) blockContainer.blockArrayDelete(Block(2));
	else
	{
		// Some blocks must be placed beyond this point, so this optimisation is valid
		blockContainer.blockArrayCreate();

		for (int lX = 0; lX < CHUNK_SIZE; ++lX)
			for (int lZ = 0; lZ < CHUNK_SIZE; ++lZ)
			{
				int index = lZ * CHUNK_SIZE + lX;

				// Determine the default block to use based on the biome
				Block defaultBlock(2);
				switch (genParameters.biomeMap.biomeArray[index])
				{
				case BIOME::DESERT:
					defaultBlock = Block(5);
					break;
				case BIOME::DESERT_DEEP:
					defaultBlock = Block(17);
					break;
				case BIOME::FOREST_BOREAL:
					defaultBlock = Block(11);
					break;
				case BIOME::FOREST_TEMPERATE:
					defaultBlock = Block(1);
					break;
				case BIOME::RAINFOREST:
					defaultBlock = Block(8);
					break;
				case BIOME::SAVANNAH:
					defaultBlock = Block(16);
					break;
				case BIOME::SHRUBLAND:
					defaultBlock = Block(16);
					break;
				case BIOME::TUNDRA:
					defaultBlock = Block(7);
					break;
				default:
					break;
				}

				for (int lY = 0; lY < CHUNK_SIZE; ++lY)
				{
					ChunkLocalBlockPos blockPos(lX, lY, lZ);
					const auto _worldHeight = _chunkBottom + lY;
					const auto _surfaceHeight = genParameters.heightMap.heightArray[index];

					if (_worldHeight < SEA_LEVEL + 2)
					{
						if (_worldHeight <= _surfaceHeight)
						{
							if (_worldHeight < SEA_LEVEL - 2) setBlock(blockPos, Block(2));
							else setBlock(blockPos, Block(5));
						}
						else if (_worldHeight < SEA_LEVEL) setBlock(blockPos, Block(6));
					}
					else if (_worldHeight < _surfaceHeight) setBlock(blockPos, Block(2));
					else if (_worldHeight == _surfaceHeight) setBlock(blockPos, defaultBlock);
				}
			}
	}

	// Create population features
	// Return if chunk is entirely below surface or if all the surface air blocks are also below this chunk
	if (_chunkTop <= genParameters.heightMap.heightMin || genParameters.heightMap.heightMax + 1 < _chunkBottom) return;

	// LCG as the PRNG for population: this may or may not work out in the end
	// I have no idea tbh, I am not the stats person
	// Update: it somehow works
	ChunkPRNG prng(position);

	int _worldPosX = position.x * CHUNK_SIZE;
	int _worldPosZ = position.z * CHUNK_SIZE;

	// Ruin placement code. Ruins are a general class of structures that can be placed in funky ways
	// Try to place a ruin at a random point in the chunk
	{
		uint16_t _randpos = prng.raw();
		int pX = _randpos % CHUNK_SIZE;
		int pZ = (_randpos / CHUNK_SIZE) % CHUNK_SIZE;
		int _idx = pZ * CHUNK_SIZE + pX;
		int _ground = genParameters.heightMap.heightArray[_idx];

		switch (genParameters.biomeMap.biomeArray[_idx])
		{
		case BIOME::DESERT:
			if (_ground > -10 && prng.raw() > 65163) Structures::Ruins::clayFrame(
				*this, prng, BlockPos(_worldPosX + pX, _ground, _worldPosZ + pZ)
			);
		default:
			break;
		}
	}


	for (int lX = 0; lX < CHUNK_SIZE; ++lX)
		for (int lZ = 0; lZ < CHUNK_SIZE; ++lZ)
		{
			const int _index = lZ * CHUNK_SIZE + lX;
			const int _surfaceLevel = genParameters.heightMap.heightArray[_index];
			// Continue if surface air block is below chunk OR if the topmost block is above the chunk
			if (_surfaceLevel + 1 < _chunkBottom || _surfaceLevel + 1 > _chunkTop) continue;

			// Currently no features generate below sea level, so this is sort of a hack until those features exist
			if (_surfaceLevel < SEA_LEVEL + 3) continue;

			// Variables defined for quick access, this could absolutely by optimised
			// TODO: optimise this
			const BlockPos _centre(_worldPosX + lX, _surfaceLevel + 1, _worldPosZ + lZ);
			const uint16_t _foliageValue = prng.raw();

			switch (genParameters.biomeMap.biomeArray[_index])
			{
			case BIOME::DESERT:
				// Cactus
				if (_foliageValue > 65439)
				{
					auto height = prng.scaledInt(1.13, 2.87);
					for (int i = 0; i < height; ++i) setBlockPopulation(_centre.offset(0, i, 0), Block(9), 0);
				}
				// Desert Flower
				else if (_foliageValue > 65394) setBlockPopulation(_centre, Block(14), 0);
				else if (_foliageValue > 65391) setBlockPopulation(_centre, Block(10), 0);
				break;
			case BIOME::DESERT_DEEP:
				break;
			case BIOME::FOREST_BOREAL:
				if      (_foliageValue > 65358) Structures::Trees::PineBasic(*this, prng, _centre);
				else if (_foliageValue > 65325) Structures::Trees::PineMassive(*this, prng, _centre);
				else if (_foliageValue > 65161) Structures::Trees::PineFancy(*this, prng, _centre);
				break;
			case BIOME::FOREST_TEMPERATE:
				if      (_foliageValue > 64434) Structures::Trees::Oak(*this, prng, _centre);
				else if (_foliageValue > 64342) Structures::Trees::Aspen(*this, prng, _centre);
				else if (_foliageValue > 64093) setBlockPopulation(_centre, Block(15), 0);
				else if (_foliageValue > 63988) setBlockPopulation(_centre, Block(10), 0);
				break;
			case BIOME::RAINFOREST:
				// Rainforests are pretty bland like this ngl (slightly better now)
				if      (_foliageValue > 60272) Structures::Trees::RainforestBasic(*this, prng, _centre);
				else if (_foliageValue > 59989) Structures::Trees::RainforestTall(*this, prng, _centre);
				else if (_foliageValue > 49596) Structures::Trees::RainforestShrub(*this, _centre);
				else if (_foliageValue > 37063) setBlockPopulation(_centre, Block(10), 0);
				break;
			case BIOME::SAVANNAH:
				if      (_foliageValue > 65530) Structures::Trees::SavannahBaobab(*this, prng, _centre);
				else if (_foliageValue > 65423) Structures::Trees::SavannahAcacia(*this, prng, _centre);
				else if (_foliageValue > 52428) setBlockPopulation(_centre, Block(10), 0);
				break;
			case BIOME::SHRUBLAND:
				if      (_foliageValue > 52428) setBlockPopulation(_centre, Block(10), 0);
				else if (_foliageValue > 52369) setBlockPopulation(_centre, Block(4), 0);
				break;
			case BIOME::TUNDRA:
				if (_foliageValue > 65430) setBlockPopulation(_centre, Block(2), 0);
				break;
			default:
				break;
			}
		}
	populationChangesInside.shrink_to_fit();
	populationChangesAdjacent.shrink_to_fit();
}



void Chunk::PopulateChunk(World& world)
{
	const int CHUNK_NEIGHBOURS[26][3] = {
		{-1, -1, -1},
		{-1, -1,  0},
		{-1, -1,  1},
		{-1,  0, -1},
		{-1,  0,  0},
		{-1,  0,  1},
		{-1,  1, -1},
		{-1,  1,  0},
		{-1,  1,  1},
		{ 0, -1, -1},
		{ 0, -1,  0},
		{ 0, -1,  1},
		{ 0,  0, -1},
		{ 0,  0,  1},
		{ 0,  1, -1},
		{ 0,  1,  0},
		{ 0,  1,  1},
		{ 1, -1, -1},
		{ 1, -1,  0},
		{ 1, -1,  1},
		{ 1,  0, -1},
		{ 1,  0,  0},
		{ 1,  0,  1},
		{ 1,  1, -1},
		{ 1,  1,  0},
		{ 1,  1,  1}
	};

	// Sort out changes based on age
	std::unordered_map<BlockPos, std::pair<Block, unsigned>> _changes;
	for (auto& [_pos, _block, _age] : populationChangesInside)
		if (!_changes.contains(_pos) || _changes.at(_pos).second < _age) _changes[_pos] = { _block, _age };

	// Get changes from neighbours
	for (auto [lX, lY, lZ] : CHUNK_NEIGHBOURS)
		world.getChunk(ChunkPos(position.x + lX, position.y + lY, position.z + lZ))->
			addAdjacentPopulationChanges(_changes, position);

	for (auto& [_pos, _change] : _changes)
		if (_pos == position && (_change.second > 1024 || getBlock(_pos).blockType == 0))
			setBlock(ChunkLocalBlockPos(_pos), _change.first);

	populationChangesInside.clear();
}



Block Chunk::getBlock(ChunkLocalBlockPos blockPos) const
{
	return blockContainer.getBlock(blockPos);
}



std::vector<bool> Chunk::getSolidFaceMask(AxisDirection direction) const
{
	return blockContainer.getSolidFace(direction);
}



void Chunk::setBlock(ChunkLocalBlockPos blockPos, Block block)
{
	blockContainer.setBlock(blockPos, block);
}



bool Chunk::isEmpty() const
{
	return blockContainer.isEmpty();
}



void Chunk::setBlockPopulation(BlockPos blockPos, Block block, unsigned age)
{
	if (ChunkPos(blockPos) == position) populationChangesInside.push_back({ blockPos, block, age });
	else populationChangesAdjacent.push_back({ blockPos, block, age });
}



void Chunk::addAdjacentPopulationChanges(std::unordered_map<BlockPos, std::pair<Block, unsigned>>& changes, ChunkPos pos) const
{
	for (auto& _change : populationChangesAdjacent)
		if (_change.pos == pos && (!changes.contains(_change.pos) || changes.at(_change.pos).second < _change.age))
			changes[_change.pos] = { _change.block, _change.age };
}


