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
	if (std::holds_alternative<std::unique_ptr<std::array<u8, CHUNK_VOLUME>>>(blockArray)) {
		return;
	}

	auto newArray = std::make_unique<std::array<u8, CHUNK_VOLUME>>();
	if (std::holds_alternative<Block>(blockArray)) {
		Block _block = std::get<Block>(blockArray);
		blockArrayBlocksByIndex.push_back(Block(0));
		if (_block.blockType != 0) {
			blockArrayBlocksByIndex.push_back(_block);
			newArray->fill(1u);
		}
	}
	else if (std::holds_alternative<std::unique_ptr<std::array<u16, CHUNK_VOLUME>>>(blockArray)) {
		if (blockArrayBlocksByIndex.size() > 256) {
			throw std::runtime_error("Cannot shrink block array to byte, too many blocks.");
		}
		auto& curArray = *std::get<std::unique_ptr<std::array<u16, CHUNK_VOLUME>>>(blockArray);
		std::ranges::transform(
			curArray,
			newArray->begin(),
			[](u16 x) {
				return static_cast<uint8_t>(x);
			}
		);
	}
	blockArray = std::move(newArray);
}



void BlockContainer::setSizeShort() {	
	if (std::holds_alternative<std::unique_ptr<std::array<u16, CHUNK_VOLUME>>>(blockArray)) {
		return;
	}

	auto newArray = std::make_unique<std::array<u16, CHUNK_VOLUME>>();
	if (std::holds_alternative<Block>(blockArray)) {
		Block _block = std::get<Block>(blockArray);
		blockArrayBlocksByIndex.push_back(Block(0));
		if (_block.blockType != 0) {
			blockArrayBlocksByIndex.push_back(_block);
			newArray->fill(1u);
		}
	}
	else if (std::holds_alternative<std::unique_ptr<std::array<u8, CHUNK_VOLUME>>>(blockArray)) {
		auto& curArray = *std::get<std::unique_ptr<std::array<u8, CHUNK_VOLUME>>>(blockArray);
		std::ranges::copy(curArray, newArray->begin());
	}
	blockArray = std::move(newArray);
}



BlockContainer BlockContainer::clone() const {
	struct CloneVisitor {
		decltype(blockArray) operator()(const Block& curArray) {
			return curArray;
		}
		decltype(blockArray) operator()(const std::unique_ptr<std::array<u8, CHUNK_VOLUME>>& curArray) {
			auto newArray = std::make_unique<std::array<u8, CHUNK_VOLUME>>();
			std::ranges::copy(*curArray, newArray->begin());
			return newArray;
		}
		decltype(blockArray) operator()(const std::unique_ptr<std::array<u16, CHUNK_VOLUME>>& curArray) {
			auto newArray = std::make_unique<std::array<u16, CHUNK_VOLUME>>();
			std::ranges::copy(*curArray, newArray->begin());
			return newArray;
		}
	};

	BlockContainer newBlockContainer{};
	newBlockContainer.blockArray = std::visit(CloneVisitor(), blockArray);
	newBlockContainer.blockArrayBlocksByIndex = blockArrayBlocksByIndex;
	return newBlockContainer;
}



Block BlockContainer::getBlock(ChunkLocalBlockPos blockPos) const noexcept {
	struct GetVisitor {
		ChunkLocalBlockPos pos;

		Block operator()(const Block& block) {
			return block;
		}
		Block operator()(const std::unique_ptr<std::array<u8, CHUNK_VOLUME>>& arr) {
			return (*arr)[pos.asIndex()];
		}
		Block operator()(const std::unique_ptr<std::array<u16, CHUNK_VOLUME>>& arr) {
			return (*arr)[pos.asIndex()];
		}
	};
	return std::visit(GetVisitor{blockPos}, blockArray);
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
	std::ranges::transform(
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
		auto& _array = *std::get<1>(blockArray);
		for (size_t i = 0; i < CHUNK_VOLUME; ++i) {
			_solid[i] = _indexTransparency[_array[i]];
		}
		break;
	}
	case 2: {
		auto& _array = *std::get<2>(blockArray);
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

	boost::container::small_vector<bool, 64> _indexTransparency;
	_indexTransparency.reserve(blockArrayBlocksByIndex.size());
	std::ranges::transform(
		blockArrayBlocksByIndex,
		std::back_inserter(_indexTransparency),
		[](Block b) -> bool {
			return IS_SOLID[b.blockType];
		}
	);

	std::vector<bool> _solid(CHUNK_AREA);
	if (std::holds_alternative<std::unique_ptr<std::array<u8, CHUNK_VOLUME>>>(blockArray)) {
		const auto& _array = *std::get<std::unique_ptr<std::array<u8, CHUNK_VOLUME>>>(blockArray);

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
	// u16 array
	else {
		auto& _array = *std::get<std::unique_ptr<std::array<u16, CHUNK_VOLUME>>>(blockArray);

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
void BlockContainer::setBlockRaw(u16 arrayIndex, u16 blockIndex) {
	struct SetBlockRawVisitor {
		u16 arrayIdx;
		u16 blockIdx;

		void operator()([[maybe_unused]] const Block& block) {}
		void operator()(const std::unique_ptr<std::array<u8, CHUNK_VOLUME>>& arr) {
			(*arr)[arrayIdx] = static_cast<u8>(blockIdx);
		}
		void operator()(const std::unique_ptr<std::array<u16, CHUNK_VOLUME>>& arr) {
			(*arr)[arrayIdx] = blockIdx;
		}
	};
	std::visit(SetBlockRawVisitor{arrayIndex, blockIndex}, blockArray);
}



u16 BlockContainer::getOrAddPalleteIndex(Block block) {
	for (u16 i = 0; i < blockArrayBlocksByIndex.size(); ++i) {
		if (blockArrayBlocksByIndex[i] == block) {
			return i;
		}
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
