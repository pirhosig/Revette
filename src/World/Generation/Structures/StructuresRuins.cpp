#include "StructuresRuins.h"
#include "../ChunkPRNG.h"
#include "../../Chunk.h"



namespace Structures::Ruins
{
	void clayFrame(Chunk& chunk, ChunkPRNG& prng, BlockPos base)
	{
		const Block CLAY(18);
		{
			int height = prng.scaledInt(4.7, 1.4);
			for (int i = 0; i < height; ++i)
				chunk.setBlockPopulation(base.offset(-2, i, 0), Block(18), 1030);
		}

		{
			int height = prng.scaledInt(4.7, 1.4);
			for (int i = 0; i < height; ++i)
				chunk.setBlockPopulation(base.offset(2, i, 0), Block(18), 1030);
		}
	}
}