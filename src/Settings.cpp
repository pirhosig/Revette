#include "Settings.h"
#include <stdexcept>
#include <simdjson.h>
#include "GlobalLog.h"



Settings::Settings() try {
    simdjson::padded_string jsonString = simdjson::padded_string::load("res/settings.json");
    simdjson::dom::parser parser;
    auto json = parser.parse(jsonString);

    loadDistanceHorizontal = static_cast<i32>(json["loadDistanceHorizontal"].get_uint64());
    loadDistanceVertical = static_cast<i32>(json["loadDistanceVertical"].get_uint64());
    generationSeed = static_cast<i32>(json["generationSeed"].get_int64());

    validationLayersEnabled = json["validationLayersEnabled"].get_bool();
}
catch (const simdjson::simdjson_error& e) {
    GlobalLog.Write("Failed to load settings:");
    GlobalLog.Write(e.what());

    throw std::runtime_error("Failed to load settings");
}



i32 Settings::getLoadDistanceHorizontal() const { return loadDistanceHorizontal; }
i32 Settings::getLoadDistanceVertical() const { return loadDistanceVertical; }
i32 Settings::getGenerationSeed() const { return generationSeed; }

bool Settings::getValidationLayersEnabled() const { return validationLayersEnabled; }
