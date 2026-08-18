#pragma once
#include <array>
#include <cstdint>
#include <memory>

#include <FastNoise/FastNoise.h>

#include "Core/RevetteCore.h"
#include "GenerationUnitPos2D.h"





/*
The idea behind the surface generation unit is to have a larger subdivision of space than a chunk, in order to produce
better generation on a larger scale. A surface generation unit will produce a height map and biome map, as well as a
list of structure locations. This data can then be used to produce individual chunks.

The placement of structures is decided by a priority based space reservation scheme. That is, initially a list of
structure bounding boxes is produced, from which two overlapping bounding boxes will result in one structure being
removed based on which structure has the lower priority (ties are broken arbitrarily, but deterministically). This
results in a disjoint set of structures.
*/
class SurfaceGenerationUnit {
private:
    class BiomeType {
    public:
        enum EnumType : uint8_t {
            DESERT_WARM,
            DESERT_WARM_DEEP,
            FOREST_BOREAL,
            FOREST_TEMPERATE,
            RAINFOREST,
            SAVANNAH,
            SHRUBLAND,
            TUNDRA
        };

    private:
        EnumType enumVal;

    public:
        BiomeType();
        BiomeType(EnumType _enumVal);

        uint16_t getFoliageThreshold() const;
    };

public:
    class NoiseSources;

private:
    std::unique_ptr<std::array<std::array<uint16_t, CHUNK_AREA>, GENERATION_UNIT_WIDTH_C * GENERATION_UNIT_WIDTH_C>> heightData;

public:
    static constexpr uint16_t SEA_LEVEL = 64U;

    SurfaceGenerationUnit(GenerationUnitPos2D pos, const NoiseSources& noiseSources);
};



class SurfaceGenerationUnit::NoiseSources {
    const int32_t seed;
    const FastNoise::SmartNode<> noiseHeight;
    const FastNoise::SmartNode<> noiseHumidity;
    const FastNoise::SmartNode<> noiseTemperature;

public:
    NoiseSources(
        int32_t _seed,
		const char* noiseEncodingHeight,
		const char* noiseEncodingHumidity,
		const char* noiseEncodingTemperature
    );

    NoiseSources(NoiseSources&&) = delete;
    NoiseSources(const NoiseSources&) = delete;
    NoiseSources operator=(NoiseSources&&) = delete;
    NoiseSources operator=(const NoiseSources&) = delete;

    std::pair<uint16_t, uint16_t> genChunkHeight(ChunkPos2D chunkPos, std::array<uint16_t, CHUNK_AREA>& output) const;
    void genChunkBiomes(ChunkPos2D chunkPos, std::array<BiomeType, CHUNK_AREA>& output) const;
};
