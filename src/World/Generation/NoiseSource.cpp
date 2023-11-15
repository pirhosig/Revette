#include "NoiseSource.h"

#include <stdexcept>



std::array<float, CHUNK_AREA> NoiseSource2D::genChunkNoise(ChunkPos2D chunkPos) const
{
	std::array<float, CHUNK_AREA> noise;
	generator->GenUniformGrid2D(
		noise.data(), chunkPos.x * CHUNK_SIZE, chunkPos.z * CHUNK_SIZE, CHUNK_SIZE, CHUNK_SIZE, (1 / 64.0f), seed
	);
	return noise;
}



std::unique_ptr<float[]> NoiseSource2D::genChunkNoiseLOD(ChunkPos2D chunkPos, unsigned char level) const
{
	const int chunkVoxelSize = CHUNK_SIZE >> level;
	std::unique_ptr<float[]> noise = std::make_unique<float[]>(chunkVoxelSize * chunkVoxelSize);
	generator->GenUniformGrid2D(
		noise.get(),
		chunkPos.x * CHUNK_SIZE,
		chunkPos.z * CHUNK_SIZE,
		chunkVoxelSize,
		chunkVoxelSize,
		((1 << level) / 64.0f),
		seed
	);
	return noise;
}
