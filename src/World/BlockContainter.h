#pragma once
#include <map>
#include <memory>
#include <variant>
#include <vector>
#include "ChunkPos.h"
#include "Block.h"



class BlockContainer
{
public:
	BlockContainer();
	BlockContainer(const BlockContainer&) = delete;

	void blockArrayCreate();
	void blockArrayDelete();
	void blockArrayExtend();

	Block getBlock(ChunkLocalBlockPos blockPos) const;

	void setBlock(ChunkLocalBlockPos blockPos, Block block);
	void setBlockRaw(int arrayIndex, int blockIndex);

	bool isEmpty() const;

	std::variant<std::monostate, std::unique_ptr<uint8_t[]>, std::unique_ptr<uint16_t[]>> blockArray;
	std::vector<Block> blockArrayBlocksByIndex;
	std::map<Block, uint16_t, blockComparator> blockArrayIndicesByBlock;
	uint16_t currentIndex;
};

