#include "BiomeMap.h"
#include <algorithm>
#include "NoiseSource.h"



double BIOME_TABLE[5][5] = {
	{  0.0 ,  0.0 ,  0.0 ,  0.0 ,  2.0  },
	{  0.0 ,  0.0 ,  0.0 ,  2.0 ,  2.0  },
	{  0.0 ,  0.0 ,  2.0 ,  2.0 ,  2.0  },
	{  0.0 ,  0.0 ,  1.0 ,  1.0 ,  1.0  },
	{  0.0 ,  0.4 ,  1.0 ,  1.0 ,  1.0  }
};



BiomeMap::BiomeMap(ChunkPos2D noisePos, NoiseSource2D& noiseTemperature, NoiseSource2D& noiseHumidity) : biomeArray{}
{
	std::array<float, CHUNK_AREA> temperature = noiseTemperature.GenChunkNoise(noisePos);
	std::array<float, CHUNK_AREA> humidity = noiseHumidity.GenChunkNoise(noisePos);

	for (int lZ = 0; lZ < CHUNK_SIZE; ++lZ)
	{
		for (int lX = 0; lX < CHUNK_SIZE; ++lX)
		{
			int _index = lZ * CHUNK_SIZE + lX;
			int _dist = noisePos.x * CHUNK_SIZE + lX;
			temperature[_index] = std::max(0.0, temperature[_index] - (std::abs(_dist) / static_cast<double>(WORLD_RADIUS * 2)));
		}
	}

	for (int i = 0; i < CHUNK_AREA; ++i)
	{
		if (temperature[i] < 0.3) biomeArray[i] = 1;
	}
}