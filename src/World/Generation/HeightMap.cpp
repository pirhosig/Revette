#include "HeightMap.h"
#include <algorithm>
#include "NoiseSource.h"



HeightMap::HeightMap(ChunkPos2D noisePos, NoiseSource2D& noiseSource) : heightMax(SHRT_MIN), heightMin(SHRT_MAX)
{
	std::array<float, CHUNK_AREA> noiseMap = noiseSource.genChunkNoise(noisePos);
	for (int i = 0; i < CHUNK_AREA; ++i)
	{
		heightArray[i] = static_cast<short>(noiseMap[i]) + SEA_LEVEL;
		heightMax = std::max(heightMax, heightArray[i]);
		heightMin = std::min(heightMin, heightArray[i]);
	}
}