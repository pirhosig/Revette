#pragma once
#include <array>
#include <cstdint>
#include <memory>
#include <queue>

#include <boost/container/vector.hpp>
#include <FastNoise/FastNoise.h>

#include "Core/RevetteCore.h"
#include "World/BlockContainer.h"
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
        enum EnumType : u8 {
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

        BiomeType(BiomeType&&) = default;
        BiomeType(const BiomeType&) = default;

        u16 getFoliageThreshold() const;
        Block getSurfaceBlock() const;
    };

    class ProtoChunk1;
    class ProtoChunk2;



public:
    class NoiseSources;



public:
    static constexpr u16 SEA_LEVEL = 64U;



private:
    GenerationUnitPos2D pos;

    // These should be replaced with type-erased containers in order to leak less private state.
    std::unique_ptr<
        std::array<std::array<u16, CHUNK_AREA>, GENERATION_UNIT_WIDTH_C * GENERATION_UNIT_WIDTH_C>
    > heightData;
    std::unique_ptr<
        std::array<std::array<BiomeType, CHUNK_AREA>, GENERATION_UNIT_WIDTH_C * GENERATION_UNIT_WIDTH_C>
    > biomeData;
    std::array<
        std::queue<std::pair<u16, GenerationUnitPos2D::LocalPos>>,
        2
    > generationPassQueues;


    void generatePass1();



public:
    SurfaceGenerationUnit(GenerationUnitPos2D _pos, const NoiseSources& noiseSources);

    GenerationUnitPos2D getPosition() const;

    bool expandGeneratedArea();
};



class SurfaceGenerationUnit::NoiseSources {
    const i32 seed;
    const FastNoise::SmartNode<> noiseHeight;
    const FastNoise::SmartNode<> noiseHumidity;
    const FastNoise::SmartNode<> noiseTemperature;

public:
    NoiseSources(
        i32 _seed,
		const char* noiseEncodingHeight,
		const char* noiseEncodingHumidity,
		const char* noiseEncodingTemperature
    );

    NoiseSources(NoiseSources&&) = delete;
    NoiseSources(const NoiseSources&) = delete;
    NoiseSources operator=(NoiseSources&&) = delete;
    NoiseSources operator=(const NoiseSources&) = delete;

    std::pair<u16, u16> genChunkHeight(ChunkPos2D chunkPos, std::array<u16, CHUNK_AREA>& output) const;
    void genChunkBiomes(ChunkPos2D chunkPos, std::array<BiomeType, CHUNK_AREA>& output) const;
};
