#include "SurfaceGenerationUnit.h"
#include <limits>
#include <stdexcept>



namespace {

FastNoise::SmartNode<> createNoiseSourceFromEncoding(const char* encodedGenerator) {
    auto noiseSource = FastNoise::NewFromEncodedNodeTree(encodedGenerator);
    if (noiseSource.get() == nullptr) {
        throw std::runtime_error(
            "Failed to create noise source from encoded string. This likely indicates an invalid string"
        );
    }
    return noiseSource;
}

FastNoise::SmartNode<> createHeightNoise(const char* encodedGenerator) {
    auto baseGenerator = createNoiseSourceFromEncoding(encodedGenerator);

    auto shiftedGenerator = FastNoise::New<FastNoise::Add>();
    shiftedGenerator->SetLHS(std::move(baseGenerator));
    shiftedGenerator->SetRHS(static_cast<float>(SurfaceGenerationUnit::SEA_LEVEL));

    return shiftedGenerator;
}

FastNoise::SmartNode<> createHumidityNoise(const char* encodedGenerator) {
    auto baseGenerator = createNoiseSourceFromEncoding(encodedGenerator);

    auto remappedGenerator = FastNoise::New<FastNoise::Remap>();
    remappedGenerator->SetSource(std::move(baseGenerator));
    // Due to size of biome look up table (16 x 16).
    remappedGenerator->SetToMax(15.0f);
    remappedGenerator->SetClampOutput(true);

    return remappedGenerator;
}

FastNoise::SmartNode<> createTemperatureNoise(const char* encodedGenerator) {
    auto baseGenerator = createNoiseSourceFromEncoding(encodedGenerator);

    auto shiftedGenerator = FastNoise::New<FastNoise::Add>();
    shiftedGenerator->SetLHS(std::move(baseGenerator));
    shiftedGenerator->SetRHS(0.4f);

    auto xDistance = FastNoise::New<FastNoise::Gradient>();
    // The initial noise is in the range -1.0 to 1.0 (roughly) so the gradient ensures that temperature
    // will reach 0 at the edge of the world
    xDistance->SetMultiplier<FastNoise::Dim::X>(2.0f / static_cast<float>(WORLD_RADIUS_BLOCK));

    auto latitudeBias = FastNoise::New<FastNoise::Abs>();
    latitudeBias->SetSource(std::move(xDistance));

    auto biasedGenerator = FastNoise::New<FastNoise::Subtract>();
    biasedGenerator->SetLHS(std::move(shiftedGenerator));
    biasedGenerator->SetRHS(std::move(latitudeBias));

    auto remappedGenerator = FastNoise::New<FastNoise::Remap>();
    remappedGenerator->SetSource(std::move(biasedGenerator));
    // Due to size of biome look up table (16 x 16).
    remappedGenerator->SetToMax(15.0f);
    remappedGenerator->SetClampOutput(true);

    return remappedGenerator;
}

consteval std::array<float, CHUNK_AREA> getPositionArrayX() {
    std::array<float, CHUNK_AREA> positionArray;

    for (u32 x = 0; x < CHUNK_SIZE; ++x) {
        for (u32 y = 0; y < CHUNK_SIZE; ++y) {
            positionArray[x * CHUNK_SIZE + y] = static_cast<float>(x);
        }
    }

    return positionArray;
}
constexpr auto positionsX = getPositionArrayX();

consteval std::array<float, CHUNK_AREA> getPositionArrayY() {
    std::array<float, CHUNK_AREA> positionArray;

    for (u32 x = 0; x < CHUNK_SIZE; ++x) {
        for (u32 y = 0; y < CHUNK_SIZE; ++y) {
            positionArray[x * CHUNK_SIZE + y] = static_cast<float>(y);
        }
    }
    
    return positionArray;
}
constexpr auto positionsY = getPositionArrayY();

constexpr uint8_t BIOME_TABLE[16][16] = {
	{ 7,  7,  7,  7,  7,  0,  0,  0,  0,  0,  0,  1,  1,  1,  1,  1 },
	{ 7,  7,  7,  7,  7,  0,  0,  0,  0,  0,  0,  0,  0,  1,  1,  1 },
	{ 7,  7,  7,  7,  7,  2,  2,  0,  0,  0,  0,  0,  0,  0,  0,  0 },
	{ 7,  7,  7,  7,  7,  2,  2,  2,  0,  0,  0,  0,  0,  0,  0,  0 },
	{ 7,  7,  7,  7,  7,  2,  2,  2,  6,  6,  6,  6,  6,  0,  0,  5 },
	{ 7,  7,  7,  7,  7,  2,  2,  2,  6,  6,  6,  6,  6,  5,  5,  5 },
	{ 7,  7,  7,  7,  2,  2,  2,  3,  6,  6,  6,  6,  6,  5,  5,  5 },
	{ 7,  7,  7,  7,  2,  2,  2,  3,  3,  3,  6,  6,  6,  5,  5,  5 },
	{ 7,  7,  7,  2,  2,  2,  2,  3,  3,  3,  3,  3,  3,  5,  5,  5 },
	{ 7,  7,  7,  2,  2,  2,  3,  3,  3,  3,  3,  3,  3,  5,  5,  4 },
	{ 7,  7,  7,  2,  2,  2,  3,  3,  3,  3,  3,  3,  4,  4,  4,  4 },
	{ 7,  7,  7,  2,  2,  2,  3,  3,  3,  3,  3,  4,  4,  4,  4,  4 },
	{ 7,  7,  7,  2,  2,  2,  3,  3,  3,  3,  4,  4,  4,  4,  4,  4 },
	{ 7,  7,  7,  2,  2,  2,  3,  3,  3,  4,  4,  4,  4,  4,  4,  4 },
	{ 7,  7,  7,  2,  2,  2,  3,  3,  3,  4,  4,  4,  4,  4,  4,  4 },
	{ 7,  7,  7,  2,  2,  2,  3,  3,  3,  4,  4,  4,  4,  4,  4,  4 }
};

}



SurfaceGenerationUnit::SurfaceGenerationUnit(GenerationUnitPos2D _pos, const NoiseSources& noiseSources) :
    pos{_pos},
    heightData{std::make_unique<decltype(heightData)::element_type>()}
{
    for (u32 x = 0; x < GENERATION_UNIT_WIDTH_C; ++x) {
        for (u32 z = 0; z < GENERATION_UNIT_WIDTH_C; ++z) {
            noiseSources.genChunkHeight(
                ChunkPos2D(
                    pos.getX() * GENERATION_UNIT_WIDTH_C + static_cast<i32>(x),
                    pos.getZ() * GENERATION_UNIT_WIDTH_C + static_cast<i32>(z)
                ),
                (*heightData)[x * GENERATION_UNIT_WIDTH_C + z]
            );
        }
    }
}



GenerationUnitPos2D SurfaceGenerationUnit::getPosition() const {
    return pos;
}



// I don't really want to have a default, but this is an okay compromise.
SurfaceGenerationUnit::BiomeType::BiomeType() :
    enumVal{static_cast<EnumType>(0)}
{}



SurfaceGenerationUnit::BiomeType::BiomeType(SurfaceGenerationUnit::BiomeType::EnumType _enumVal) :
    enumVal{_enumVal}
{}




/*
This function returns a threshold above which a plant should be generated for a given biome. A
threshold of 2^16 - 1 indicates that no plants should ever generate.
*/
uint16_t SurfaceGenerationUnit::BiomeType::getFoliageThreshold() const {
    switch (enumVal) {
        case DESERT_WARM:
            return 65391U;
        case DESERT_WARM_DEEP:
            return std::numeric_limits<uint16_t>::max();
        case FOREST_BOREAL:
            return 65161U;
        case FOREST_TEMPERATE:
            return 63988U;
        case RAINFOREST:
            return 37063U;
        case SAVANNAH:
            return 52428U;
        case SHRUBLAND:
            return 52369U;
        case TUNDRA:
            return 65512U;
    }

    // Just in case.
    return std::numeric_limits<uint16_t>::max();
}



SurfaceGenerationUnit::NoiseSources::NoiseSources(
    int32_t _seed,
    const char* noiseEncodingHeight,
    const char* noiseEncodingHumidity,
    const char* noiseEncodingTemperature
) : 
    seed{_seed},
    noiseHeight{createHeightNoise(noiseEncodingHeight)},
    noiseHumidity{createHumidityNoise(noiseEncodingHumidity)},
    noiseTemperature{createTemperatureNoise(noiseEncodingTemperature)}
{}



std::pair<uint16_t, uint16_t> SurfaceGenerationUnit::NoiseSources::genChunkHeight(
    ChunkPos2D chunkPos,
    std::array<uint16_t, CHUNK_AREA>& output
) const {
    std::array<float, CHUNK_AREA> rawNoise;
    auto outputMinMax = noiseHeight->GenPositionArray2D(
        rawNoise.data(),
        CHUNK_AREA,
        positionsX.data(),
        positionsY.data(),
        chunkPos.getX() * CHUNK_SIZE,
        chunkPos.getZ() * CHUNK_SIZE,
        seed
    );

    for (size_t i = 0; i < CHUNK_AREA; ++i) {
        output[i] = static_cast<uint16_t>(rawNoise[i]);
    }

    return {
        static_cast<uint16_t>(outputMinMax.min),
        static_cast<uint16_t>(outputMinMax.max)
    };
}



void SurfaceGenerationUnit::NoiseSources::genChunkBiomes(
    ChunkPos2D chunkPos,
    std::array<BiomeType, CHUNK_AREA>& output
) const {
    std::array<float, CHUNK_AREA> rawHumidity;
    noiseHumidity->GenPositionArray2D(
        rawHumidity.data(),
        CHUNK_AREA,
        positionsX.data(),
        positionsY.data(),
        chunkPos.getX() * CHUNK_SIZE,
        chunkPos.getZ() * CHUNK_SIZE,
        seed + 0x63
    );

    std::array<float, CHUNK_AREA> rawTemperature;
    noiseTemperature->GenPositionArray2D(
        rawTemperature.data(),
        CHUNK_AREA,
        positionsX.data(),
        positionsY.data(),
        chunkPos.getX() * CHUNK_SIZE,
        chunkPos.getZ() * CHUNK_SIZE,
        seed + 0x57
    );

    for (size_t i = 0; i < CHUNK_AREA; ++i) {
        output[i] = static_cast<BiomeType::EnumType>(
            BIOME_TABLE[static_cast<size_t>(rawHumidity[i])][static_cast<size_t>(rawTemperature[i])]
        );
    }
}
