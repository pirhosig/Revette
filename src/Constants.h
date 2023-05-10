#pragma once



// Chunk constants
constexpr int CHUNK_SIZE = 32;
constexpr int CHUNK_AREA = CHUNK_SIZE * CHUNK_SIZE;
constexpr int CHUNK_VOLUME = CHUNK_AREA * CHUNK_SIZE;

// Loading constants, should probably be settings not constants but oh well
constexpr int LOAD_DISTANCE = 20;
constexpr int LOAD_DISTANCE_VERTICAL = 7;

// World size constants
constexpr int WORLD_RADIUS_CHUNK = 512;
constexpr int WORLD_RADIUS_BLOCK = WORLD_RADIUS_CHUNK * CHUNK_SIZE;
constexpr int WORLD_DIAMETER_CHUNK = WORLD_RADIUS_CHUNK * 2;
constexpr int WORLD_DIAMETER_BLOCK = WORLD_RADIUS_BLOCK * 2;