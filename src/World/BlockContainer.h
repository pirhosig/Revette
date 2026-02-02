#pragma once
#include <memory>
#include <variant>
#include <vector>
#include "AxisDirection.h"
#include "Block.h"
#include "ChunkPos.h"



class BlockContainer {
public:
	std::variant<Block, std::unique_ptr<uint8_t[]>, std::unique_ptr<uint16_t[]>> blockArray;
	std::vector<Block> blockArrayBlocksByIndex;

public:
	BlockContainer();

	void setSingleBlock(Block block);
	void setSizeByte();
	void setSizeShort();

	Block getBlock(ChunkLocalBlockPos blockPos) const;
	Block getBlockRaw(unsigned int index) const;
	std::vector<bool> getSolid() const;
	std::vector<bool> getSolidFace(AxisDirection direction) const;

	void setBlock(ChunkLocalBlockPos blockPos, Block block);
	void setBlockRaw(int arrayIndex, int blockIndex);
	int getOrAddPalleteIndex(Block block);

	bool isAir() const;
	bool isSolid() const;
};
