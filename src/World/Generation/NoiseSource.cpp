#include "NoiseSource.h"

#include <stdexcept>



namespace {

consteval std::array<float, CHUNK_AREA> getPositionArrayX() {
    std::array<float, CHUNK_AREA> positionArray;

    for (u32 x = 0; x < CHUNK_SIZE; ++x) {
        for (u32 y = 0; y < CHUNK_SIZE; ++y) {
            positionArray[y * CHUNK_SIZE + x] = static_cast<float>(x);
        }
    }

    return positionArray;
}

consteval std::array<float, CHUNK_AREA> getPositionArrayY() {
    std::array<float, CHUNK_AREA> positionArray;

    for (u32 x = 0; x < CHUNK_SIZE; ++x) {
        for (u32 y = 0; y < CHUNK_SIZE; ++y) {
            positionArray[y * CHUNK_SIZE + x] = static_cast<float>(y);
        }
    }
    
    return positionArray;
}

}



NoiseSource2D::NoiseSource2D(const char* noiseSetting, int _seed) :
	generator(FastNoise::NewFromEncodedNodeTree(noiseSetting)),
	seed{ _seed }
{
	if (generator.get() == nullptr) {
		throw std::runtime_error("Failed to initialise NoiseSource2D");
	}
}



std::array<float, CHUNK_AREA> NoiseSource2D::genChunkNoise(ChunkPos2D chunkPos) const {
	static constexpr auto positionsX = getPositionArrayX();
	static constexpr auto positionsY = getPositionArrayY();

	std::array<float, CHUNK_AREA> noise;
	generator->GenPositionArray2D(
		noise.data(),
		CHUNK_AREA,
		positionsX.data(),
		positionsY.data(),
		chunkPos.getX() * CHUNK_SIZE,
		chunkPos.getZ() * CHUNK_SIZE,
		seed
	);
	return noise;
}
