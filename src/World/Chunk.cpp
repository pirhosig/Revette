#include "Chunk.h"
#include <array>
#include <cassert>
#include <string>

#include "World.h"
#include "Generation/GeneratorChunkParameters.h"
#include "Generation/NoiseSource.h"
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



Chunk::Chunk(ChunkPos _pos) : position(_pos), generated(false), blockArrayType(BlockArrayType::NONE)
{
	blockArrayBlocksByIndex.push_back(Block(0));
	blockArrayIndicesByBlock[Block(0)] = 0;
	currentIndex = 1;
}



void Chunk::GenerateChunk(const GeneratorChunkParameters& generatorParameters, World& world)
{
	if (generated) throw EXCEPTION_WORLD::ChunkRegeneration("Attempted to re-generate chunk");
	generated = true;

	const int _chunkBottom = position.y * CHUNK_SIZE;
	const int _chunkTop = _chunkBottom + CHUNK_SIZE - 1;

	// Return if all of the chunk falls above the terrain height
	if (generatorParameters.heightMap.heightMax < _chunkBottom && _chunkBottom > 0) return;

	blockArrayCreate();

	// Fill the chunk if all of the chunk falls below the terrain height
	if (_chunkTop <= generatorParameters.heightMap.heightMin)
	{
		for (int lX = 0; lX < CHUNK_SIZE; ++lX)
		{
			for (int lZ = 0; lZ < CHUNK_SIZE; ++lZ)
			{
				for (int lY = 0; lY < CHUNK_SIZE; ++lY)
				{
					setBlock(ChunkLocalBlockPos(lX, lY, lZ), Block(2));
				}
			}
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
			case BIOME::FOREST:
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

				if (generatorParameters.heightMap.heightArray[index] < (position.y * CHUNK_SIZE + lY))
				{
					if (lY + _chunkBottom > 0) setBlock(blockPos, Block(0));
					else setBlock(blockPos, Block(6));
				}
				else setBlock(blockPos, defaultBlock);
			}
		}
	}
}



void Chunk::PopulateChunk(const GeneratorChunkParameters& generatorParameters, const NoiseSource2D& noiseFoliage, World& world)
{
	const int _chunkHeightMin = position.y * CHUNK_SIZE;
	const int _chunkHeightMax = _chunkHeightMin + CHUNK_SIZE - 1;

	// Return if chunk is entirely below surface or if all the surface air blocks are also below this chunk
	if (_chunkHeightMax <= generatorParameters.heightMap.heightMin || generatorParameters.heightMap.heightMax + 1 < _chunkHeightMin) return;

	std::array<float, CHUNK_AREA> foliageValues = noiseFoliage.GenChunkNoise(ChunkPos2D(position));

	for (int lX = 0; lX < CHUNK_SIZE; ++lX)
	{
		for (int lZ = 0; lZ < CHUNK_SIZE; ++lZ)
		{
			const int _index = lZ * CHUNK_SIZE + lX;
			const int _surfaceLevel = generatorParameters.heightMap.heightArray[_index];
			// Continue if surface air block is below chunk OR if the topmost block is above the chunk
			if (_surfaceLevel + 1 < _chunkHeightMin || _surfaceLevel + 1 > _chunkHeightMax) continue;

			const int _worldX = position.x * CHUNK_SIZE + lX;
			const int _worldZ = position.z * CHUNK_SIZE + lZ;
			const int _bottomAir = _surfaceLevel + 1;
			const bool _aboveSeaLevel = (_surfaceLevel >= 0);
			const auto _foliageValue = foliageValues[_index];

			switch (generatorParameters.biomeMap.biomeArray[_index])
			{
			case BIOME::DESERT:
				// Cactus
				if (_aboveSeaLevel && _foliageValue > 0.9952)
				{
					setBlockPopulation(BlockPos(_worldX, _bottomAir, _worldZ), Block(9), 0);
					setBlockPopulation(BlockPos(_worldX, _bottomAir + 1, _worldZ), Block(9), 0);
				}
				break;
			case BIOME::FOREST:
				// Basic bitch tree
				if (_aboveSeaLevel && _foliageValue > 0.982)
				{
					int _treeHeight = 6;
					unsigned long long _treeAge = 0;

					// Build tree trunk
					for (int i = 0; i < _treeHeight - 2; ++i)
					{
						setBlockPopulation(BlockPos(_worldX, _bottomAir + i, _worldZ), Block(3), _treeAge);
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
				break;
			case BIOME::RAINFOREST:
				// Rainforests are pretty bland like this ngl
				if (!_aboveSeaLevel) break;
				if (_foliageValue > 0.86)
				{
					int _treeHeight = 8;
					unsigned long long _treeAge = 0;

					// Build tree trunk
					for (int i = 0; i < _treeHeight - 2; ++i)
					{
						setBlockPopulation(BlockPos(_worldX, _bottomAir + i, _worldZ), Block(3), _treeAge);
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
				else if (_foliageValue > 0.65)
				{
					setBlockPopulation(BlockPos(_worldX, _bottomAir, _worldZ), Block(4), 0);
				}
				else if (_foliageValue > 0.50)
				{
					setBlockPopulation(BlockPos(_worldX, _bottomAir, _worldZ), Block(10), 0);
				}
				break;
			case BIOME::SHRUBLAND:
				if (!_aboveSeaLevel) break;
				if (_foliageValue > 0.80)
				{
					setBlockPopulation(BlockPos(_worldX, _bottomAir, _worldZ), Block(10), 0);
				}
				break;
			case BIOME::TUNDRA:
				break;
			default:
				break;
			}
		}
	}

	for (auto& change : populationChanges)
	{
		world.setBlock(change.first, change.second.block);
	}
}



Block Chunk::getBlock(ChunkLocalBlockPos blockPos) const
{
	if (isEmpty()) return blockArrayBlocksByIndex[0];
	int _index = flattenIndex(blockPos);
	int _blockIndex{};
	if (blockArrayType == BlockArrayType::COMPACT) _blockIndex = blockArrayCompact[_index];
	else _blockIndex = blockArrayExtended[_index];
	return blockArrayBlocksByIndex.at(_blockIndex);
}



void Chunk::setBlock(ChunkLocalBlockPos blockPos, Block block)
{
	if (isEmpty()) blockArrayCreate();
	auto it = blockArrayIndicesByBlock.find(block);
	int _blockIndex;
	if (it != blockArrayIndicesByBlock.end()) _blockIndex = it->second;
	else
	{
		blockArrayBlocksByIndex.push_back(block);
		blockArrayIndicesByBlock[block] = currentIndex;
		_blockIndex = currentIndex;
		if (currentIndex > 255) blockArrayExtend();
		currentIndex++;
	}
	setBlockRaw(flattenIndex(blockPos), _blockIndex);
}



bool Chunk::isEmpty() const
{
	return (blockArrayType == BlockArrayType::NONE);
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
	if (containsPosition(blockPos)) setBlock(ChunkLocalBlockPos(blockPos), block);
	else if ((!populationChanges.contains(blockPos)) || (populationChanges.at(blockPos).age < age)) populationChanges[blockPos] = {block, age};
}



void Chunk::blockArrayCreate()
{
	assert(blockArrayType == BlockArrayType::NONE);
	blockArrayCompact = std::make_unique<uint8_t[]>(CHUNK_VOLUME);
	blockArrayType = BlockArrayType::COMPACT;
}



void Chunk::blockArrayDelete()
{
	blockArrayCompact.reset();
	blockArrayExtended.reset();
	blockArrayType = BlockArrayType::NONE;
}



void Chunk::blockArrayExtend()
{
	assert(blockArrayType == BlockArrayType::COMPACT);
	blockArrayExtended = std::make_unique<uint16_t[]>(CHUNK_VOLUME);
	for (int i = 0; i < CHUNK_VOLUME; ++i)
	{
		blockArrayExtended[i] = blockArrayCompact[i];
	}
	blockArrayCompact.reset();
	blockArrayType = BlockArrayType::EXTENDED;
}



// Directly sets the value in the block array
void Chunk::setBlockRaw(int arrayIndex, int blockIndex)
{
	if (blockArrayType == BlockArrayType::COMPACT) blockArrayCompact[arrayIndex] = blockIndex;
	else blockArrayExtended[arrayIndex] = blockIndex;
}


