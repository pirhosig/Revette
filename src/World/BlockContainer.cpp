#include "BlockContainer.h"

#include <algorithm>
#include <cassert>

#include <boost/container/small_vector.hpp>

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
			std::fill(newArray.get(), newArray.get() + CHUNK_VOLUME, 1u);
		}
	}
	else if (std::holds_alternative<std::unique_ptr<uint16_t[]>>(blockArray)) {
		if (blockArrayBlocksByIndex.size() > 256) {
			throw std::runtime_error("Cannot shrink block array to byte, too many blocks.");
		}
		auto& currentArray = std::get<std::unique_ptr<uint16_t[]>>(blockArray);
		std::transform(
			currentArray.get(),
			currentArray.get() + CHUNK_SIZE,
			newArray.get(),
			[](uint16_t x) {
				return static_cast<uint8_t>(x);
			}
		);
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
			std::fill(newArray.get(), newArray.get() + CHUNK_VOLUME, 1u);
		}
	}
	else if (std::holds_alternative<std::unique_ptr<uint8_t[]>>(blockArray)) {
		auto& currentArray = std::get<std::unique_ptr<uint8_t[]>>(blockArray);
		std::copy(currentArray.get(), currentArray.get() + CHUNK_SIZE, newArray.get());
	}
	blockArray = std::move(newArray);
}



Block BlockContainer::getBlock(ChunkLocalBlockPos blockPos) const {
	size_t blockTypeIndex{};
	switch (blockArray.index()) {
	case 0:
		return std::get<0>(blockArray);
		break;
	case 1:
		blockTypeIndex = std::get<1>(blockArray)[blockPos.asIndex()];
		break;
	case 2:
		blockTypeIndex = std::get<2>(blockArray)[blockPos.asIndex()];
		break;
	default:
		return Block(0);
		break;
	}
	return blockArrayBlocksByIndex[blockTypeIndex];
}



std::vector<bool> BlockContainer::getSolid() const {
	if (std::holds_alternative<Block>(blockArray)) {
		return std::vector<bool>(
			CHUNK_VOLUME,
			IS_SOLID[std::get<Block>(blockArray).blockType]
		);
	}

	boost::container::small_vector<bool, 64U> _indexTransparency;
	_indexTransparency.reserve(blockArrayBlocksByIndex.size());
	std::transform(
		blockArrayBlocksByIndex.begin(),
		blockArrayBlocksByIndex.end(),
		std::back_inserter(_indexTransparency),
		[](Block b) -> bool {
			return IS_SOLID[b.blockType];
		}
	);
	
	std::vector<bool> _solid(CHUNK_VOLUME);
	switch (blockArray.index()) {
	case 0:
		break;
	case 1: {
		auto& _array = std::get<std::unique_ptr<uint8_t[]>>(blockArray);
		for (size_t i = 0; i < CHUNK_VOLUME; ++i) {
			_solid[i] = _indexTransparency[_array[i]];
		}
		break;
	}
	case 2: {
		auto& _array = std::get<std::unique_ptr<uint16_t[]>>(blockArray);
		for (size_t i = 0; i < CHUNK_VOLUME; ++i) {
			_solid[i] = _indexTransparency[_array[i]];
		}
		break;
	}
	}

	return _solid;
}



std::vector<bool> BlockContainer::getSolidFace(AxisDirection direction) const {
	if (std::holds_alternative<Block>(blockArray)) {
		return std::vector<bool>(CHUNK_AREA, IS_SOLID[std::get<Block>(blockArray).blockType]);
	}

	boost::container::small_vector<bool, 64U> _indexTransparency;
	_indexTransparency.reserve(blockArrayBlocksByIndex.size());
	std::transform(
		blockArrayBlocksByIndex.begin(),
		blockArrayBlocksByIndex.end(),
		std::back_inserter(_indexTransparency),
		[](Block b) -> bool {
			return IS_SOLID[b.blockType];
		}
	);

	std::vector<bool> _solid(CHUNK_AREA);
	if (std::holds_alternative<std::unique_ptr<uint8_t[]>>(blockArray)) {
		const auto& _array = std::get<std::unique_ptr<uint8_t[]>>(blockArray);

		switch (direction) {
		case AxisDirection::Up:
			for (unsigned lX = 0; lX < CHUNK_SIZE; ++lX) {
			for (unsigned lZ = 0; lZ < CHUNK_SIZE; ++lZ) {
				const auto posIn  = ((CHUNK_SIZE - 1) * CHUNK_SIZE) + (lX * CHUNK_AREA) + lZ;
				const auto posOut = (lX * CHUNK_SIZE) + lZ;
				_solid[posOut] = _indexTransparency[_array[posIn]];
			}
			}
			break;
		case AxisDirection::Down:
			for (unsigned lX = 0; lX < CHUNK_SIZE; ++lX) {
			for (unsigned lZ = 0; lZ < CHUNK_SIZE; ++lZ) {
				const auto posIn  = (lX * CHUNK_AREA) + lZ;
				const auto posOut = (lX * CHUNK_SIZE) + lZ;
				_solid[posOut] = _indexTransparency[_array[posIn]];
			}
			}
			break;
		case AxisDirection::North:
			for (unsigned lY = 0; lY < CHUNK_SIZE; ++lY) {
			for (unsigned lZ = 0; lZ < CHUNK_SIZE; ++lZ) {
				const auto posIn  = ((CHUNK_SIZE - 1) * CHUNK_AREA) + (lY * CHUNK_SIZE) + lZ;
				const auto posOut = (lY * CHUNK_SIZE) + lZ;
				_solid[posOut] = _indexTransparency[_array[posIn]];
			}
			}
			break;
		case AxisDirection::South:
			for (unsigned lY = 0; lY < CHUNK_SIZE; ++lY) {
			for (unsigned lZ = 0; lZ < CHUNK_SIZE; ++lZ) {
				const auto posIn  = (lY * CHUNK_SIZE) + lZ;
				const auto posOut = (lY * CHUNK_SIZE) + lZ;
				_solid[posOut] = _indexTransparency[_array[posIn]];
			}
			}
			break;
		case AxisDirection::East:
			for (unsigned lX = 0; lX < CHUNK_SIZE; ++lX) {
			for (unsigned lY = 0; lY < CHUNK_SIZE; ++lY) {
				const auto posIn  = (CHUNK_SIZE - 1) + (lX * CHUNK_AREA) + (lY * CHUNK_SIZE);
				const auto posOut = (lX * CHUNK_SIZE) + lY;
				_solid[posOut] = _indexTransparency[_array[posIn]];
			}
			}
			break;
		case AxisDirection::West:
			for (unsigned lX = 0; lX < CHUNK_SIZE; ++lX) {
			for (unsigned lY = 0; lY < CHUNK_SIZE; ++lY) {
				const auto posIn  = (lX * CHUNK_AREA) + (lY * CHUNK_SIZE);
				const auto posOut = (lX * CHUNK_SIZE) + lY;
				_solid[posOut] = _indexTransparency[_array[posIn]];
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
			for (unsigned lX = 0; lX < CHUNK_SIZE; ++lX) {
			for (unsigned lZ = 0; lZ < CHUNK_SIZE; ++lZ) {
				const auto posIn  = ((CHUNK_SIZE - 1) * CHUNK_SIZE) + (lX * CHUNK_AREA) + lZ;
				const auto posOut = (lX * CHUNK_SIZE) + lZ;
				_solid[posOut] = _indexTransparency[_array[posIn]];
			}
			}
			break;
		case AxisDirection::Down:
			for (unsigned lX = 0; lX < CHUNK_SIZE; ++lX) {
			for (unsigned lZ = 0; lZ < CHUNK_SIZE; ++lZ) {
				const auto posIn  = (lX * CHUNK_AREA) + lZ;
				const auto posOut = (lX * CHUNK_SIZE) + lZ;
				_solid[posOut] = _indexTransparency[_array[posIn]];
			}
			}
			break;
		case AxisDirection::North:
			for (unsigned lY = 0; lY < CHUNK_SIZE; ++lY) {
			for (unsigned lZ = 0; lZ < CHUNK_SIZE; ++lZ) {
				const auto posIn  = ((CHUNK_SIZE - 1) * CHUNK_AREA) + (lY * CHUNK_SIZE) + lZ;
				const auto posOut = (lY * CHUNK_SIZE) + lZ;
				_solid[posOut] = _indexTransparency[_array[posIn]];
			}
			}
			break;
		case AxisDirection::South:
			for (unsigned lY = 0; lY < CHUNK_SIZE; ++lY) {
			for (unsigned lZ = 0; lZ < CHUNK_SIZE; ++lZ) {
				const auto posIn  = (lY * CHUNK_SIZE) + lZ;
				const auto posOut = (lY * CHUNK_SIZE) + lZ;
				_solid[posOut] = _indexTransparency[_array[posIn]];
			}
			}
			break;
		case AxisDirection::East:
			for (unsigned lX = 0; lX < CHUNK_SIZE; ++lX) {
			for (unsigned lY = 0; lY < CHUNK_SIZE; ++lY) {
				const auto posIn  = (CHUNK_SIZE - 1) + (lX * CHUNK_AREA) + (lY * CHUNK_SIZE);
				const auto posOut = (lX * CHUNK_SIZE) + lY;
				_solid[posOut] = _indexTransparency[_array[posIn]];
			}
			}
			break;
		case AxisDirection::West:
			for (unsigned lX = 0; lX < CHUNK_SIZE; ++lX) {
			for (unsigned lY = 0; lY < CHUNK_SIZE; ++lY) {
				const auto posIn  = (lX * CHUNK_AREA) + (lY * CHUNK_SIZE);
				const auto posOut = (lX * CHUNK_SIZE) + lY;
				_solid[posOut] = _indexTransparency[_array[posIn]];
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
	setBlockRaw(blockPos.asIndex(), getOrAddPalleteIndex(block));
}



// Directly sets the value in the block array, without any safety checks
void BlockContainer::setBlockRaw(uint16_t arrayIndex, uint16_t blockIndex) {
	if (std::holds_alternative<std::unique_ptr<uint8_t[]>>(blockArray)) {
		std::get<std::unique_ptr<uint8_t[]>>(blockArray)[arrayIndex] = static_cast<uint8_t>(blockIndex);
	}
	else if (std::holds_alternative<std::unique_ptr<uint16_t[]>>(blockArray)) {
		std::get<std::unique_ptr<uint16_t[]>>(blockArray)[arrayIndex] = blockIndex;
	}
}



uint16_t BlockContainer::getOrAddPalleteIndex(Block block) {
	for (uint16_t i = 0; i < static_cast<uint16_t>(blockArrayBlocksByIndex.size()); ++i) {
		if (blockArrayBlocksByIndex[i] == block) return i;
	}
	blockArrayBlocksByIndex.push_back(block);
	if (blockArrayBlocksByIndex.size() > 256) {
		setSizeShort();
	}
	return static_cast<uint32_t>(blockArrayBlocksByIndex.size() - 1);
}



bool BlockContainer::isAir() const {
	return std::holds_alternative<Block>(blockArray) && std::get<Block>(blockArray).blockType == 0;
}



bool BlockContainer::isSolid() const {
	return std::holds_alternative<Block>(blockArray) && IS_SOLID[std::get<Block>(blockArray).blockType];
}
