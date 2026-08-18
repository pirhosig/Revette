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
    std::vector<std::unique_ptr<SurfaceGenerationUnit>> surfaceUnits;

public:
    WorldGenerator(
        GlobalApplicationState& _globalApplicationState,
        const Settings& _settings
    );

    void run();
};
