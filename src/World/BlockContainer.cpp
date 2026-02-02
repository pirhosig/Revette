#include "BlockContainer.h"
#include <cassert>
#include "../Exceptions.h"



namespace {

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



inline int flattenIndex(const ChunkLocalBlockPos blockPos) {
	// assert(blockPositionIsInside(blockPos.x, blockPos.y, blockPos.z) && "Block outside chunk.");
	return static_cast<unsigned>(blockPos.x) * CHUNK_AREA + static_cast<unsigned>(blockPos.y) * CHUNK_SIZE + blockPos.z;
}

}



BlockContainer::BlockContainer() : blockArray{Block(0)} {}



void BlockContainer::setSingleBlock(Block block) {
	blockArray = block;
	blockArrayBlocksByIndex.clear();
}



void BlockContainer::setSizeByte() {
	if (std::holds_alternative<std::unique_ptr<uint8_t[]>>(blockArray)) {
		return;
	}

	auto newArray = std::make_unique<uint8_t[]>(CHUNK_VOLUME);
	if (std::holds_alternative<Block>(blockArray)) {
		Block _block = std::get<Block>(blockArray);
		blockArrayBlocksByIndex.push_back(Block(0));
		if (_block.blockType != 0) {
			blockArrayBlocksByIndex.push_back(_block);
			for (int i = 0; i < CHUNK_VOLUME; ++i) {
				newArray[i] = 1u;
			}
		}
	}
	else if (std::holds_alternative<std::unique_ptr<uint16_t[]>>(blockArray)) {
		if (blockArrayBlocksByIndex.size() > 256) {
			throw std::runtime_error("Cannot shrink block array to byte, too many blocks.");
		}
		auto& currentArray = std::get<std::unique_ptr<uint16_t[]>>(blockArray);
		for (int i = 0; i < CHUNK_VOLUME; ++i) {
			newArray[i] = static_cast<uint8_t>(currentArray[i]);
		}
	}
	blockArray = std::move(newArray);
}



void BlockContainer::setSizeShort() {
	if (std::holds_alternative<std::unique_ptr<uint16_t[]>>(blockArray)) {
		return;
	}

	auto newArray = std::make_unique<uint16_t[]>(CHUNK_VOLUME);
	if (std::holds_alternative<Block>(blockArray)) {
		Block _block = std::get<Block>(blockArray);
		blockArrayBlocksByIndex.push_back(Block(0));
		if (_block.blockType != 0) {
			blockArrayBlocksByIndex.push_back(_block);
			for (int i = 0; i < CHUNK_VOLUME; ++i) {
				newArray[i] = 1u;
			}
		}
	}
	else if (std::holds_alternative<std::unique_ptr<uint8_t[]>>(blockArray)) {
		auto& currentArray = std::get<std::unique_ptr<uint8_t[]>>(blockArray);
		for (int i = 0; i < CHUNK_VOLUME; ++i) {
			newArray[i] = currentArray[i];
		}
	}
	blockArray = std::move(newArray);
}



Block BlockContainer::getBlock(ChunkLocalBlockPos blockPos) const {
	return getBlockRaw(flattenIndex(blockPos));
}



Block BlockContainer::getBlockRaw(unsigned int index) const {
	int _blockIndex{};
	switch (blockArray.index()) {
	case 0:
		return std::get<0>(blockArray);
		break;
	case 1:
		_blockIndex = std::get<1>(blockArray)[index];
		break;
	case 2:
		_blockIndex = std::get<2>(blockArray)[index];
		break;
	default:
		return Block(0);
		break;
	}
	return blockArrayBlocksByIndex[_blockIndex];
}



std::vector<bool> BlockContainer::getSolid() const {
	std::vector<bool> _solid(CHUNK_VOLUME);
	std::vector<bool> _indexTransparency(blockArrayBlocksByIndex.size());
	for (std::size_t i = 0; i < blockArrayBlocksByIndex.size(); ++i) {
		_indexTransparency[i] = IS_SOLID[blockArrayBlocksByIndex[i].blockType];
	}

	switch (blockArray.index()) {
	case 0:
		break;
	case 1: {
		auto& _array = std::get<std::unique_ptr<uint8_t[]>>(blockArray);
		for (int i = 0; i < CHUNK_VOLUME; ++i) {
			_solid[i] = _indexTransparency[_array[i]];
		}
		break;
	}
	case 2: {
		auto& _array = std::get<std::unique_ptr<uint16_t[]>>(blockArray);
		for (int i = 0; i < CHUNK_VOLUME; ++i) {
			_solid[i] = _indexTransparency[_array[i]];
		}
		break;
	}
	default:
		break;
	}

	return _solid;
}



std::vector<bool> BlockContainer::getSolidFace(AxisDirection direction) const {
	if (std::holds_alternative<Block>(blockArray)) {
		return std::vector<bool>(CHUNK_AREA, IS_SOLID[std::get<Block>(blockArray).blockType]);
	}

	std::vector<bool> _solid(CHUNK_AREA);
	std::vector<bool> _indexTransparency(blockArrayBlocksByIndex.size());
	for (std::size_t i = 0; i < blockArrayBlocksByIndex.size(); ++i) {
		_indexTransparency[i] = IS_SOLID[blockArrayBlocksByIndex[i].blockType];
	}

	int _indexOffset = 0;
	if (std::holds_alternative<std::unique_ptr<uint8_t[]>>(blockArray)) {
		auto& _array = std::get<std::unique_ptr<uint8_t[]>>(blockArray);

		switch (direction) {
		case AxisDirection::Up:
			_indexOffset = (CHUNK_SIZE - 1) * CHUNK_SIZE;
			[[fallthrough]];
		case AxisDirection::Down:
			for (unsigned lX = 0; lX < CHUNK_SIZE; ++lX) {
			for (unsigned lZ = 0; lZ < CHUNK_SIZE; ++lZ) {
				_solid[lX * CHUNK_SIZE + lZ] = _indexTransparency[_array[_indexOffset + lX * CHUNK_AREA + lZ]];
			}
			}
			break;
		case AxisDirection::North:
			_indexOffset = (CHUNK_SIZE - 1) * CHUNK_AREA;
			[[fallthrough]];
		case AxisDirection::South:
			for (unsigned lY = 0; lY < CHUNK_SIZE; ++lY) {
			for (unsigned lZ = 0; lZ < CHUNK_SIZE; ++lZ) {
				_solid[lY * CHUNK_SIZE + lZ] = _indexTransparency[_array[_indexOffset + lY * CHUNK_SIZE + lZ]];
			}
			}
			break;
		case AxisDirection::East:
			_indexOffset = CHUNK_SIZE - 1;
			[[fallthrough]];
		case AxisDirection::West:
			for (unsigned lX = 0; lX < CHUNK_SIZE; ++lX) {
			for (unsigned lY = 0; lY < CHUNK_SIZE; ++lY) {
				_solid[lX * CHUNK_SIZE + lY] = _indexTransparency[_array[_indexOffset + lX * CHUNK_AREA + lY * CHUNK_SIZE]];
			}
			}
			break;
		default:
			break;
		}
	}
	// uint16_t array
	else {
		auto& _array = std::get<std::unique_ptr<uint16_t[]>>(blockArray);

		switch (direction) {
		case AxisDirection::Up:
			_indexOffset = (CHUNK_SIZE - 1) * CHUNK_SIZE;
			[[fallthrough]];
		case AxisDirection::Down:
			for (unsigned lX = 0; lX < CHUNK_SIZE; ++lX) {
			for (unsigned lZ = 0; lZ < CHUNK_SIZE; ++lZ) {
				_solid[lX * CHUNK_SIZE + lZ] = _indexTransparency[_array[_indexOffset + lX * CHUNK_AREA + lZ]];
			}
			}
			break;
		case AxisDirection::North:
			_indexOffset = (CHUNK_SIZE - 1) * CHUNK_AREA;
			[[fallthrough]];
		case AxisDirection::South:
			for (unsigned lY = 0; lY < CHUNK_SIZE; ++lY) {
			for (unsigned lZ = 0; lZ < CHUNK_SIZE; ++lZ) {
				_solid[lY * CHUNK_SIZE + lZ] = _indexTransparency[_array[_indexOffset + lY * CHUNK_SIZE + lZ]];
			}
			}
			break;
		case AxisDirection::East:
			_indexOffset = CHUNK_SIZE - 1;
			[[fallthrough]];
		case AxisDirection::West:
			for (unsigned lX = 0; lX < CHUNK_SIZE; ++lX) {
			for (unsigned lY = 0; lY < CHUNK_SIZE; ++lY) {
				_solid[lX * CHUNK_SIZE + lY] = _indexTransparency[_array[_indexOffset + lX * CHUNK_AREA + lY * CHUNK_SIZE]];
			}
			}
			break;
		default:
			break;
		}
	}

	return _solid;
}



void BlockContainer::setBlock(ChunkLocalBlockPos blockPos, Block block) {
	if (std::holds_alternative<Block>(blockArray)) setSizeByte();
	setBlockRaw(flattenIndex(blockPos), getOrAddPalleteIndex(block));
}



// Directly sets the value in the block array, without any safety checks
void BlockContainer::setBlockRaw(int arrayIndex, int blockIndex) {
	if (std::holds_alternative<std::unique_ptr<uint8_t[]>>(blockArray)) {
		std::get<std::unique_ptr<uint8_t[]>>(blockArray)[arrayIndex] = static_cast<uint8_t>(blockIndex);
	}
	else if (std::holds_alternative<std::unique_ptr<uint16_t[]>>(blockArray)) {
		std::get<std::unique_ptr<uint16_t[]>>(blockArray)[arrayIndex] = static_cast<uint16_t>(blockIndex);
	}
}



int BlockContainer::getOrAddPalleteIndex(Block block) {
	for (int i = 0; i < static_cast<int>(blockArrayBlocksByIndex.size()); ++i) {
		if (blockArrayBlocksByIndex[i] == block) return i;
	}
	blockArrayBlocksByIndex.push_back(block);
	if (blockArrayBlocksByIndex.size() > 256) {
		setSizeShort();
	}
	return static_cast<int>(blockArrayBlocksByIndex.size() - 1);
}



bool BlockContainer::isAir() const {
	return std::holds_alternative<Block>(blockArray) && std::get<Block>(blockArray).blockType == 0;
}



bool BlockContainer::isSolid() const {
	return std::holds_alternative<Block>(blockArray) && IS_SOLID[std::get<Block>(blockArray).blockType];
}
