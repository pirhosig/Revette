#include "Chunk.h"
#include <array>
#include <cassert>
#include <string>

#include "World.h"
#include "Generation/GeneratorChunkParameters.h"
#include "Generation/NoiseSource.h"
#include "Generation/Structures/StructurePlants.h"
#include "../Constants.h"
#include "../Exceptions.h"



inline int blockPositionIsInside(int x, int y, int z)
{
	return (x >= 0) && (x < CHUNK_SIZE) && (y >= 0) && (y < CHUNK_SIZE) && (z >= 0) && (z < CHUNK_SIZE);
}



inline int flattenIndex(const ChunkLocalBlockPos blockPos)
{
	assert(blockPositionIsInside(blockPos.x, blockPos.y, blockPos.z) && "Block outside chunk.");
	return blockPos.x * CHUNK_SIZE * CHUNK_SIZE + blockPos.y * CHUNK_SIZE + blockPos.z;
}



Chunk::Chunk(ChunkPos _pos) : position(_pos), generated(false) {}



void Chunk::GenerateChunk(
	const GeneratorChunkParameters& generatorParameters,
	const NoiseSource2D& noiseFoliage,
	const NoiseSource2D& noiseFoliageSecondary
)
{
	if (generated) throw EXCEPTION_WORLD::ChunkRegeneration("Attempted to re-generate chunk");
	generated = true;

	const int _chunkBottom = position.y * CHUNK_SIZE;
	const int _chunkTop = _chunkBottom + CHUNK_SIZE - 1;

	// Return if all of the chunk falls above the terrain height
	if (generatorParameters.heightMap.heightMax < _chunkBottom && _chunkBottom > 0) return;

	blockContainer.blockArrayCreate();

	// Fill the chunk if all of the chunk falls below the terrain height
	// This code is sort of horrible, but it runs hella fast compared to what was here before
	if (_chunkTop < generatorParameters.heightMap.heightMin)
	{
		int _rawIndex = blockContainer.addBlockToPallete(Block(2));
		for (int i = 0; i < CHUNK_VOLUME; ++i)
		{
			blockContainer.setBlockRaw(i, _rawIndex);
		}
		return;
	}

	for (int lX = 0; lX < CHUNK_SIZE; ++lX)
	{
		for (int lZ = 0; lZ < CHUNK_SIZE; ++lZ)
		{
			int index = lZ * CHUNK_SIZE + lX;

			// Determine the default block to use based on the biome
			Block defaultBlock(2);
			switch (generatorParameters.biomeMap.biomeArray[index])
			{
			case BIOME::DESERT:
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
			case BIOME::SHRUBLAND:
				defaultBlock = Block(1);
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
				const auto _surfaceHeight = generatorParameters.heightMap.heightArray[index];

				if (_surfaceHeight < _worldHeight) {
					if (_worldHeight <= 0) setBlock(blockPos, Block(6));
				}
				else setBlock(blockPos, defaultBlock);
			}
		}
	}


	// Create population features
	// Return if chunk is entirely below surface or if all the surface air blocks are also below this chunk
	if (_chunkTop <= generatorParameters.heightMap.heightMin || generatorParameters.heightMap.heightMax + 1 < _chunkBottom) return;

	std::array<float, CHUNK_AREA> foliageValues = noiseFoliage.GenChunkNoise(ChunkPos2D(position));
	std::array<float, CHUNK_AREA> foliageValuesSecondary = noiseFoliageSecondary.GenChunkNoise(ChunkPos2D(position));

	for (int lX = 0; lX < CHUNK_SIZE; ++lX)
	{
		for (int lZ = 0; lZ < CHUNK_SIZE; ++lZ)
		{
			const int _index = lZ * CHUNK_SIZE + lX;
			const int _surfaceLevel = generatorParameters.heightMap.heightArray[_index];
			// Continue if surface air block is below chunk OR if the topmost block is above the chunk
			if (_surfaceLevel + 1 < _chunkBottom || _surfaceLevel + 1 > _chunkTop) continue;

			// Currently no features generate below sea level, so this is sort of a hack until those features exist
			if (_surfaceLevel < 0) continue;

			const int _worldX = position.x * CHUNK_SIZE + lX;
			const int _worldZ = position.z * CHUNK_SIZE + lZ;
			const int _bottomAir = _surfaceLevel + 1;
			const BlockPos _centre(_worldX, _bottomAir, _worldZ);
			const auto _foliageValue = foliageValues[_index];
			const auto _foliageValueSecondary = foliageValuesSecondary[_index];

			switch (generatorParameters.biomeMap.biomeArray[_index])
			{
			case BIOME::DESERT:
				// Cactus
				if (_foliageValue > 0.9956)
				{
					int _cactusHeight = static_cast<int>(2.67 + _foliageValueSecondary);
					for (int i = 0; i < _cactusHeight; ++i) {
						setBlockPopulation(BlockPos(_worldX, _bottomAir + i, _worldZ), Block(9), 0);
					}
				}
				// Desert Flower
				else if (_foliageValue > 0.9942) setBlockPopulation(_centre, Block(14), 0);
				break;
			case BIOME::FOREST_BOREAL:
				// Basic pine tree
				if (_foliageValue > 0.9910)
					Structures::placeTreePine(*this, _centre, 0, static_cast<int>(6.6 + _foliageValueSecondary * 2.4));
				else if (_foliageValue > 0.9893)
					Structures::placeTreePineMassive(*this, _centre, 0, static_cast<int>(15.7 + _foliageValueSecondary * 4.7));
				break;
			case BIOME::FOREST_TEMPERATE:
				// Basic bitch tree
				if (_foliageValue > 0.9840)
				{
					int _treeHeight = static_cast<int>(10.1 + _foliageValueSecondary * 5.3);
					unsigned long long _treeAge = 0;
					// Build tree trunk
					for (int i = 0; i < _treeHeight - 2; ++i) {
						setBlockPopulation(BlockPos(_worldX, _bottomAir + i, _worldZ), Block(3), _treeAge + 1);
					}
					const int leafBase = _bottomAir + _treeHeight - 2;
					// Add the leaves
					setBlockPopulation(BlockPos(_worldX, leafBase, _worldZ), Block(4), _treeAge);
					setBlockPopulation(BlockPos(_worldX, leafBase + 1, _worldZ), Block(4), _treeAge);
					setBlockPopulation(BlockPos(_worldX - 1, leafBase, _worldZ), Block(4), _treeAge);
					setBlockPopulation(BlockPos(_worldX + 1, leafBase, _worldZ), Block(4), _treeAge);
					setBlockPopulation(BlockPos(_worldX, leafBase, _worldZ - 1), Block(4), _treeAge);
					setBlockPopulation(BlockPos(_worldX, leafBase, _worldZ + 1), Block(4), _treeAge);
				}
				else if (_foliageValue > 0.9815)
					Structures::placeTreeAspen(*this, _centre, 0, static_cast<int>(12.7 + _foliageValueSecondary * 6.2));
				break;
			case BIOME::RAINFOREST:
				// Rainforests are pretty bland like this ngl (slightly better now)
				if (_foliageValue > 0.92)
				{
					int _treeHeight = static_cast<int>(9.9 + _foliageValueSecondary * 3.2);
					unsigned long long _treeAge = 0;
					// Build tree trunk
					for (int i = 0; i < _treeHeight - 2; ++i) {
						setBlockPopulation(BlockPos(_worldX, _bottomAir + i, _worldZ), Block(3), _treeAge + 1);
					}
					const int leafBase = _bottomAir + _treeHeight - 2;
					// Add the leaves
					for (int i = -1; i < 2; ++i) {
						for (int j = -1; j < 2; ++j) {
							setBlockPopulation(BlockPos(_worldX + i, leafBase, _worldZ + j), Block(4), _treeAge);
						}
					}
					setBlockPopulation(BlockPos(_worldX, leafBase + 1, _worldZ), Block(4), _treeAge);
					setBlockPopulation(BlockPos(_worldX - 2, leafBase, _worldZ), Block(4), _treeAge);
					setBlockPopulation(BlockPos(_worldX + 2, leafBase, _worldZ), Block(4), _treeAge);
					setBlockPopulation(BlockPos(_worldX, leafBase, _worldZ - 2), Block(4), _treeAge);
					setBlockPopulation(BlockPos(_worldX, leafBase, _worldZ + 2), Block(4), _treeAge);
				}
				else if (_foliageValue > 0.914)
					Structures::placeTreeRainforestTall(*this, _centre, 0, static_cast<int>(15.7 + _foliageValueSecondary * 4.3));
				else if (_foliageValue > 0.74) Structures::placeTreeRainforestShrub((*this), _centre, 0);
				else if (_foliageValue > 0.55) setBlockPopulation(_centre, Block(10), 0);
				break;
			case BIOME::SHRUBLAND:
				if (_foliageValue > 0.80) setBlockPopulation(_centre, Block(10), 0);
				else if (_foliageValue > 0.7991) setBlockPopulation(_centre, Block(4), 0);
				break;
			case BIOME::TUNDRA:
				if (_foliageValue > 0.9976) setBlockPopulation(_centre, Block(2), 0);
				break;
			default:
				break;
			}
		}
	}
}



void Chunk::PopulateChunk(World& world)
{
	for (int lX = -1; lX < 2; ++lX)
	{
		for (int lY = -1; lY < 2; ++lY)
		{
			for (int lZ = -1; lZ < 2; ++lZ)
			{
				auto& _chunk = world.getChunk(ChunkPos(position.x + lX, position.y + lY, position.z + lZ));
				_chunk->addAdjacentPopulationChanges(*this);
			}
		}
	}

	for (auto& change : populationChanges)
	{
		auto& _pos = change.first;
		if (ChunkPos(_pos) == position) setBlock(ChunkLocalBlockPos(_pos), change.second.block);
	}
}



Block Chunk::getBlock(ChunkLocalBlockPos blockPos) const
{
	return blockContainer.getBlock(blockPos);
}



void Chunk::setBlock(ChunkLocalBlockPos blockPos, Block block)
{
	blockContainer.setBlock(blockPos, block);
}



bool Chunk::isEmpty() const
{
	return blockContainer.isEmpty();
}



bool Chunk::isGenerated() const
{
	return generated;
}



bool Chunk::containsPosition(BlockPos blockPos) const
{
	return ChunkPos(blockPos) == position;
}



void Chunk::setBlockPopulation(BlockPos blockPos, Block block, unsigned long long age)
{
	if ((!populationChanges.contains(blockPos)) || (populationChanges.at(blockPos).age < age)) populationChanges[blockPos] = {block, age};
}



void Chunk::addAdjacentPopulationChanges(Chunk& _chunk) const
{
	for (auto& change : populationChanges)
	{
		auto& _pos = change.first;
		if (ChunkPos(_pos) == _chunk.position) _chunk.setBlockPopulation(_pos, change.second.block, change.second.age);
	}
}


