#include "HeightMap.h"
#include <algorithm>
#include "NoiseSource.h"



HeightMap::HeightMap(ChunkPos2D noisePos, NoiseSource2D& noiseSource) : heightMax(INT_MIN), heightMin(INT_MAX)
{
	std::array<float, CHUNK_AREA> noiseMap = noiseSource.GenChunkNoise(noisePos);
	for (int i = 0; i < CHUNK_AREA; ++i)
	{
		heightArray[i] = static_cast<int>(noiseMap[i]);
		heightMax = std::max(heightMax, heightArray[i]);
		heightMin = std::min(heightMin, heightArray[i]);
	}
}