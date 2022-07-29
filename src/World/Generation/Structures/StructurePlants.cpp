#include "StructurePlants.h"



std::vector<BlockPlace> getStructureTreePine(int height)
{
	const Block LOG(3);
	const Block LEAF(4);

	std::vector<BlockPlace> ret;

	// Tree trunk
	for (int i = 0; i < height - 1; ++i)
	{
		ret.push_back({ 0, i, 0, LOG, 1 });
	}

	const int leafBase = height - 1;
	ret.push_back({ 0, leafBase, 0, LEAF, 0 });
	ret.push_back({ 0, leafBase + 1, 0, LEAF, 0 });
	ret.push_back({ -1, leafBase, 0, LEAF, 0 });
	ret.push_back({ 1, leafBase, 0, LEAF, 0 });
	ret.push_back({ 0, leafBase, -1, LEAF, 0 });
	ret.push_back({ 0, leafBase,  1, LEAF, 0 });

	ret.push_back({ -1, leafBase - 2, -1, LEAF, 0 });
	ret.push_back({ -1, leafBase - 2,  0, LEAF, 0 });
	ret.push_back({ -1, leafBase - 2,  1, LEAF, 0 });
	ret.push_back({ 1, leafBase - 2, -1, LEAF, 0 });
	ret.push_back({ 1, leafBase - 2,  0, LEAF, 0 });
	ret.push_back({ 1, leafBase - 2,  1, LEAF, 0 });
	ret.push_back({ 0, leafBase - 2, -1, LEAF, 0 });
	ret.push_back({ 0, leafBase - 2,  1, LEAF, 0 });

	if (height > 7)
	{
		ret.push_back({ -1, leafBase - 4, -1, LEAF, 0 });
		ret.push_back({ -1, leafBase - 4,  0, LEAF, 0 });
		ret.push_back({ -1, leafBase - 4,  1, LEAF, 0 });
		ret.push_back({ 1, leafBase - 4, -1, LEAF, 0 });
		ret.push_back({ 1, leafBase - 4,  0, LEAF, 0 });
		ret.push_back({ 1, leafBase - 4,  1, LEAF, 0 });
		ret.push_back({ 0, leafBase - 4, -1, LEAF, 0 });
		ret.push_back({ 0, leafBase - 4,  1, LEAF, 0 });
		ret.push_back({ -2, leafBase - 4,  0, LEAF, 0 });
		ret.push_back({ 2, leafBase - 4,  0, LEAF, 0 });
		ret.push_back({ 0, leafBase - 4, -2, LEAF, 0 });
		ret.push_back({ 0, leafBase - 4,  2, LEAF, 0 });
	}

	return ret;
}



