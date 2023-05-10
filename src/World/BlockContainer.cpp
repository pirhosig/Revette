#include "BlockContainter.h"
#include <cassert>
#include "../Exceptions.h"


// This should ideally be stored in a physics engine lookup, but it works for now
// TODO: move this to a physics engine
const bool IS_SOLID[] = {
	false,
	true,
	true,
	true,
	false,
	true,
	false,
	true,
	true,
	true,
	false,
	true,
	false,
	true,
	false,
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
	true
};



inline int blockPositionIsInside(int x, int y, int z)
{
	return (x >= 0) && (x < CHUNK_SIZE) && (y >= 0) && (y < CHUNK_SIZE) && (z >= 0) && (z < CHUNK_SIZE);
}



inline int flattenIndex(const ChunkLocalBlockPos blockPos)
{
	// assert(blockPositionIsInside(blockPos.x, blockPos.y, blockPos.z) && "Block outside chunk.");
	return blockPos.x * CHUNK_AREA + blockPos.y * CHUNK_SIZE + blockPos.z;
}



void BlockContainer::blockArrayCreate()
{
	assert(std::holds_alternative<std::monostate>(blockArray));
	blockArray = std::make_unique<uint8_t[]>(CHUNK_VOLUME);
	blockArrayBlocksByIndex.push_back(emptyBlock);
}



void BlockContainer::blockArrayDelete(Block block)
{
	blockArray = std::monostate();
	emptyBlock = block;
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
		return emptyBlock;
		break;
	case 1:
		_blockIndex = std::get<1>(blockArray)[_index];
		break;
	case 2:
		_blockIndex = std::get<2>(blockArray)[_index];
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
	std::vector<bool> _solid(CHUNK_VOLUME);
	std::vector<bool> _indexTransparency(blockArrayBlocksByIndex.size());
	for (std::size_t i = 0; i < blockArrayBlocksByIndex.size(); ++i)
		_indexTransparency[i] = IS_SOLID[blockArrayBlocksByIndex[i].blockType];

	switch (blockArray.index())
	{
	case 1:
		for (int i = 0; i < CHUNK_VOLUME; ++i)
			_solid[i] = _indexTransparency[std::get<1>(blockArray)[i]];
		break;
	case 2:
		for (int i = 0; i < CHUNK_VOLUME; ++i)
			_solid[i] = _indexTransparency[std::get<2>(blockArray)[i]];
		break;
	default:
		break;
	}

	return _solid;
}



std::vector<bool> BlockContainer::getSolidFace(AxisDirection direction) const
{
	// Skip any checking if the chunk contains only one block type
	if (isEmpty()) return std::vector<bool>(CHUNK_AREA, IS_SOLID[emptyBlock.blockType]);

	std::vector<bool> _solid(CHUNK_AREA);
	std::vector<bool> _indexTransparency(blockArrayBlocksByIndex.size());
	for (std::size_t i = 0; i < blockArrayBlocksByIndex.size(); ++i)
		_indexTransparency[i] = IS_SOLID[blockArrayBlocksByIndex[i].blockType];

	int _indexOffset = 0;
	switch (blockArray.index())
	{
	case 1:
		switch (direction)
		{
		case AxisDirection::Up:
			_indexOffset = (CHUNK_SIZE - 1) * CHUNK_SIZE;
		case AxisDirection::Down:
			for (unsigned lX = 0; lX < CHUNK_SIZE; ++lX)
				for (unsigned lZ = 0; lZ < CHUNK_SIZE; ++lZ)
					_solid[lX * CHUNK_SIZE + lZ] = _indexTransparency[std::get<1>(blockArray)[_indexOffset + lX * CHUNK_AREA + lZ]];
			break;
		case AxisDirection::North:
			_indexOffset = (CHUNK_SIZE - 1) * CHUNK_AREA;
		case AxisDirection::South:
			for (unsigned lY = 0; lY < CHUNK_SIZE; ++lY)
				for (unsigned lZ = 0; lZ < CHUNK_SIZE; ++lZ)
					_solid[lY * CHUNK_SIZE + lZ] = _indexTransparency[std::get<1>(blockArray)[_indexOffset + lY * CHUNK_SIZE + lZ]];
			break;
		case AxisDirection::East:
			_indexOffset = CHUNK_SIZE - 1;
		case AxisDirection::West:
			for (unsigned lX = 0; lX < CHUNK_SIZE; ++lX)
				for (unsigned lY = 0; lY < CHUNK_SIZE; ++lY)
					_solid[lX * CHUNK_SIZE + lY] = _indexTransparency[std::get<1>(blockArray)[_indexOffset + lX * CHUNK_AREA + lY * CHUNK_SIZE]];
			break;
		default:
			break;
		}
		break;
	case 2:
		switch (direction)
		{
		case AxisDirection::Up:
			_indexOffset = (CHUNK_SIZE - 1) * CHUNK_SIZE;
		case AxisDirection::Down:
			for (unsigned lX = 0; lX < CHUNK_SIZE; ++lX)
				for (unsigned lZ = 0; lZ < CHUNK_SIZE; ++lZ)
					_solid[lX * CHUNK_SIZE + lZ] = _indexTransparency[std::get<2>(blockArray)[_indexOffset + lX * CHUNK_AREA + lZ]];
			break;
		case AxisDirection::North:
			_indexOffset = (CHUNK_SIZE - 1) * CHUNK_AREA;
		case AxisDirection::South:
			for (unsigned lY = 0; lY < CHUNK_SIZE; ++lY)
				for (unsigned lZ = 0; lZ < CHUNK_SIZE; ++lZ)
					_solid[lY * CHUNK_SIZE + lZ] = _indexTransparency[std::get<2>(blockArray)[_indexOffset + lY * CHUNK_SIZE + lZ]];
			break;
		case AxisDirection::East:
			_indexOffset = CHUNK_SIZE - 1;
		case AxisDirection::West:
			for (unsigned lX = 0; lX < CHUNK_SIZE; ++lX)
				for (unsigned lY = 0; lY < CHUNK_SIZE; ++lY)
					_solid[lX * CHUNK_SIZE + lY] = _indexTransparency[std::get<2>(blockArray)[_indexOffset + lX * CHUNK_AREA + lY * CHUNK_SIZE]];
			break;
		default:
			break;
		}
		break;
	default:
		break;
	}
	return _solid;
}



void BlockContainer::setBlock(ChunkLocalBlockPos blockPos, Block block)
{
	if (isEmpty()) blockArrayCreate();
	// Linear search for matching index, if it doesn't exist then it is added
	int _blockIndex = -1;
	for (int i = 0; i < static_cast<int>(blockArrayBlocksByIndex.size()); ++i) {
		if (blockArrayBlocksByIndex[i] == block)
		{
			_blockIndex = i;
			break;
		}
	}
	if (_blockIndex == -1)
	{
		_blockIndex = addBlockToPallete(block);
		if (_blockIndex > 255) blockArrayExtend();
	}
	setBlockRaw(flattenIndex(blockPos), _blockIndex);
}



// Directly sets the value in the block array, without any safety checks
void BlockContainer::setBlockRaw(int arrayIndex, int blockIndex)
{
	if (blockArray.index() == 1) std::get<1>(blockArray)[arrayIndex] = static_cast<uint8_t>(blockIndex);
	else std::get<2>(blockArray)[arrayIndex] = static_cast<uint16_t>(blockIndex);
}



int BlockContainer::addBlockToPallete(Block block)
{
	blockArrayBlocksByIndex.push_back(block);
	return static_cast<int>(blockArrayBlocksByIndex.size() - 1);
}



bool BlockContainer::isEmpty() const
{
	return std::holds_alternative<std::monostate>(blockArray);
}
