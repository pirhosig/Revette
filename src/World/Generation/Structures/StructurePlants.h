#pragma once
#include "../../Block.h"

class Chunk;
class ChunkPRNG;


namespace Structures::Trees
{
	// Boreal Forest
	void PineBasic(Chunk& chunk, ChunkPRNG& prng, BlockPos base);
	void PineFancy(Chunk& chunk, ChunkPRNG& prng, BlockPos base);
	void PineMassive(Chunk& chunk, ChunkPRNG& prng, BlockPos base);


	// Rainforest
	void RainforestBasic(Chunk& chunk, ChunkPRNG& prng, BlockPos base);
	void RainforestShrub(Chunk& chunk, BlockPos base);
	void RainforestTall(Chunk& chunk, ChunkPRNG& prng, BlockPos base);


	// Savannah
	void SavannahBaobab(Chunk& chunk, ChunkPRNG& prng, BlockPos base);
	void SavannahAcacia(Chunk& chunk, ChunkPRNG& prng, BlockPos base);


	// Temperate Forest
	void Aspen(Chunk& chunk, ChunkPRNG& prng, BlockPos base);
	void Oak(Chunk& chunk, ChunkPRNG& prng, BlockPos base);
}