#include "StructurePlants.h"
#include "../../Chunk.h"

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
	void placeTreePine(Chunk& chunk, BlockPos base, unsigned long long age, int height)
	{
		const Block LOG(3);
		const Block LEAF(4);

		// Tree trunk
		for (int i = 0; i < height; ++i) {
			chunk.setBlockPopulation(base.offset(0, i, 0), LOG, age + 1);
		}

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


	void placeTreePineMassive(Chunk& chunk, BlockPos base, unsigned long long age, int height)
	{
		const Block LOG(3);
		const Block LEAF(4);

		// Tree trunk
		for (int i = 0; i < height; ++i) {
			chunk.setBlockPopulation(base.offset(0, i, 0), LOG, age + 1);
		}

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



	void placeTreeRainforestShrub(Chunk& chunk, BlockPos base, unsigned long long age)
	{
		const Block LEAF(4);
		chunk.setBlockPopulation(base, Block(3), age + 1);
		chunk.setBlockPopulation(base.offset(0, 1, 0), LEAF, age);
		chunk.setBlockPopulation(base.offset(-1, 0, 0), LEAF, age);
		chunk.setBlockPopulation(base.offset(1, 0, 0), LEAF, age);
		chunk.setBlockPopulation(base.offset(0, 0, -1), LEAF, age);
		chunk.setBlockPopulation(base.offset(0, 0, 1), LEAF, age);
	}



	void placeTreeRainforestTall(Chunk& chunk, BlockPos base, unsigned long long age, int height)
	{
		const Block LOG(3);
		const Block LEAF(4);

		for (int i = 0; i < height; ++i) {
			chunk.setBlockPopulation(base.offset(0, i, 0), LOG, age + 1);
		}

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
}
