#include "BlockContainter.h"
#include <cassert>
#include "../Exceptions.h"



inline int blockPositionIsInside(int x, int y, int z)
{
	return (x >= 0) && (x < CHUNK_SIZE) && (y >= 0) && (y < CHUNK_SIZE) && (z >= 0) && (z < CHUNK_SIZE);
}



inline int flattenIndex(const ChunkLocalBlockPos blockPos)
{
	// assert(blockPositionIsInside(blockPos.x, blockPos.y, blockPos.z) && "Block outside chunk.");
	return blockPos.x * CHUNK_SIZE * CHUNK_SIZE + blockPos.y * CHUNK_SIZE + blockPos.z;
}



BlockContainer::BlockContainer()
{
	blockArrayBlocksByIndex.push_back(Block(0));
	blockArrayIndicesByBlock[Block(0)] = 0;
	currentIndex = 1;
}



void BlockContainer::blockArrayCreate()
{
	assert(std::holds_alternative<std::monostate>(blockArray));
	blockArray = std::make_unique<uint8_t[]>(CHUNK_VOLUME);
}



void BlockContainer::blockArrayDelete()
{
	blockArray = std::monostate();
}



void BlockContainer::blockArrayExtend()
{
	assert(std::holds_alternative<std::unique_ptr<uint8_t[]>>(blockArray));
	std::unique_ptr<uint16_t[]> replaceArray = std::make_unique<uint16_t[]>(CHUNK_VOLUME);
	for (int i = 0; i < CHUNK_VOLUME; ++i)
	{
		replaceArray[i] = std::get<std::unique_ptr<uint8_t[]>>(blockArray)[i];
	}
	blockArray = std::move(replaceArray);
}



Block BlockContainer::getBlock(ChunkLocalBlockPos blockPos) const try
{
	int _blockIndex{};
	int _index = flattenIndex(blockPos);
	switch (blockArray.index())
	{
	case 1:
		_blockIndex = std::get<std::unique_ptr<uint8_t[]>>(blockArray)[_index];
		break;
	case 2:
		_blockIndex = std::get<std::unique_ptr<uint16_t[]>>(blockArray)[_index];
		break;
	default:
		return Block(0);
		break;
	}
	return blockArrayBlocksByIndex.at(_blockIndex);
}
catch (std::out_of_range)
{
	throw EXCEPTION_WORLD::BlockIndexOutOfRange("Block index out of range");
}
catch (...)
{
	throw;
}



void BlockContainer::setBlock(ChunkLocalBlockPos blockPos, Block block)
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



// Directly sets the value in the block array, without any safety checks
void BlockContainer::setBlockRaw(int arrayIndex, int blockIndex)
{
	if (std::holds_alternative<std::unique_ptr<uint8_t[]>>(blockArray))
		std::get<std::unique_ptr<uint8_t[]>>(blockArray)[arrayIndex] = static_cast<uint8_t>(blockIndex);
	else std::get<std::unique_ptr<uint16_t[]>>(blockArray)[arrayIndex] = static_cast<uint16_t>(blockIndex);
}



bool BlockContainer::isEmpty() const
{
	return std::holds_alternative<std::monostate>(blockArray);
}
