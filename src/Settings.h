#pragma once
#include "Core/RevetteCore.h"



class Settings {
private:
    i32 loadDistanceHorizontal;
    i32 loadDistanceVertical;
    i32 generationSeed;

    bool validationLayersEnabled;

public:
    Settings();
    Settings(Settings&&) = delete;
    Settings(const Settings&) = delete;
    Settings operator=(Settings&&) = delete;
    Settings operator=(const Settings&) = delete;

    
    i32 getLoadDistanceHorizontal() const;
    i32 getLoadDistanceVertical() const;
    i32 getGenerationSeed() const;

    bool getValidationLayersEnabled() const;
};
