#include "NoiseSource.h"

#include <stdexcept>



std::array<float, CHUNK_AREA> NoiseSource2D::genChunkNoise(ChunkPos2D chunkPos) const
{
	std::array<float, CHUNK_AREA> noise;
	generator->GenUniformGrid2D(
		noise.data(),
		chunkPos.getX() * CHUNK_SIZE,
		chunkPos.getZ() * CHUNK_SIZE,
		CHUNK_SIZE,
		CHUNK_SIZE,
		1.0f,
		1.0f,
		seed
	);
	return noise;
}
