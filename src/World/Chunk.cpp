#include "Chunk.h"
#include <array>
#include <cassert>
#include <string>

#include "World.h"
#include "Generation/ChunkPRNG.h"
#include "Generation/GeneratorChunkParameters.h"
#include "Generation/Structures/StructurePlants.h"
#include "../Constants.h"
#include "../Exceptions.h"
#include "../Math/ProbabilityTable.h"



inline int blockPositionIsInside(int x, int y, int z)
{
	return (x >= 0) && (x < CHUNK_SIZE) && (y >= 0) && (y < CHUNK_SIZE) && (z >= 0) && (z < CHUNK_SIZE);
}



Chunk::Chunk(ChunkPos _pos) : position(_pos), generated(false) {}



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
					defaultBlock = Block(5);
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

					if (_surfaceHeight < _worldHeight) {
						if (_worldHeight <= 0) setBlock(blockPos, Block(6));
					}
					else setBlock(blockPos, defaultBlock);
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

	for (int lX = 0; lX < CHUNK_SIZE; ++lX)
		for (int lZ = 0; lZ < CHUNK_SIZE; ++lZ)
		{
			const int _index = lZ * CHUNK_SIZE + lX;
			const int _surfaceLevel = genParameters.heightMap.heightArray[_index];
			// Continue if surface air block is below chunk OR if the topmost block is above the chunk
			if (_surfaceLevel + 1 < _chunkBottom || _surfaceLevel + 1 > _chunkTop) continue;

			// Currently no features generate below sea level, so this is sort of a hack until those features exist
			if (_surfaceLevel < 0) continue;

			// Variables defined for quick access, this could absolutely by optimised
			// TODO: optimise this
			const BlockPos _centre(position.x * CHUNK_SIZE + lX, _surfaceLevel + 1, position.z * CHUNK_SIZE + lZ);
			const double _foliageValue = prng.next();

			switch (genParameters.biomeMap.biomeArray[_index])
			{
			case BIOME::DESERT:
				// Cactus
				if (_foliageValue > 0.9983)
				{
					auto height = prng.scaledInt(1.13, 2.87);
					for (int i = 0; i < height; ++i) setBlockPopulation(_centre.offset(0, i, 0), Block(9), 0);
				}
				// Desert Flower
				else if (_foliageValue > 0.9974) setBlockPopulation(_centre, Block(14), 0);
				else if (_foliageValue > 0.9973) setBlockPopulation(_centre, Block(10), 0);
				break;
			case BIOME::DESERT_DEEP:
				break;
			case BIOME::FOREST_BOREAL:
				// Basic pine tree
				if (_foliageValue > 0.9910) Structures::placeTreePine(*this, prng, _centre);
				else if (_foliageValue > 0.9893) Structures::placeTreePineMassive(*this, prng, _centre);
				break;
			case BIOME::FOREST_TEMPERATE:
				// Basic bitch tree
				if (_foliageValue > 0.9790) Structures::placeTreeOak(*this, prng, _centre);
				else if (_foliageValue > 0.9775) Structures::placeTreeAspen(*this, prng, _centre);
				else if (_foliageValue > 0.9740) setBlockPopulation(_centre, Block(15), 0);
				break;
			case BIOME::RAINFOREST:
				// Rainforests are pretty bland like this ngl (slightly better now)
				if (_foliageValue > 0.92) Structures::placeTreeRainforestBasic(*this, prng, _centre);
				else if (_foliageValue > 0.914) Structures::placeTreeRainforestTall(*this, prng, _centre);
				else if (_foliageValue > 0.74) Structures::placeTreeRainforestShrub(*this, _centre);
				else if (_foliageValue > 0.55) setBlockPopulation(_centre, Block(10), 0);
				break;
			case BIOME::SAVANNAH:
				if (_foliageValue > 0.9999) Structures::placeTreeSavannahBaobab(*this, prng, _centre);
				else if (_foliageValue > 0.9983) Structures::placeTreeSavannahAcacia(*this, prng, _centre);
				else if (_foliageValue > 0.8000) setBlockPopulation(_centre, Block(10), 0);
				break;
			case BIOME::SHRUBLAND:
				if (_foliageValue > 0.8000) setBlockPopulation(_centre, Block(10), 0);
				else if (_foliageValue > 0.7991) setBlockPopulation(_centre, Block(4), 0);
				break;
			case BIOME::TUNDRA:
				if (_foliageValue > 0.9984) setBlockPopulation(_centre, Block(2), 0);
				break;
			default:
				break;
			}
		}
}



void Chunk::PopulateChunk(World& world)
{
	for (int lX = -1; lX < 2; ++lX)
		for (int lY = -1; lY < 2; ++lY)
			for (int lZ = -1; lZ < 2; ++lZ)
			{
				world.getChunk(ChunkPos(position.x + lX, position.y + lY, position.z + lZ))->
					addAdjacentPopulationChanges(*this);
			}

	for (auto& [_pos, _change] : populationChanges)
		if (ChunkPos(_pos) == position && getBlock(_pos).blockType == 0) setBlock(ChunkLocalBlockPos(_pos), _change.block);
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



void Chunk::setBlockPopulation(BlockPos blockPos, Block block, unsigned long long age)
{
	if ((!populationChanges.contains(blockPos)) || (populationChanges.at(blockPos).age < age))
		populationChanges[blockPos] = {block, age};
}



void Chunk::addAdjacentPopulationChanges(Chunk& _chunk) const
{
	for (auto& [_pos, _change] : populationChanges)
		if (ChunkPos(_pos) == _chunk.position) _chunk.setBlockPopulation(_pos, _change.block, _change.age);
}


