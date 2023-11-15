#include "BiomeMap.h"
#include <algorithm>
#include <cmath>
#include "NoiseSource.h"



int BIOME_TABLE[16][16] = {
	{ 7, 7, 7, 7, 7, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1 },
	{ 0, 7, 7, 7, 7, 2, 2, 0, 0, 0, 0, 0, 1, 1, 1, 1 },
	{ 0, 0, 7, 2, 2, 2, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0 },
	{ 0, 0, 0, 2, 2, 2, 2, 3, 6, 6, 6, 6, 0, 0, 0, 0 },
	{ 0, 0, 0, 0, 2, 2, 3, 3, 3, 3, 6, 6, 6, 5, 5, 5 },
	{ 0, 0, 0, 0, 0, 2, 3, 3, 3, 3, 3, 6, 6, 5, 5, 5 },
	{ 0, 0, 0, 0, 0, 0, 3, 3, 3, 3, 3, 3, 6, 5, 5, 5 },
	{ 0, 0, 0, 0, 0, 0, 0, 3, 3, 3, 3, 3, 3, 5, 5, 5 },
	{ 0, 0, 0, 0, 0, 0, 0, 0, 3, 4, 4, 4, 4, 4, 5, 5 },
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 4, 4, 4, 4, 4, 4 },
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 4, 4, 4, 4, 4 },
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 4, 4, 4, 4 },
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 4, 4, 4 },
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 4, 4 },
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 4 },
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4 }
};



BiomeMap::BiomeMap(ChunkPos2D noisePos, NoiseSource2D& noiseTemperature, NoiseSource2D& noiseHumidity) :
	biomeArray{}
{
	std::array<float, CHUNK_AREA> temperature = noiseTemperature.genChunkNoise(noisePos);
	std::array<float, CHUNK_AREA> humidity = noiseHumidity.genChunkNoise(noisePos);

	for (int lZ = 0; lZ < CHUNK_SIZE; ++lZ)
		for (int lX = 0; lX < CHUNK_SIZE; ++lX)
		{
			int _index = lZ * CHUNK_SIZE + lX;
			int _dist = noisePos.x * CHUNK_SIZE + lX;
			temperature[_index] = std::clamp(
				0.40f + temperature[_index] - (std::abs(_dist) / WORLD_RADIUS_BLOCK_F),
				0.0f,
				1.0f
			);
		}

	for (int i = 0; i < CHUNK_AREA; ++i)
	{
		int _indexTemperature = static_cast<int>(temperature[i] * 15.0f);
		int _indexHumidity = std::clamp(static_cast<int>(humidity[i] * _indexTemperature), 0, _indexTemperature);
		double _biomeType = BIOME_TABLE[_indexHumidity][_indexTemperature];
		int _biomeInt = static_cast<int>(_biomeType);
		biomeArray[i] = static_cast<BIOME>(_biomeInt);
	}
}