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

struct PendingBlock {
    ChunkLocalBlockPos pos;
    u16 age;
    Block block;
};

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
Represents a chunk column in the surface generation unit after the first generation step.
At this point the raw noise data has been calculated.
*/
class SurfaceGenerationUnit::SurfaceNoise {
public:
    std::array<u16, CHUNK_AREA> height;
    std::array<BiomeType, CHUNK_AREA> biomes;
    u16 heightMin;
    u16 heightMax;

    
    SurfaceNoise(ChunkPos2D _pos, const NoiseSources& noise);
    SurfaceNoise(SurfaceNoise&&) = default;
    SurfaceNoise& operator=(SurfaceNoise&&) = default;
    
    SurfaceNoise(const SurfaceNoise&) = delete;
    SurfaceNoise& operator=(const SurfaceNoise&) = delete;
};



SurfaceGenerationUnit::SurfaceNoise::SurfaceNoise(ChunkPos2D _pos, const NoiseSources& noise) {
    auto [_heightMin, _heightMax] = noise.genChunkHeight(_pos, height);
    heightMin = _heightMin;
    heightMax = _heightMax;
    noise.genChunkBiomes(_pos, biomes);
}



/*
Represents a chunk after the second generation step, once the base terrain shape has been calculated.
*/
class SurfaceGenerationUnit::ProtoChunk {
public:
    BlockContainer blockContainer;
    ChunkPos pos;

    ProtoChunk(ChunkPos _pos, SurfaceNoise& protoChunkNoise);
    rvl::vector32<PendingBlock> generateMinorFeatures();
};



SurfaceGenerationUnit::ProtoChunk::ProtoChunk(ChunkPos _pos, SurfaceNoise& protoChunkNoise) :
    pos{_pos}
{
    const i32 _lowY = pos.getY() * CHUNK_SIZE;
    const i32 _uppY = _lowY + CHUNK_SIZE - 1;

    // Return if all of the chunk falls above the terrain height
    if (protoChunkNoise.heightMax + 1 < _lowY && SEA_LEVEL < _lowY) {
        return;
    }

    // Fill the chunk if all of the chunk falls below the terrain height
    if (_uppY < protoChunkNoise.heightMin) {
        blockContainer.setSingleBlock(Block(2));
        return;
    }

    // This code is sort of horrible, but it runs hella fast compared to what was here before
    // Nvm this code is now even faster, and also looks okay

    // Some blocks must be placed beyond this point, so this optimisation is valid
    blockContainer.setSizeByte();
    const auto _blockStone = blockContainer.getOrAddPalleteIndex(Block(2));

    for (u16 lX = 0; lX < CHUNK_SIZE; ++lX) {
    for (u16 lZ = 0; lZ < CHUNK_SIZE; ++lZ) {
        const ChunkPos2D::LocalPos _columnPos{lX, lZ};

        // TODO: switch to relative surface height ... Done?
        const i32 _surface = protoChunkNoise.height[_columnPos.asIndex()] - pos.getY() * CHUNK_SIZE;

        u16 lY = 0;

        // Fill up subsurface.
        for (; lY < CHUNK_SIZE && _lowY + lY < _surface; ++lY) {
            blockContainer.setBlockRaw(
                ChunkLocalBlockPos(lX, lY, lZ).asIndex(),
                _blockStone
            );
        }
        if (lY == CHUNK_SIZE) {
            continue;
        }

        // Surface

        // Surface above shoreline.
        if (SEA_LEVEL < _lowY + lY) {
            blockContainer.setBlock(
                ChunkLocalBlockPos(lX, lY, lZ),
                protoChunkNoise.biomes[_columnPos.asIndex()].getSurfaceBlock()
            );
            ++lY;
        }
        // Shore or sea.
        else {
            blockContainer.setBlock(ChunkLocalBlockPos(lX, lY, lZ), Block(5));
            ++lY;

            // Fill rest with water.
            for (; lY < CHUNK_SIZE && _lowY + lY <= SEA_LEVEL; ++lY) {
                blockContainer.setBlock(ChunkLocalBlockPos(lX, lY, lZ), Block(6));
            }
        }
    }
    }
}



rvl::vector32<PendingBlock> SurfaceGenerationUnit::ProtoChunk::generateMinorFeatures() {
    // Create population features
	// Return if chunk is entirely below surface or if all the surface air blocks are also below this chunk
	if (_chunkTop <= genParameters.heightMap.heightMin || genParameters.heightMap.heightMax + 1 < _chunkBottom) return;

	// LCG as the PRNG for population: this may or may not work out in the end
	// I have no idea tbh, I am not the stats person
	// Update: it somehow works
	ChunkPRNG prng(position);

	i32 _worldPosX = position.getX() * CHUNK_SIZE;
	i32 _worldPosZ = position.getZ() * CHUNK_SIZE;

	// Ruin placement code. Ruins are a general class of structures that can be placed in funky ways
	// Try to place a ruin at a random poi32 in the chunk
	{
		uint16_t _randpos = prng.raw();
		i32 pX = _randpos % CHUNK_SIZE;
		i32 pZ = (_randpos / CHUNK_SIZE) % CHUNK_SIZE;
		const auto _idx = static_cast<size_t>(pZ * CHUNK_SIZE + pX);
		i32 _ground = genParameters.heightMap.heightArray[_idx];

		switch (genParameters.biomeMap.biomeArray[_idx])
		{
		case BIOME::DESERT:
			if (_ground > -10 && prng.raw() > 65163) Structures::Ruins::clayFrame(
				*this, prng, BlockPos(_worldPosX + pX, _ground, _worldPosZ + pZ)
			);
		default:
			break;
		}
	}


	for (i32 lX = 0; lX < CHUNK_SIZE; ++lX) {
	for (i32 lZ = 0; lZ < CHUNK_SIZE; ++lZ) {
		const auto _index = static_cast<size_t>(lZ * CHUNK_SIZE + lX);
		const i32 _surfaceLevel = genParameters.heightMap.heightArray[_index];

		// Continue if surface air block is below chunk OR if the topmost block is above the chunk
		// Currently no features generate below sea level, so this is sort of a hack until those features exist
		if (
			_surfaceLevel + 1 < _chunkBottom ||
			_surfaceLevel + 1 > _chunkTop ||
			_surfaceLevel < SEA_LEVEL + 3
		) {
			continue;
		}

		// Variables defined for quick access
		const BlockPos _centre(_worldPosX + lX, _surfaceLevel + 1, _worldPosZ + lZ);
		const uint16_t _foliageValue = prng.raw();

		switch (genParameters.biomeMap.biomeArray[_index]) {
		case BIOME::DESERT:
			// Cactus
			if (_foliageValue > 65439)
			{
				auto height = prng.scaledInt(1.13, 2.87);
				for (i32 i = 0; i < height; ++i) setBlockPopulation(_centre.offset(0, i, 0), Block(9), 0);
			}
			// Desert Flower
			else if (_foliageValue > 65394) setBlockPopulation(_centre, Block(14), 0);
			else if (_foliageValue > 65391) setBlockPopulation(_centre, Block(10), 0);
			break;
		
		case BIOME::DESERT_DEEP:
			break;
		
		case BIOME::FOREST_BOREAL:
			if      (_foliageValue > 65358) Structures::Trees::PineBasic(*this, prng, _centre);
			else if (_foliageValue > 65325) Structures::Trees::PineMassive(*this, prng, _centre);
			else if (_foliageValue > 65161) Structures::Trees::PineFancy(*this, prng, _centre);
			break;
		
		case BIOME::FOREST_TEMPERATE:
			if      (_foliageValue > 64434) Structures::Trees::Oak(*this, prng, _centre);
			else if (_foliageValue > 64342) Structures::Trees::Aspen(*this, prng, _centre);
			else if (_foliageValue > 64093) setBlockPopulation(_centre, Block(15), 0);
			else if (_foliageValue > 63988) setBlockPopulation(_centre, Block(10), 0);
			break;
		
		case BIOME::RAINFOREST:
			// Rainforests are pretty bland like this ngl (slightly better now)
			if      (_foliageValue > 60272) Structures::Trees::RainforestBasic(*this, prng, _centre);
			else if (_foliageValue > 59989) Structures::Trees::RainforestTall(*this, prng, _centre);
			else if (_foliageValue > 49596) Structures::Trees::RainforestShrub(*this, _centre);
			else if (_foliageValue > 37063) setBlockPopulation(_centre, Block(10), 0);
			break;
		
		case BIOME::SAVANNAH:
			if      (_foliageValue > 65530) Structures::Trees::SavannahBaobab(*this, prng, _centre);
			else if (_foliageValue > 65423) Structures::Trees::SavannahAcacia(*this, prng, _centre);
			else if (_foliageValue > 52428) setBlockPopulation(_centre, Block(10), 0);
			break;
		
		case BIOME::SHRUBLAND:
			if      (_foliageValue > 52428) setBlockPopulation(_centre, Block(10), 0);
			else if (_foliageValue > 52369) setBlockPopulation(_centre, Block(4), 0);
			break;
		
		case BIOME::TUNDRA:
			if (_foliageValue > 65430) setBlockPopulation(_centre, Block(2), 0);
			break;
		
		default:
			break;
		}
	}
	}
};



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



// Returns a pair of (min, max) specifying the range of the height within this chunk.
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



SurfaceGenerationUnit::SurfaceGenerationUnit(GenerationUnitPos2D _pos, const NoiseSources& noiseSources) :
    pos{_pos}
{
    // It might be a good idea to move this outside of the constructor.
    surfaceNoises.reserve(GENERATION_UNIT_WIDTH_C * GENERATION_UNIT_WIDTH_C);
    for (u16 x = 0; x < GENERATION_UNIT_WIDTH_C; ++x) {
    for (u16 z = 0; z < GENERATION_UNIT_WIDTH_C; ++z) {
        GenerationUnitPos2D::LocalPos genPos{x, z};
        surfaceNoises.emplace_back(pos.asChunkPos2D(genPos), noiseSources);
    }
    }
}



SurfaceGenerationUnit::~SurfaceGenerationUnit() = default;



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
