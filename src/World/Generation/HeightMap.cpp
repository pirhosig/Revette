#include "HeightMap.h"
#include <algorithm>
#include <numeric>
#include "NoiseSource.h"



HeightMap::HeightMap(ChunkPos2D noisePos, NoiseSource2D& noiseSource) :
	heightMin(std::numeric_limits<i16>::max()),
	heightMax(std::numeric_limits<i16>::min())
{
	std::array<float, CHUNK_AREA> noiseMap = noiseSource.genChunkNoise(noisePos);
	for (int i = 0; i < CHUNK_AREA; ++i)
	{
		heightArray[i] = static_cast<i16>(noiseMap[i]) + SEA_LEVEL;
		heightMax = std::max(heightMax, heightArray[i]);
		heightMin = std::min(heightMin, heightArray[i]);
	}
}