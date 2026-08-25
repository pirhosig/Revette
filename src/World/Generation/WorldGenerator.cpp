#include "WorldGenerator.h"

#include <algorithm>

#include "GlobalLog.h"



// TODO: load these values from settings
const char* const NOISE_HEIGHTMAP =
    "FQkXCRUJDQAH@BCGZmBkAJBg@AIBEBAOamRk/C83MTD0EAg8JBg@AIBFBAOamRk/DAMAAKBBBAMAAEBBBA==";
const char* const NOISE_HUMIDITY = "KQkNCQY@CRRQ=";
const char* const NOISE_TEMPERATURE = "KQkNCQY@CRRQ=";
constexpr i32 GENERATION_UNIT_LOAD_DIST = 2;
constexpr i32 MAX_GENERATION_UNIT_LOAD_DIST = 3;



WorldGenerator::WorldGenerator(
    GlobalApplicationState& _globalApplicationState,
    const Settings& _settings
) :
    globalApplicationState{_globalApplicationState},
    settings{_settings},
    surfaceNoiseSources(
        settings.getGenerationSeed(),
        NOISE_HEIGHTMAP,
        NOISE_HUMIDITY,
        NOISE_TEMPERATURE
    ),
    playerChunkPosition(0, 0, 0),
    generationIsComplete{false}
{
    GlobalLog.Write("Created world generator.");
}



/*
Run the generation thread.
*/
void WorldGenerator::run() {
    while (globalApplicationState.applicationShouldTerminate.load() == false) {
        do {
            const auto newPlayerChunkPosition = globalApplicationState.playerChunkPosition.load();
            if (playerChunkPosition != newPlayerChunkPosition) {
                playerChunkPosition = newPlayerChunkPosition;
                onLoadCentreChange();
            }
        } while (expandGeneratedArea());

        // We won't need to generate anything else until the player moves across a chunk boundary.
        globalApplicationState.playerChunkPosition.wait(playerChunkPosition);
    }
}



/*
Increase the generated area by a radius of one.
*/
bool WorldGenerator::expandGeneratedArea() {
    // TODO

    
}



void WorldGenerator::onLoadCentreChange() {
    const GenerationUnitPos2D playerGenerationUnitPos{ChunkPos2D{playerChunkPosition}};

    constexpr auto gridWidth = MAX_GENERATION_UNIT_LOAD_DIST * 2 + 1;
    std::array<std::array<bool, gridWidth>, gridWidth> visitedGrid{};

    // Iterate over existing generation units and remove the ones which are no longer needed.
    std::erase_if(
        surfaceGenerationUnits,
        [playerGenerationUnitPos](
            decltype(surfaceGenerationUnits)::const_reference unit
        ) -> bool {
            return GENERATION_UNIT_LOAD_DIST < playerGenerationUnitPos.getManhattanDistance(unit->getPosition());
        }
    );

    // Iterate over existing generation units and mark them as visited.
    for (const auto& unit : surfaceGenerationUnits) {
        const auto offset = playerGenerationUnitPos.getOffsetTo(unit->getPosition());
        const auto xPos = static_cast<size_t>(offset.getX() + gridWidth);
        const auto zPos = static_cast<size_t>(offset.getZ() + gridWidth);
        visitedGrid[xPos][zPos] = true;
    }
    
    // Iterate over all generation unit positions, and create the ones that don't exist.
    // We could probably do this better, but this works for now.
    for (i32 dX = -GENERATION_UNIT_LOAD_DIST; dX <= GENERATION_UNIT_LOAD_DIST; ++dX) {
    for (i32 dZ = -GENERATION_UNIT_LOAD_DIST; dZ <= GENERATION_UNIT_LOAD_DIST; ++dZ) {
        if (std::abs(dX) + std::abs(dZ) > GENERATION_UNIT_LOAD_DIST) {
            continue;
        }
        surfaceGenerationUnits.emplace_back(
            std::make_unique<SurfaceGenerationUnit>(
                playerGenerationUnitPos.calculateOffsetBy(dX, dZ),
                surfaceNoiseSources
            )
        );
    }
    }
}
