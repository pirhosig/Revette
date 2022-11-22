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
	return blockPos.x * CHUNK_AREA + blockPos.y * CHUNK_SIZE + blockPos.z;
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
	case 0:
		return Block(0);
		break;
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



std::vector<bool> BlockContainer::getSolid() const
{
	const bool IS_SOLID[] = {
	false,
	true,
	true,
	true,
	false,
	true,
	true,
	true,
	true,
	true,
	false,
	true,
	false,
	true,
	false,
	true,
	true,
	true,
	true,
	true,
	true,
	true,
	true,
	true,
	true,
	true
	};

	std::vector<bool> _solid(CHUNK_VOLUME);
	std::vector<bool> _indexTransparency(blockArrayBlocksByIndex.size());
	for (std::size_t i = 0; i < blockArrayBlocksByIndex.size(); ++i)
		_indexTransparency[i] = IS_SOLID[blockArrayBlocksByIndex[i].blockType];

	switch (blockArray.index())
	{
	case 0:
		break;
	case 1:
		for (int i = 0; i < CHUNK_VOLUME; ++i)
			_solid[i] = _indexTransparency[std::get<std::unique_ptr<uint8_t[]>>(blockArray)[i]];
		break;
	case 2:
		for (int i = 0; i < CHUNK_VOLUME; ++i)
			_solid[i] = _indexTransparency[std::get<std::unique_ptr<uint16_t[]>>(blockArray)[i]];
		break;
	default:
		break;
	}

	return _solid;
}



void BlockContainer::setBlock(ChunkLocalBlockPos blockPos, Block block)
{
	if (isEmpty()) blockArrayCreate();
	auto it = blockArrayIndicesByBlock.find(block);
	int _blockIndex;
	if (it != blockArrayIndicesByBlock.end()) _blockIndex = it->second;
	else
	{
		_blockIndex = addBlockToPallete(block);
		if (_blockIndex > 255) blockArrayExtend();
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



int BlockContainer::addBlockToPallete(Block block)
{
	blockArrayBlocksByIndex.push_back(block);
	blockArrayIndicesByBlock[block] = currentIndex;
	return currentIndex++;
}



bool BlockContainer::isEmpty() const
{
	return std::holds_alternative<std::monostate>(blockArray);
}
