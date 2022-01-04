#include "Chunk.h"
#include "../Exceptions.h"



inline int checkAndFlattenIndex(const int x, const int y, const int z)
{
	if (!((x > 0) && (x < CHUNK_SIZE) && (y > 0) && (y < CHUNK_SIZE) && (z > 0) && (z < CHUNK_SIZE)))
	{
		throw EXCEPTION_WORLD::BlockIndexOutOfRange("Invalid chunk index");
	}
	return x * CHUNK_SIZE * CHUNK_SIZE + y * CHUNK_SIZE + z;
}



Chunk::Chunk(int x, int y, int z) : position(x, y, z)
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



Block Chunk::getBlock(int x, int y, int z) const
{
	if (!blockArray) return blockArrayBlocksByIndex[0];
	int index = checkAndFlattenIndex(x, y, z);
	return blockArrayBlocksByIndex.at(blockArray[index]);
}



void Chunk::setBlock(int x, int y, int z, Block block)
{
	if (!blockArray) createBlockArray();
	int index = checkAndFlattenIndex(x, y, z);
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



void Chunk::createBlockArray()
{
	blockArray = std::make_unique<uint16_t[]>(CHUNK_VOLUME);
}


