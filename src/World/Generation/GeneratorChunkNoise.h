#pragma once
#include "../../GlobalLog.h"
#include "NoiseSource.h"
class GeneratorChunkParameters;



// A config object that stores all the required noise generators for chunk generation
class GeneratorChunkNoise
{
public:
	GeneratorChunkNoise(
		int seed,
		const char* settingHeight,
		const char* settingTemperature,
		const char* settingRainfall
	) : noiseHeight(settingHeight, seed),
		noiseTemperature(settingTemperature, seed + 0x57),
		noiseRainfall(settingRainfall, seed + 0x63)
	{}

private:
	NoiseSource2D noiseHeight;
	NoiseSource2D noiseTemperature;
	NoiseSource2D noiseRainfall;

	friend GeneratorChunkParameters;
};