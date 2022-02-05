#include "Chunk.h"
#include <array>
#include <string>

#include "../Constants.h"
#include "../Exceptions.h"



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



void Chunk::GenerateChunk(FastNoise::SmartNode<>& noiseHeightmap)
{
	if (generated) throw EXCEPTION_WORLD::ChunkRegeneration("Attempted to re-generate chunk");
	generated = true;
	createBlockArray();

	std::array<int, CHUNK_AREA> heightMap{};
	{
		std::array<float, CHUNK_AREA> noiseMap{};
		int cPosX = position.x * CHUNK_SIZE;
		int cPosZ = position.z * CHUNK_SIZE;
		noiseHeightmap->GenUniformGrid2D(noiseMap.data(), cPosX, cPosZ, CHUNK_SIZE, CHUNK_SIZE, 0.00625, 42);
		for (int i = 0; i < CHUNK_AREA; ++i)
		{
			heightMap[i] = static_cast<int>(noiseMap[i] * 5.0);
		}
	}

	for (int lX = 0; lX < CHUNK_SIZE; ++lX)
	{
		for (int lZ = 0; lZ < CHUNK_SIZE; ++lZ)
		{
			for (int lY = 0; lY < CHUNK_SIZE; ++lY)
			{
				ChunkLocalBlockPos blockPos(lX, lY, lZ);
				int index = lX * CHUNK_SIZE + lZ;
				if (heightMap[index] < (position.y * CHUNK_SIZE + lY)) blockArray[checkAndFlattenIndex(blockPos)] = 0;
				else setBlock(blockPos, Block(1));
			}
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


