#pragma once
#include <memory>
#include <variant>
#include <vector>
#include "AxisDirection.h"
#include "Block.h"
#include "ChunkPos.h"



class BlockContainer {
	std::variant<
		Block,
		std::unique_ptr<std::array<u8,  CHUNK_VOLUME>>,
		std::unique_ptr<std::array<u16, CHUNK_VOLUME>>
	> blockArray;
	std::vector<Block> blockArrayBlocksByIndex;



public:
	BlockContainer();

	void setSingleBlock(Block block);
	void setSizeByte();
	void setSizeShort();

	BlockContainer clone() const;
	Block getBlock(ChunkLocalBlockPos blockPos) const noexcept;
	std::vector<bool> getSolid() const;
	std::vector<bool> getSolidFace(AxisDirection direction) const;

	void setBlock(ChunkLocalBlockPos blockPos, Block block);
	void setBlockRaw(uint16_t arrayIndex, uint16_t blockIndex);
	uint16_t getOrAddPalleteIndex(Block block);

	bool isAir() const;
	bool isSolid() const;
};
