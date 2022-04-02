#include "BiomeMap.h"
#include <algorithm>
#include "NoiseSource.h"



double BIOME_TABLE[5][5] = {
	{  0.0 ,  0.0 ,  1.0 ,  1.0 ,  1.0  },
	{  0.0 ,  0.0 ,  1.0 ,  1.0 ,  1.0  },
	{  0.0 ,  0.0 ,  2.0 ,  2.0 ,  2.0  },
	{  0.0 ,  0.0 ,  0.0 ,  2.0 ,  2.0  },
	{  0.0 ,  0.0 ,  0.0 ,  0.0 ,  2.0  }
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
			temperature[_index] = std::max(0.0f, temperature[_index] - (std::abs(static_cast<float>(_dist)) / static_cast<float>(WORLD_RADIUS * 2)));
		}
	}

	for (int i = 0; i < CHUNK_AREA; ++i)
	{
		int _indexTemperature = static_cast<int>(temperature[i] * 4.98);
		int _indexHumidity = std::min(static_cast<int>(humidity[i] * 4.98), _indexTemperature);
		double _biomeType = BIOME_TABLE[_indexHumidity][_indexTemperature];
		int _biomeInt = static_cast<int>(_biomeType);
		biomeArray[i] = static_cast<BIOME>(_biomeInt);
	}
}