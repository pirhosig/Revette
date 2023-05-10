#pragma once
#include <memory>
#include <variant>
#include <vector>
#include "AxisDirection.h"
#include "Block.h"
#include "ChunkPos.h"



class BlockContainer
{
public:
	BlockContainer() : emptyBlock{ Block(0) } {};
	BlockContainer(const BlockContainer&) = delete;

	void blockArrayCreate();
	void blockArrayDelete(Block block);
	void blockArrayExtend();

	Block getBlock(ChunkLocalBlockPos blockPos) const;
	std::vector<bool> getSolid() const;
	std::vector<bool> getSolidFace(AxisDirection direction) const;

	void setBlock(ChunkLocalBlockPos blockPos, Block block);
	void setBlockRaw(int arrayIndex, int blockIndex);
	int addBlockToPallete(Block block);

	bool isEmpty() const;

	Block emptyBlock;

	std::variant<std::monostate, std::unique_ptr<uint8_t[]>, std::unique_ptr<uint16_t[]>> blockArray;
	std::vector<Block> blockArrayBlocksByIndex;
};

