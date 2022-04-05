#pragma once
#include "NoiseSource.h"
class GeneratorChunkParameters;



// A config object that stores all the required noise generators for chunk generation
class GeneratorChunkNoise
{
public:
	GeneratorChunkNoise(
		int seed,
		float freqHeight,
		float freqTemperature,
		float freqRainfall,
		const char* settingHeight,
		const char* settingTemperature,
		const char* settingRainfall,
		const char* settingBiomeEdge
	) : noiseHeight(settingHeight, freqHeight, seed),
		noiseTemperature(settingTemperature, freqTemperature, seed + 0x57),
		noiseRainfall(settingRainfall, freqRainfall, seed + 0x63),
		noiseBiomeEdge(settingBiomeEdge, 1.0, seed)
	{}

private:
	NoiseSource2D noiseHeight;
	NoiseSource2D noiseTemperature;
	NoiseSource2D noiseRainfall;
	NoiseSource2D noiseBiomeEdge;

	friend GeneratorChunkParameters;
};