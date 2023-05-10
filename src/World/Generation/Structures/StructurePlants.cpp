#include "StructurePlants.h"
#include "../../Chunk.h"
#include "../ChunkPRNG.h"

/*
Idk where else to put this, but I guess an explanation of the basic structure parameters is necessary
Chunk: self explanatory, the structure centre is in
BlockPos: the centre of the structure, the structure may extended up to 32 blocks in any directions from this block
age: the base age of the structure, older structures will overwrite newer ones in any placement overlaps

Tree specific parameters:
height: the height of the tree trunk, which the leaves may or may not extend above
*/



namespace Structures
{
	void placeTreePine(Chunk& chunk, ChunkPRNG& prng, BlockPos base)
	{
		const Block LOG(3);
		const Block LEAF(4);

		const unsigned long long age = 0;
		const int height = prng.scaledInt(2.4, 6.6);

		// Tree trunk
		for (int i = 0; i < height; ++i) chunk.setBlockPopulation(base.offset(0, i, 0), LOG, age + 1);

		const int leafBase = height;

		chunk.setBlockPopulation(base.offset(0, leafBase, 0), LEAF, age);
		chunk.setBlockPopulation(base.offset(0, leafBase + 1, 0), LEAF, age);
		chunk.setBlockPopulation(base.offset(-1, leafBase, 0), LEAF, age);
		chunk.setBlockPopulation(base.offset(1, leafBase, 0), LEAF, age);
		chunk.setBlockPopulation(base.offset(0, leafBase, -1), LEAF, age);
		chunk.setBlockPopulation(base.offset(0, leafBase, 1), LEAF, age);

		chunk.setBlockPopulation(base.offset(-1, leafBase - 2, -1), LEAF, age);
		chunk.setBlockPopulation(base.offset(-1, leafBase - 2, 0), LEAF, age);
		chunk.setBlockPopulation(base.offset(-1, leafBase - 2, 1), LEAF, age);
		chunk.setBlockPopulation(base.offset(1, leafBase - 2, -1), LEAF, age);
		chunk.setBlockPopulation(base.offset(1, leafBase - 2, 0), LEAF, age);
		chunk.setBlockPopulation(base.offset(1, leafBase - 2, 1), LEAF, age);
		chunk.setBlockPopulation(base.offset(0, leafBase - 2, -1), LEAF, age);
		chunk.setBlockPopulation(base.offset(0, leafBase - 2, 1), LEAF, age);

		if (height > 6)
		{
			chunk.setBlockPopulation(base.offset(-1, leafBase - 4, -1), LEAF, age);
			chunk.setBlockPopulation(base.offset(-1, leafBase - 4, 0), LEAF, age);
			chunk.setBlockPopulation(base.offset(-1, leafBase - 4, 1), LEAF, age);
			chunk.setBlockPopulation(base.offset(1, leafBase - 4, -1), LEAF, age);
			chunk.setBlockPopulation(base.offset(1, leafBase - 4, 0), LEAF, age);
			chunk.setBlockPopulation(base.offset(1, leafBase - 4, 1), LEAF, age);
			chunk.setBlockPopulation(base.offset(0, leafBase - 4, -1), LEAF, age);
			chunk.setBlockPopulation(base.offset(0, leafBase - 4, 1), LEAF, age);
			chunk.setBlockPopulation(base.offset(-2, leafBase - 4, 0), LEAF, age);
			chunk.setBlockPopulation(base.offset(2, leafBase - 4, 0), LEAF, age);
			chunk.setBlockPopulation(base.offset(0, leafBase - 4, -2), LEAF, age);
			chunk.setBlockPopulation(base.offset(0, leafBase - 4, 2), LEAF, age);
		}
	}


	void placeTreePineMassive(Chunk& chunk, ChunkPRNG& prng, BlockPos base)
	{
		const Block LOG(3);
		const Block LEAF(4);

		const unsigned long long age = 0;
		const int height = prng.scaledInt(4.7, 15.7);

		// Tree trunk
		for (int i = 0; i < height; ++i) chunk.setBlockPopulation(base.offset(0, i, 0), LOG, age + 1);

		// Top leaf
		chunk.setBlockPopulation(base.offset(0, height, 0), LEAF, age);

		// Subsequent leaf layers
		for (int _level = height - 1; _level > 5; _level -= 2)
		{
			int _radius = (height - _level + 1) / 2;
			// Cardinal directions
			for (int i = 1; i <= _radius; ++i)
			{
				chunk.setBlockPopulation(base.offset(-i, _level,  0), LEAF, age);
				chunk.setBlockPopulation(base.offset( i, _level,  0), LEAF, age);
				chunk.setBlockPopulation(base.offset( 0, _level, -i), LEAF, age);
				chunk.setBlockPopulation(base.offset( 0, _level,  i), LEAF, age);
			}
			// Corners
			for (int i = 1; i < _radius; ++i)
			{
				int _width = _radius - i;
				for (int j = 1; j <= _width; ++j)
				{
					chunk.setBlockPopulation(base.offset(-i, _level, -j), LEAF, age);
					chunk.setBlockPopulation(base.offset(-i, _level,  j), LEAF, age);
					chunk.setBlockPopulation(base.offset( i, _level, -j), LEAF, age);
					chunk.setBlockPopulation(base.offset( i, _level,  j), LEAF, age);
				}
			}
		}
	}



	void placeTreeRainforestBasic(Chunk& chunk, ChunkPRNG& prng, BlockPos base)
	{
		const Block LEAF(4);

		const unsigned long long age = 0;
		const int height = prng.scaledInt(3.2, 9.9);

		for (int i = 0; i < height - 2; ++i)
			chunk.setBlockPopulation(base.offset(0, i, 0), Block(3), age + 1);
		// Add the leaves
		for (int i = -1; i < 2; ++i)
			for (int j = -1; j < 2; ++j)
				chunk.setBlockPopulation(base.offset(i, height - 2, j), LEAF, age);
		chunk.setBlockPopulation(base.offset(0 , height - 1,  0), LEAF, age);
		chunk.setBlockPopulation(base.offset(-2, height - 2,  0), LEAF, age);
		chunk.setBlockPopulation(base.offset(2 , height - 2,  0), LEAF, age);
		chunk.setBlockPopulation(base.offset(0 , height - 2, -2), LEAF, age);
		chunk.setBlockPopulation(base.offset(0 , height - 2,  2), LEAF, age);
	}



	void placeTreeRainforestShrub(Chunk& chunk, BlockPos base)
	{
		const Block LEAF(4);
		const unsigned long long age = 0;

		chunk.setBlockPopulation(base, Block(3), age + 1);
		chunk.setBlockPopulation(base.offset(0, 1, 0), LEAF, age);
		chunk.setBlockPopulation(base.offset(-1, 0, 0), LEAF, age);
		chunk.setBlockPopulation(base.offset(1, 0, 0), LEAF, age);
		chunk.setBlockPopulation(base.offset(0, 0, -1), LEAF, age);
		chunk.setBlockPopulation(base.offset(0, 0, 1), LEAF, age);
	}



	void placeTreeRainforestTall(Chunk& chunk, ChunkPRNG& prng, BlockPos base)
	{
		const Block LOG(3);
		const Block LEAF(4);

		const unsigned long long age = 0;
		const int height = prng.scaledInt(5.3, 15.7);

		for (int i = 0; i < height; ++i) chunk.setBlockPopulation(base.offset(0, i, 0), LOG, age + 1);

		chunk.setBlockPopulation(base.offset(-1, height, 0), LOG, age + 1);
		chunk.setBlockPopulation(base.offset(1, height, 0), LOG, age + 1);
		chunk.setBlockPopulation(base.offset(0, height, -1), LOG, age + 1);
		chunk.setBlockPopulation(base.offset(0, height, 1), LOG, age + 1);

		const int _leafBase = height + 1;

		chunk.setBlockPopulation(base.offset(-2, _leafBase, -1), LEAF, age);
		chunk.setBlockPopulation(base.offset(-2, _leafBase, 0), LEAF, age);
		chunk.setBlockPopulation(base.offset(-2, _leafBase, 1), LEAF, age);

		for (int i = -1; i < 2; ++i)
		{
			chunk.setBlockPopulation(base.offset(i, _leafBase, -2), LEAF, age);
			chunk.setBlockPopulation(base.offset(i, _leafBase, -1), LEAF, age);
			chunk.setBlockPopulation(base.offset(i, _leafBase, 0), LEAF, age);
			chunk.setBlockPopulation(base.offset(i, _leafBase, 1), LEAF, age);
			chunk.setBlockPopulation(base.offset(i, _leafBase, 2), LEAF, age);
		}

		chunk.setBlockPopulation(base.offset(2, _leafBase, -1), LEAF, age);
		chunk.setBlockPopulation(base.offset(2, _leafBase, 0), LEAF, age);
		chunk.setBlockPopulation(base.offset(2, _leafBase, 1), LEAF, age);

		chunk.setBlockPopulation(base.offset(0, _leafBase + 1, 0), LEAF, age);
		chunk.setBlockPopulation(base.offset(-1, _leafBase + 1, 0), LEAF, age);
		chunk.setBlockPopulation(base.offset(1, _leafBase + 1, 0), LEAF, age);
		chunk.setBlockPopulation(base.offset(0, _leafBase + 1, -1), LEAF, age);
		chunk.setBlockPopulation(base.offset(0, _leafBase + 1, 1), LEAF, age);
	}



	void placeTreeSavannahBaobab(Chunk& chunk, ChunkPRNG& prng, BlockPos base)
	{
		const Block LOG(3);
		const Block LEAF(4);

		const int age = 0;
		const int height = prng.scaledInt(8.0, 7.0);

		// Create trunk
		for (int lX = -1; lX <= 1; ++lX)
			for (int lZ = -1; lZ <= 1; ++lZ)
				for (int lY = 0; lY < height; ++lY)
					chunk.setBlockPopulation(base.offset(lX, lY, lZ), LOG, age + 2);

		// Random branches
		for (int lX = -1; lX <= 1; ++lX)
			for (int lZ = -1; lZ <= 1; ++lZ)
			{
				// No branch
				if (prng.next() < 0.35) continue;
				int cX = lX;
				int cY = height - 1;
				int cZ = lZ;
				for (int i = 0; i < 4; ++i)
				{
					auto nval = prng.next();
					if (nval < 0.250) break;
					if (nval > 0.750) cY++;
					else if (nval > 0.625) cX++;
					else if (nval > 0.500) cX--;
					else if (nval > 0.375) cZ++;
					else cZ--;
					chunk.setBlockPopulation(base.offset(cX, cY, cZ), LOG, age + 2);
				}
				// Chance to add leaves
				if (prng.next() > 0.2)
				{
					const int def[][2] = { {0, 0}, {-1, 0}, {1, 0}, {0, -1}, {0, 1} };
					for (auto [a, b] : def) chunk.setBlockPopulation(base.offset(cX + a, cY + 1, cZ + b), LEAF, age);
					const int pos[][3] = { {-1, 1, -1}, {-1, 1, 1}, {1, 1, -1}, {1, 1, 1}, {0, 2, 0} };
					for (auto [a, b, c] : pos) if (prng.next() > 0.6)
						chunk.setBlockPopulation(base.offset(cX + a, cY + b, cZ + c), LEAF, age);
				}
			}
	}



	void addLeaves(Chunk& chunk, ChunkPRNG& prng, BlockPos base, unsigned long long age)
	{
		const int def[][2] = { {0, 0}, {-1, 0}, {1, 0}, {0, -1}, {0, 1} };
		for (auto [a, b] : def) chunk.setBlockPopulation(base.offset(a, 0, b), Block(4), age);
		const int pos[][3] = { {-1, 0, -1}, {-1, 0, 1}, {1, 0, -1}, {1, 0, 1}, {0, 1, 0} };
		for (auto [a, b, c] : pos) if (prng.next() > 0.6)
			chunk.setBlockPopulation(base.offset(a, b, c), Block(4), age);
	}



	// Helper function for acacia trees
	void addBranch(Chunk& chunk, ChunkPRNG& prng, BlockPos base, int dX, int dZ)
	{
		int cX = 0;
		int cY = 0;
		int cZ = 0;
		for (int i = 0; i < 5; ++i)
		{
			auto a = prng.next();
			if (a < 0.45) break;
			// Chance to start another branch off this branch
			else if (a < 0.47)
			{
				const int dir[][2] = { {0, -1}, {0, 1}, {-1, 0}, {1, 0} };
				auto b = prng.raw() % 4;
				addBranch(chunk, prng, base.offset(cX, cY, cZ), dir[b][0], dir[b][1]);
			}
			else if (a < 0.74)
			{
				cX += dX;
				cZ += dZ;
			}
			else cY++;

			chunk.setBlockPopulation(base.offset(cX, cY, cZ), Block(3), 5);
		}

		addLeaves(chunk, prng, base.offset(cX, cY + 1, cZ), 0);
	}



	void placeTreeSavannahAcacia(Chunk& chunk, ChunkPRNG& prng, BlockPos base)
	{
		const Block LOG(3);
		const Block LEAF(4);

		const unsigned long long age = 0;
		const int height = prng.scaledInt(6.3, 1.89);

		for (int i = 0; i < height; ++i)
		{
			chunk.setBlockPopulation(base.offset(0, i, 0), LOG, age + 5);
			if (prng.next() > 0.71)
			{
				const int dir[][2] = { {0, -1}, {0, 1}, {-1, 0}, {1, 0} };
				auto rdir = prng.raw() % 4;
				addBranch(chunk, prng, base.offset(0, i, 0), dir[rdir][0], dir[rdir][1]);
			}
		}

		addLeaves(chunk, prng, base.offset(0, height, 0), 0);
	}



	void placeTreeAspen(Chunk& chunk, ChunkPRNG& prng, BlockPos base)
	{
		const Block LEAF(12);
		const Block LOG(13);

		const unsigned long long age = 0;
		const int height = prng.scaledInt(6.2, 12.7);

		// Trunk
		for (int i = 0; i < height; ++i) chunk.setBlockPopulation(base.offset(0, i, 0), LOG, age + 1);

		// Leaves
		for (int lX = -1; lX <= 1; ++lX)
			for (int lZ = -1; lZ <= 1; ++lZ) {
				if (!lX && !lZ) continue;
				chunk.setBlockPopulation(base.offset(lX, height - 1, lZ), LEAF, age);
			}

		chunk.setBlockPopulation(base.offset( 0, height - 2,  0), LEAF, age);
		chunk.setBlockPopulation(base.offset(-1, height - 2,  0), LEAF, age);
		chunk.setBlockPopulation(base.offset( 1, height - 2,  0), LEAF, age);
		chunk.setBlockPopulation(base.offset( 0, height - 2, -1), LEAF, age);
		chunk.setBlockPopulation(base.offset( 0, height - 2,  1), LEAF, age);

		chunk.setBlockPopulation(base.offset( 0, height,  0), LEAF, age);
		chunk.setBlockPopulation(base.offset(-1, height,  0), LEAF, age);
		chunk.setBlockPopulation(base.offset( 1, height,  0), LEAF, age);
		chunk.setBlockPopulation(base.offset( 0, height, -1), LEAF, age);
		chunk.setBlockPopulation(base.offset( 0, height,  1), LEAF, age);
	}



	void placeTreeOak(Chunk& chunk, ChunkPRNG& prng, BlockPos base)
	{
		const Block LOG(3);
		const Block LEAF(4);

		const unsigned long long age = 0;
		const int height = prng.scaledInt(5.3, 10.1);

		for (int i = 0; i < height; ++i) {
			chunk.setBlockPopulation(base.offset(0, i, 0), LOG, age + 1);
		}

		int POS_TABLE[][3] = {
			{ 0,  1,  0},
			{ 0,  1, -1},
			{ 0,  1,  1},
			{-1,  1,  0},
			{ 1,  1,  0},
			{-1,  0, -1},
			{-1,  0,  0},
			{-1,  0,  1},
			{ 0,  0, -1},
			{ 0,  0,  0},
			{ 0,  0,  1},
			{ 1,  0, -1},
			{ 1,  0,  0},
			{ 1,  0,  1},
			{-1, -1, -1},
			{-1, -1,  0},
			{-1, -1,  1},
			{ 0, -1, -1},
			{ 0, -1,  1},
			{ 1, -1, -1},
			{ 1, -1,  0},
			{ 1, -1,  1},
			{-2, -1, -1},
			{-2, -1,  0},
			{-2, -1,  1},
			{ 2, -1, -1},
			{ 2, -1,  0},
			{ 2, -1,  1},
			{-1, -1, -2},
			{ 0, -1, -2},
			{ 1, -1, -2},
			{-1, -1,  2},
			{ 0, -1,  2},
			{ 1, -1,  2},
			{ 0, -2, -1},
			{ 0, -2,  1},
			{-1, -2,  0},
			{ 1, -2,  0}
		};
		for (auto [i, j, k] : POS_TABLE) {
			chunk.setBlockPopulation(base.offset(i, height + j, k), LEAF, age);
		}
	}
}
