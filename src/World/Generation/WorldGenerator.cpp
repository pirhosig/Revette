#include "WorldGenerator.h"



// TODO: load these values from settings
constexpr int SEED = 24383737;
const char* const NOISE_HEIGHTMAP = "FQkXCRUJDQAH@BCGZmBkAJBg@AIBEBAOamRk/C83MTD0EAg8JBg@AIBFBAOamRk/DAMAAKBBBAMAAEBBBA==";
const char* const NOISE_HUMIDITY = "KQkNCQY@CRRQ=";
const char* const NOISE_TEMPERATURE = "KQkNCQY@CRRQ=";
WorldGenerator::WorldGenerator(
    GlobalApplicationState& _globalApplicationState,
    const Settings& _settings
) :
    globalApplicationState{_globalApplicationState},
    settings{_settings},
    surfaceNoiseSources(
        SEED,
        NOISE_HEIGHTMAP,
        NOISE_HUMIDITY,
        NOISE_TEMPERATURE
    )
{}



/*
Run the generation thread.
*/
void WorldGenerator::run() {

    while (globalApplicationState.applicationShouldTerminate.load() == false) {
        

        // We won't need to generate anything else until the player moves across a chunk boundary.
        globalApplicationState.playerChunkPosition.wait(globalApplicationState.playerChunkPosition);
    }
}
