#include "Chunk.h"
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



Chunk::Chunk(ChunkPos _pos) : position(_pos)
{
	blockArrayBlocksByIndex.push_back(Block(0));
	blockArrayIndicesByBlock[Block(0)] = 0;
	currentIndex = 1;

	if (position.y < 0)
	{
		blockArrayBlocksByIndex.push_back(Block(1));
		blockArrayIndicesByBlock[Block(1)] = 1;
		currentIndex++;

		createBlockArray();
		for (int i = 0; i < CHUNK_VOLUME; ++i)
		{
			blockArray[i] = 1;
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


