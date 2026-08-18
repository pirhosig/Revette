#pragma once
#include <atomic>
#include <chrono>
#include <memory>
#include <mutex>
#include <new>

#include "Rendering/Mesh/MeshChunk.h"
#include "Util/ConcurrentQueue.h"



struct GlobalApplicationState {
    alignas(std::hardware_destructive_interference_size)
        std::atomic_bool applicationShouldTerminate;

    alignas(std::hardware_destructive_interference_size)
        std::atomic<class ChunkPos> playerChunkPosition;

    alignas(std::hardware_destructive_interference_size)
    std::atomic_uint64_t currentGameClockTick;

    alignas(std::hardware_destructive_interference_size)
        ConcurrentQueue<std::unique_ptr<MeshChunk::Data>> rendererMeshIngestQueue;

    alignas(std::hardware_destructive_interference_size)
        std::atomic<class EntityPosition> playerPosition;


public:
    explicit GlobalApplicationState();
};
