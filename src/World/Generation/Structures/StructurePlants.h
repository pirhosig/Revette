#pragma once
#include "../../Block.h"

class Chunk;
class ChunkPRNG;


namespace Structures
{
	// Boreal Forest
	void placeTreePine(Chunk& chunk, ChunkPRNG& prng, BlockPos base);
	void placeTreePineMassive(Chunk& chunk, ChunkPRNG& prng, BlockPos base);


	// Rainforest
	void placeTreeRainforestBasic(Chunk& chunk, ChunkPRNG& prng, BlockPos base);
	void placeTreeRainforestShrub(Chunk& chunk, BlockPos base);
	void placeTreeRainforestTall(Chunk& chunk, ChunkPRNG& prng, BlockPos base);


	// Savannah
	void placeTreeSavannahBaobab(Chunk& chunk, ChunkPRNG& prng, BlockPos base);
	void placeTreeSavannahAcacia(Chunk& chunk, ChunkPRNG& prng, BlockPos base);


	// Temperate Forest
	void placeTreeAspen(Chunk& chunk, ChunkPRNG& prng, BlockPos base);
	void placeTreeOak(Chunk& chunk, ChunkPRNG& prng, BlockPos base);
}