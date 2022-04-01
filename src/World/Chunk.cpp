#include "Chunk.h"
#include <array>
#include <cassert>
#include <string>

#include "World.h"
#include "Generation/HeightMap.h"
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



void Chunk::GenerateChunk(const HeightMap& noiseHeightmap, const BiomeMap& noiseBiomeMap)
{
	if (generated) throw EXCEPTION_WORLD::ChunkRegeneration("Attempted to re-generate chunk");
	generated = true;

	const int _chunkBottom = position.y * CHUNK_SIZE;
	const int _chunkTop = _chunkBottom + CHUNK_SIZE - 1;

	// Return if all of the chunk falls above the terrain height
	if (noiseHeightmap.heightMax < _chunkBottom && _chunkBottom > 0) return;

	blockArrayCreate();

	// Fill the chunk if all of the chunk falls below the terrain height
	if (_chunkTop <= noiseHeightmap.heightMin)
	{
		for (int lX = 0; lX < CHUNK_SIZE; ++lX)
		{
			for (int lZ = 0; lZ < CHUNK_SIZE; ++lZ)
			{
				for (int lY = 0; lY < CHUNK_SIZE; ++lY)
				{
					setBlock(ChunkLocalBlockPos(lX, lY, lZ), Block(1));
				}
			}
		}
		return;
	}

	for (int lX = 0; lX < CHUNK_SIZE; ++lX)
	{
		for (int lZ = 0; lZ < CHUNK_SIZE; ++lZ)
		{
			for (int lY = 0; lY < CHUNK_SIZE; ++lY)
			{
				ChunkLocalBlockPos blockPos(lX, lY, lZ);
				Block defaultBlock(1);
				int index = lZ * CHUNK_SIZE + lX;
				if (noiseBiomeMap.biomeArray[index] > 0) defaultBlock.blockType = 5;
				if (noiseHeightmap.heightArray[index] < (position.y * CHUNK_SIZE + lY))
				{
					if (lY + _chunkBottom > 0) setBlock(blockPos, Block(0));
					else setBlock(blockPos, Block(6));
				}
				else setBlock(blockPos, defaultBlock);
			}
		}
	}
}



void Chunk::PopulateChunk(const HeightMap& noiseHeightmap, const BiomeMap& noiseBiomeMap, const NoiseSource2D& noiseFoliage, World& world)
{
	return;
	const int _chunkHeightMin = position.y * CHUNK_SIZE;
	const int _chunkHeightMax = _chunkHeightMin + CHUNK_SIZE - 1;

	// Return if chunk is entirely below surface
	if (_chunkHeightMax <= noiseHeightmap.heightMin) return;

	// Return if all the surface air blocks are also below this chunk
	if (noiseHeightmap.heightMax + 1 < _chunkHeightMin) return;

	std::array<float, CHUNK_AREA> foliageValues = noiseFoliage.GenChunkNoise(ChunkPos2D(position));

	for (int lX = 0; lX < CHUNK_SIZE; ++lX)
	{
		for (int lZ = 0; lZ < CHUNK_SIZE; ++lZ)
		{
			// Continue if surface air block is below chunk
			int _index = lZ * CHUNK_SIZE + lX;
			if (noiseHeightmap.heightArray[_index] + 1 < _chunkHeightMin) continue;

			// Continue if the topmost block is above the chunk
			if (noiseHeightmap.heightArray[_index] + 1 > _chunkHeightMax) continue;

			// Check if the topmost block needs a tree
			if (noiseBiomeMap.biomeArray[_index] == 0 && foliageValues[_index] > 0.984)
			{
				for (int lH = 0; lH < 5; ++lH)
				{
					int currentHeight = noiseHeightmap.heightArray[_index] + 1 - position.y * CHUNK_SIZE + lH;
					if (blockPositionIsInside(lX, currentHeight, lZ)) setBlock(ChunkLocalBlockPos(lX, currentHeight, lZ), Block(3));
					else world.setBlock(BlockPos(lX + position.x * CHUNK_SIZE, noiseHeightmap.heightArray[_index] + 1 + lH, lZ + position.z * CHUNK_SIZE), Block(3));
				}
				world.setBlock(BlockPos(lX + position.x * CHUNK_SIZE, noiseHeightmap.heightArray[_index] + 6, lZ + position.z * CHUNK_SIZE), Block(4));
				world.setBlock(BlockPos(lX + position.x * CHUNK_SIZE, noiseHeightmap.heightArray[_index] + 7, lZ + position.z * CHUNK_SIZE), Block(4));
				world.setBlock(BlockPos(lX + position.x * CHUNK_SIZE + 1, noiseHeightmap.heightArray[_index] + 6, lZ + position.z * CHUNK_SIZE), Block(4));
				world.setBlock(BlockPos(lX + position.x * CHUNK_SIZE - 1, noiseHeightmap.heightArray[_index] + 6, lZ + position.z * CHUNK_SIZE), Block(4));
				world.setBlock(BlockPos(lX + position.x * CHUNK_SIZE, noiseHeightmap.heightArray[_index] + 6, lZ + position.z * CHUNK_SIZE + 1), Block(4));
				world.setBlock(BlockPos(lX + position.x * CHUNK_SIZE, noiseHeightmap.heightArray[_index] + 6, lZ + position.z * CHUNK_SIZE - 1), Block(4));
			}
		}
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
	assert(blockArrayType != BlockArrayType::NONE);
	if (blockArrayType == BlockArrayType::COMPACT) blockArrayCompact[arrayIndex] = blockIndex;
	else blockArrayExtended[arrayIndex] = blockIndex;
}


