#include "Settings.h"
#include <stdexcept>
#include <simdjson.h>
#include "GlobalLog.h"



Settings::Settings() try {
    simdjson::padded_string jsonString = simdjson::padded_string::load("res/settings.json");
    simdjson::dom::parser parser;
    auto json = parser.parse(jsonString);

    loadDistanceHorizontal = static_cast<uint32_t>(json["loadDistanceHorizontal"].get_uint64());
    loadDistanceVertical = static_cast<uint32_t>(json["loadDistanceVertical"].get_uint64());
    validationLayersEnabled = json["validationLayersEnabled"].get_bool();
}
catch (const simdjson::simdjson_error& e) {
    GlobalLog.Write("Failed to load settings:");
    GlobalLog.Write(e.what());

    throw std::runtime_error("Failed to load settings");
}



uint32_t Settings::getLoadDistanceHorizontal() const { return loadDistanceHorizontal; }
uint32_t Settings::getLoadDistanceVertical() const { return loadDistanceVertical; }
bool Settings::getValidationLayersEnabled() const { return validationLayersEnabled; }
