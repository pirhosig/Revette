#include "Chunk.h"
#include <array>
#include <string>

#include "../Constants.h"
#include "../Exceptions.h"
#include "Generation/HeightMap.h"
#include "Generation/NoiseSource.h"


inline int checkAndFlattenIndex(const ChunkLocalBlockPos blockPos)
{
	if (!((blockPos.x >= 0) && (blockPos.x < CHUNK_SIZE) && (blockPos.y >= 0) && (blockPos.y < CHUNK_SIZE) && (blockPos.z >= 0) && (blockPos.z < CHUNK_SIZE)))
	{
		std::string errorMessage = "Invalid chunk block index at ";
		errorMessage += std::to_string(blockPos.x) + " " + std::to_string(blockPos.y) + " " + std::to_string(blockPos.z);
		throw EXCEPTION_WORLD::BlockIndexOutOfRange(errorMessage);
	}
	return blockPos.x * CHUNK_SIZE * CHUNK_SIZE + blockPos.y * CHUNK_SIZE + blockPos.z;
}



Chunk::Chunk(ChunkPos _pos) : position(_pos), generated(false)
{
	blockArrayBlocksByIndex.push_back(Block(0));
	blockArrayIndicesByBlock[Block(0)] = 0;
	currentIndex = 1;
}



void Chunk::GenerateChunk(const HeightMap& noiseHeightmap)
{
	if (generated) throw EXCEPTION_WORLD::ChunkRegeneration("Attempted to re-generate chunk");
	generated = true;

	// Return if all of the chunk falls above the terrain height
	if (noiseHeightmap.heightMax < position.y * CHUNK_SIZE) return;

	createBlockArray();

	// Fill the chunk if all of the chunk falls below the terrain height
	if (position.y * CHUNK_SIZE + CHUNK_SIZE - 1 <= noiseHeightmap.heightMin)
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
			for (int lY = 0; lY < CHUNK_SIZE; ++lY)
			{
				ChunkLocalBlockPos blockPos(lX, lY, lZ);
				int index = lZ * CHUNK_SIZE + lX;
				if (noiseHeightmap.heightArray[index] < (position.y * CHUNK_SIZE + lY)) blockArray[checkAndFlattenIndex(blockPos)] = 0;
				else setBlock(blockPos, Block(1));
			}
		}
	}
}



void Chunk::PopulateChunk(const HeightMap& noiseHeightmap, const NoiseSource2D& noiseFoliage)
{
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
			if (foliageValues[_index] > 0.973) setBlock(ChunkLocalBlockPos(lX, noiseHeightmap.heightArray[_index] + 1 - position.y * CHUNK_SIZE, lZ), Block(3));
		}
	}
}



Block Chunk::getBlock(ChunkLocalBlockPos blockPos) const
{
	if (!blockArray) return blockArrayBlocksByIndex[0];
	int index = checkAndFlattenIndex(blockPos);
	return blockArrayBlocksByIndex.at(blockArray[index]);
}



void Chunk::setBlock(ChunkLocalBlockPos blockPos, Block block)
{
	if (!blockArray) createBlockArray();
	int index = checkAndFlattenIndex(blockPos);
	auto it = blockArrayIndicesByBlock.find(block);
	if (it != blockArrayIndicesByBlock.end()) blockArray[index] = it->second;
	else
	{
		blockArrayBlocksByIndex.push_back(block);
		blockArrayIndicesByBlock[block] = currentIndex;
		blockArray[index] = currentIndex;
		currentIndex++;
	}
}



bool Chunk::isEmpty() const
{
	if (blockArray) return false;
	else return true;
}



void Chunk::createBlockArray()
{
	blockArray = std::make_unique<uint16_t[]>(CHUNK_VOLUME);
}



void Chunk::deleteBlockArray()
{
	blockArray.reset();
}


