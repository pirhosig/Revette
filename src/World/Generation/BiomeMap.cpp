#include "BiomeMap.h"
#include <algorithm>
#include "NoiseSource.h"


BiomeMap::BiomeMap(ChunkPos2D noisePos, NoiseSource2D& noiseTemperature, NoiseSource2D& noiseHumidity) : biomeArray{}
{
	std::array<float, CHUNK_AREA> temperature = noiseTemperature.GenChunkNoise(noisePos);
	std::array<float, CHUNK_AREA> humidity = noiseHumidity.GenChunkNoise(noisePos);

	for (int i = 0; i < CHUNK_AREA; ++i)
	{
		temperature[i] = std::max(0.0, temperature[i] - std::abs(noisePos.x) / 60.0);

		if (temperature[i] < 0.3) biomeArray[i] = 1;
	}
}