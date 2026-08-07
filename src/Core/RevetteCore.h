#pragma once
#include <cstdint>

// Standard integer type aliases
using i8  = int8_t;
using i16 = int16_t;
using i32 = int32_t;
using i64 = int64_t;

using u8  = uint8_t;
using u16 = uint16_t;
using u32 = uint32_t;
using u64 = uint64_t;


/*
Constants which are widely used, and are unlikely to ever change. Constants which are only applicable to a certain
section of code should not be stored here.
*/

// Chunk constants
constexpr i32 CHUNK_SIZE_LOG = 5;
constexpr i32 CHUNK_SIZE = 1 << CHUNK_SIZE_LOG;
constexpr i32 CHUNK_VOLUME = 1 << (3 * CHUNK_SIZE_LOG);
constexpr i32 CHUNK_AREA = 1 << (2 * CHUNK_SIZE_LOG);

constexpr float CHUNK_SIZE_F = static_cast<float>(CHUNK_SIZE);
constexpr double CHUNK_SIZE_D = static_cast<double>(CHUNK_SIZE);

// World size constants.
// TODO: change world size to be a runtime constant.
constexpr i32 WORLD_RADIUS_CHUNK_LOG = 9;
constexpr i32 WORLD_RADIUS_CHUNK = 1 << WORLD_RADIUS_CHUNK_LOG;
constexpr i32 WORLD_RADIUS_BLOCK_LOG = WORLD_RADIUS_CHUNK_LOG + CHUNK_SIZE_LOG;
constexpr i32 WORLD_RADIUS_BLOCK = 1 << WORLD_RADIUS_BLOCK_LOG;
constexpr i32 WORLD_DIAMETER_BLOCK = 2 * WORLD_RADIUS_BLOCK;

constexpr float WORLD_RADIUS_BLOCK_F = static_cast<float>(WORLD_RADIUS_BLOCK);

// TODO: remove this from global constants.
constexpr int SEA_LEVEL = 64;
