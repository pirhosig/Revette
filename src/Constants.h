#pragma once



// Chunk constants
constexpr int CHUNK_SIZE = 32;
constexpr int CHUNK_AREA = CHUNK_SIZE * CHUNK_SIZE;
constexpr int CHUNK_VOLUME = CHUNK_AREA * CHUNK_SIZE;

constexpr int LOAD_DISTANCE = 25;
constexpr int LOAD_DISTANCE_VERTICAL = 5;

constexpr int WORLD_RADIUS = (LOAD_DISTANCE) * CHUNK_SIZE;