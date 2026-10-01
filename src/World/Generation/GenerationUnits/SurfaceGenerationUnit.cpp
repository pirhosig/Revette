#include "SurfaceGenerationUnit.h"
#include <limits>
#include <stdexcept>

#include "World/BlockContainer.h"



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

constexpr u8 BIOME_TABLE[16][16] = {
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
    heightData{std::make_unique<decltype(heightData)::element_type>()},
    biomeData{std::make_unique<decltype(biomeData)::element_type>()}
{
    // It might be a good idea to move this outside of the constructor.
    for (u16 x = 0; x < GENERATION_UNIT_WIDTH_C; ++x) {
    for (u16 z = 0; z < GENERATION_UNIT_WIDTH_C; ++z) {
        GenerationUnitPos2D::LocalPos genPos{x, z};
        noiseSources.genChunkHeight(pos.asChunkPos2D(genPos), (*heightData)[genPos.asIndex()]);
    }
    }

    for (u16 x = 0; x < GENERATION_UNIT_WIDTH_C; ++x) {
    for (u16 z = 0; z < GENERATION_UNIT_WIDTH_C; ++z) {
        GenerationUnitPos2D::LocalPos genPos{x, z};
        noiseSources.genChunkBiomes(pos.asChunkPos2D(genPos), (*biomeData)[genPos.asIndex()]);
    }
    }
}



GenerationUnitPos2D SurfaceGenerationUnit::getPosition() const {
    return pos;
}



bool SurfaceGenerationUnit::expandGeneratedArea() {
    if (generationPassQueues[0].size() > 0) {
        u16 currentPhase = generationPassQueues[0].front().first;
        while (generationPassQueues[0].size() > 0) {
            auto [_phase, _pos] = generationPassQueues[0].front();
            if (_phase > currentPhase) {
                break;
            }
            generationPassQueues[0].pop();

            // TODO chunk generation pass 1

            // After this stage a "ProtoChunk" needs to contain both the shaped terrain
            // as well as the pre-emptively placed local features.
        }
    }

    // Could possibly have types ProtoChunk1 --> ProtoChunk2 --> ...

    
    /*
    Here be the plan:
        1. Raw noise generation
        2. Noise modifiers: these are localised to a generation unit.
        3. Structure space reservation - 

        Local scale:

        4. Terrain shape - combine modified noise and structure placement to form the basic terrain shape.

        This does not modify the block data:
        5. Local features: small local features (e.g. plants) determination.

        6. Chunk assembly: create the full assembled chunk from the shaped terrain and local features.
    */ 

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
u16 SurfaceGenerationUnit::BiomeType::getFoliageThreshold() const {
    switch (enumVal) {
        case DESERT_WARM:
            return 65391U;
        case DESERT_WARM_DEEP:
            return std::numeric_limits<u16>::max();
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
    return std::numeric_limits<u16>::max();
}



Block SurfaceGenerationUnit::BiomeType::getSurfaceBlock() const {
    switch (enumVal) {
        case DESERT_WARM:
            return Block(5);
        case DESERT_WARM_DEEP:
            return Block(17);
        case FOREST_BOREAL:
            return Block(11);
        case FOREST_TEMPERATE:
            return Block(1);
        case RAINFOREST:
            return Block(8);
        case SAVANNAH:
            return Block(16);
        case SHRUBLAND:
            return Block(16);
        case TUNDRA:
            return Block(7);
    }

    // Just in case.
    return Block(2);
}



/*
Represents a chunk after the first generation step.
At this point the raw noise date has been calculated.
*/
class SurfaceGenerationUnit::ProtoChunk1 {
    BlockContainer blockContainer;
    ChunkPos pos;

public:
    ProtoChunk1(
        ChunkPos _pos,
        const std::array<u16, CHUNK_AREA>& heightData,
        const std::array<BiomeType, CHUNK_AREA>& biomeData
    );
};



/*
Represents a chunk after the first generation step.
At this point the raw noise date has been calculated.
*/
class SurfaceGenerationUnit::ProtoChunk2 {
    BlockContainer blockContainer;
    ChunkPos pos;

public:
    
};



SurfaceGenerationUnit::ProtoChunk1::ProtoChunk1(
    ChunkPos _pos,
    const std::array<u16, CHUNK_AREA>& heightData,
    const std::array<BiomeType, CHUNK_AREA>& biomeData
) :
    pos{_pos}
{
    const i32 _bottom = pos.getY() * CHUNK_SIZE;
	const i32 _top = _bottom + CHUNK_SIZE - 1;
    // Return if all of the chunk falls above the terrain height
    
    // Fill the chunk if all of the chunk falls below the terrain height
	// This code is sort of horrible, but it runs hella fast compared to what was here before
	// Nvm this code is now even faster, and also looks okay
    

    

    blockContainer.setSizeByte();
    const auto _blockStone = blockContainer.getOrAddPalleteIndex(Block(2));

    for (u16 lX = 0; lX < CHUNK_SIZE; ++lX) {
    for (u16 lZ = 0; lZ < CHUNK_SIZE; ++lZ) {
        // TODO: switch to relative surface height
        const i32 _surface = heightData[lZ * CHUNK_SIZE + lX];

        u16 lY = 0;


        // Fill up subsurface.
        for (; lY < CHUNK_SIZE && _bottom + lY < _surface; ++lY) {
            blockContainer.setBlockRaw(
                ChunkLocalBlockPos(lX, lY, lZ).asIndex(),
                _blockStone
            );
        }
        if (lY == CHUNK_SIZE) {
            continue;
        }

        // Surface
        
        
        if (_bottom + lY < SEA_LEVEL) {
            blockContainer.setBlock(
                ChunkLocalBlockPos(lX, lY, lZ),
                Block(2)
            );
        }
        else {
            blockContainer.setBlock(
                ChunkLocalBlockPos(lX, lY, lZ),
                biomeData[lZ * CHUNK_SIZE + lX].getSurfaceBlock()
            );
        }

        lY++;
    }
    }
}



SurfaceGenerationUnit::NoiseSources::NoiseSources(
    i32 _seed,
    const char* noiseEncodingHeight,
    const char* noiseEncodingHumidity,
    const char* noiseEncodingTemperature
) : 
    seed{_seed},
    noiseHeight{createHeightNoise(noiseEncodingHeight)},
    noiseHumidity{createHumidityNoise(noiseEncodingHumidity)},
    noiseTemperature{createTemperatureNoise(noiseEncodingTemperature)}
{}



std::pair<u16, u16> SurfaceGenerationUnit::NoiseSources::genChunkHeight(
    ChunkPos2D chunkPos,
    std::array<u16, CHUNK_AREA>& output
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
        output[i] = static_cast<u16>(rawNoise[i]);
    }

    return {
        static_cast<u16>(outputMinMax.min),
        static_cast<u16>(outputMinMax.max)
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
