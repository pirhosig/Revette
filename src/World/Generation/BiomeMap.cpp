#include "BiomeMap.h"
#include <algorithm>
#include <cmath>
#include "NoiseSource.h"



int BIOME_TABLE[16][16] = {
	{  5,  5,  5,  5,  5,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0 },
	{ {},  5,  5,  5,  5,  1,  1,  0,  0,  0,  0,  0,  0,  0,  0,  0 },
	{ {}, {},  5,  1,  1,  1,  1,  1,  0,  0,  0,  0,  0,  0,  0,  0 },
	{ {}, {}, {},  1,  1,  1,  1,  2,  4,  4,  4,  4,  4,  4,  4,  4 },
	{ {}, {}, {}, {},  1,  1,  2,  2,  2,  2,  4,  4,  4,  4,  4,  4 },
	{ {}, {}, {}, {}, {},  1,  2,  2,  2,  2,  2,  4,  4,  4,  4,  4 },
	{ {}, {}, {}, {}, {}, {},  2,  2,  2,  2,  2,  2,  4,  4,  4,  4 },
	{ {}, {}, {}, {}, {}, {}, {},  2,  2,  2,  2,  2,  2,  4,  4,  4 },
	{ {}, {}, {}, {}, {}, {}, {}, {},  2,  3,  3,  3,  3,  3,  3,  3 },
	{ {}, {}, {}, {}, {}, {}, {}, {}, {},  3,  3,  3,  3,  3,  3,  3 },
	{ {}, {}, {}, {}, {}, {}, {}, {}, {}, {},  3,  3,  3,  3,  3,  3 },
	{ {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {},  3,  3,  3,  3,  3 },
	{ {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {},  3,  3,  3,  3 },
	{ {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {},  3,  3,  3 },
	{ {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {},  3,  3 },
	{ {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {},  3 }
};


constexpr float LOWER_NOISE_CUTOFF = 0.35f;
constexpr float UPPER_NOISE_CUTOFF = 1.0f - LOWER_NOISE_CUTOFF;



// Randomizes the noise at biome edges
inline int distributeEdges(float value, int index, const std::array<float, CHUNK_AREA>& edges)
{
	float integer;
	float decimal = modf(value, &integer);
	value += 0.5f;
	if (decimal < LOWER_NOISE_CUTOFF || decimal > UPPER_NOISE_CUTOFF) return static_cast<int>(value);
	return static_cast<int>(value + edges[index]);
}



BiomeMap::BiomeMap(ChunkPos2D noisePos, NoiseSource2D& noiseTemperature, NoiseSource2D& noiseHumidity, NoiseSource2D& noiseBiomeEdge) : biomeArray{}
{
	std::array<float, CHUNK_AREA> temperature = noiseTemperature.GenChunkNoise(noisePos);
	std::array<float, CHUNK_AREA> humidity = noiseHumidity.GenChunkNoise(noisePos);
	std::array<float, CHUNK_AREA> edges = noiseBiomeEdge.GenChunkNoise(noisePos);

	for (int lZ = 0; lZ < CHUNK_SIZE; ++lZ)
	{
		for (int lX = 0; lX < CHUNK_SIZE; ++lX)
		{
			int _index = lZ * CHUNK_SIZE + lX;
			int _dist = noisePos.x * CHUNK_SIZE + lX;
			temperature[_index] = std::max(0.0f, temperature[_index] - (std::abs(static_cast<float>(_dist)) / static_cast<float>(WORLD_RADIUS_BLOCK)));
		}
	}

	for (int i = 0; i < CHUNK_AREA; ++i)
	{
		int _indexTemperature = distributeEdges(temperature[i] * 16.0f, i, edges);
		int _indexHumidity = std::min(distributeEdges(humidity[i] * 16.0f, i, edges), _indexTemperature);
		double _biomeType = BIOME_TABLE[_indexHumidity][_indexTemperature];
		int _biomeInt = static_cast<int>(_biomeType);
		biomeArray[i] = static_cast<BIOME>(_biomeInt);
	}
}