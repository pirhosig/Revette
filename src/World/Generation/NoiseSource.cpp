#include "NoiseSource.h"



std::array<float, CHUNK_AREA> NoiseSource2D::GenChunkNoise(ChunkPos chunkPos) const
{
	std::array<float, CHUNK_AREA> noise;
	generator->GenUniformGrid2D(noise.data(), chunkPos.x * CHUNK_SIZE, chunkPos.z * CHUNK_SIZE, CHUNK_SIZE, CHUNK_SIZE, frequency, seed);
	return noise;
}