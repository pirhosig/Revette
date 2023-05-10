#include "ChunkPRNG.h"



inline uint32_t basicHash(uint32_t x)
{
	x ^= x >> 16;
	x *= 0x7feb352dU;
	x ^= x >> 15;
	x *= 0x846ca68bU;
	x ^= x >> 16;
	return x;
}



uint32_t positionHash(ChunkPos chunkPos)
{
	auto hashX = basicHash(static_cast<uint32_t>(chunkPos.x));
	auto hashY = basicHash(static_cast<uint32_t>(chunkPos.y));
	auto hashZ = basicHash(static_cast<uint32_t>(chunkPos.z));
	return (hashX << 11) ^ (hashX >> 21) ^ (hashY << 4) ^ (hashY >> 28) ^ hashZ;
}



ChunkPRNG::ChunkPRNG(ChunkPos pos) : lcg(positionHash(pos)) {}



double ChunkPRNG::next()
{
	return raw() / 65535.0;
}



int ChunkPRNG::scaledInt(double a, double b)
{
	return static_cast<int>(next() * a + b);
}



uint16_t ChunkPRNG::raw()
{
	return static_cast<uint16_t>(lcg());
}
