#include "BlockContainter.h"
#include <cassert>
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



BlockContainer::BlockContainer() :
	blockArrayType(BlockArrayType::NONE)
{
	blockArrayBlocksByIndex.push_back(Block(0));
	blockArrayIndicesByBlock[Block(0)] = 0;
	currentIndex = 1;
}



void BlockContainer::blockArrayCreate()
{
	assert(blockArrayType == BlockArrayType::NONE);
	blockArrayCompact = std::make_unique<uint8_t[]>(CHUNK_VOLUME);
	blockArrayType = BlockArrayType::COMPACT;
}



void BlockContainer::blockArrayDelete()
{
	blockArrayCompact.reset();
	blockArrayExtended.reset();
	blockArrayType = BlockArrayType::NONE;
}



void BlockContainer::blockArrayExtend()
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



Block BlockContainer::getBlock(ChunkLocalBlockPos blockPos) const
{
	if (isEmpty()) return blockArrayBlocksByIndex[0];
	int _index = flattenIndex(blockPos);
	int _blockIndex{};
	if (blockArrayType == BlockArrayType::COMPACT) _blockIndex = blockArrayCompact[_index];
	else _blockIndex = blockArrayExtended[_index];
	try
	{
		return blockArrayBlocksByIndex.at(_blockIndex);
	}
	catch (std::out_of_range)
	{
		throw EXCEPTION_WORLD::BlockIndexOutOfRange("Block index out of range");
	}
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
	if (blockArrayType == BlockArrayType::COMPACT) blockArrayCompact[arrayIndex] = static_cast<uint8_t>(blockIndex);
	else blockArrayExtended[arrayIndex] = static_cast<uint16_t>(blockIndex);
}



bool BlockContainer::isEmpty() const
{
	return (blockArrayType == BlockArrayType::NONE);
}
