#pragma once



// Chunk constants
constexpr int CHUNK_SIZE = 32;
constexpr int CHUNK_AREA = CHUNK_SIZE * CHUNK_SIZE;
constexpr int CHUNK_VOLUME = CHUNK_AREA * CHUNK_SIZE;

// Loading constants, should probably be settings no constants but oh well
constexpr int LOAD_DISTANCE = 25;
constexpr int LOAD_DISTANCE_VERTICAL = 7;

// World size constants
constexpr int WORLD_RADIUS_CHUNK = 128;
constexpr int WORLD_RADIUS_BLOCK = WORLD_RADIUS_CHUNK * CHUNK_SIZE;
constexpr int WORLD_DIAMETER_CHUNK = WORLD_RADIUS_CHUNK * 2;
constexpr int WORLD_DIAMETER_BLOCK = WORLD_RADIUS_BLOCK * 2;