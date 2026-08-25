#pragma once
#include <memory>
#include <vector>
#include "Settings.h"
#include "Application/GlobalApplicationState.h"
#include "GenerationUnits/SurfaceGenerationUnit.h"


/*
TODO: need to create pipe to world and renderer.
*/
class WorldGenerator {
    GlobalApplicationState& globalApplicationState;
    const Settings& settings;

    SurfaceGenerationUnit::NoiseSources surfaceNoiseSources;
    std::vector<std::unique_ptr<SurfaceGenerationUnit>> surfaceGenerationUnits;

    ChunkPos playerChunkPosition;
    bool generationIsComplete;

public:
    WorldGenerator(
        GlobalApplicationState& _globalApplicationState,
        const Settings& _settings
    );
    
    void run();



private:
    // Expand the generated area, and returns true if further generation is needed.
    bool expandGeneratedArea();
    void onLoadCentreChange();
};
