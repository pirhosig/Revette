#pragma once
#include <cstdint>
#include <string>



class Settings {
private:
    uint32_t loadDistanceHorizontal;
    uint32_t loadDistanceVertical;

    bool validationLayersEnabled;

public:
    Settings();
    Settings(Settings&&) = delete;
    Settings(const Settings&) = delete;
    Settings operator=(Settings&&) = delete;
    Settings operator=(const Settings&) = delete;

    uint32_t getLoadDistanceHorizontal() const;
    uint32_t getLoadDistanceVertical() const;
    bool getValidationLayersEnabled() const;
};
