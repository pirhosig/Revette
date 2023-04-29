#pragma once
#include <memory>
#include <variant>
#include <vector>
#include "ChunkPos.h"
#include "Block.h"



class BlockContainer
{
public:
	BlockContainer() : emptyBlock{ Block(0) } {};
	BlockContainer(const BlockContainer&) = delete;

	void blockArrayCreate();
	void blockArrayDelete();
	void blockArrayExtend();

	Block getBlock(ChunkLocalBlockPos blockPos) const;
	std::vector<bool> getSolid() const;

	void setBlock(ChunkLocalBlockPos blockPos, Block block);
	void setBlockFill(Block block);
	void setBlockRaw(int arrayIndex, int blockIndex);
	int addBlockToPallete(Block block);

	bool isEmpty() const;

	Block emptyBlock;

	std::variant<std::monostate, std::unique_ptr<uint8_t[]>, std::unique_ptr<uint16_t[]>> blockArray;
	std::vector<Block> blockArrayBlocksByIndex;
};

