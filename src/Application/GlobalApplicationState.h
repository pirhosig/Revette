#pragma once
#include <atomic>
#include <chrono>
#include <memory>
#include <mutex>
#include <new>

#include "Core/RevetteCore.h"
#include "Rendering/Mesh/MeshChunk.h"
#include "Util/ConcurrentQueue.h"



struct GlobalApplicationState {
    alignas(CACHE_ALIGNMENT)
        std::atomic_bool applicationShouldTerminate;

    alignas(CACHE_ALIGNMENT)
        std::atomic<class ChunkPos> playerChunkPosition;

    alignas(CACHE_ALIGNMENT)
        std::atomic_uint64_t currentGameClockTick;

    alignas(CACHE_ALIGNMENT)
        ConcurrentQueue<std::unique_ptr<MeshChunk::Data>> rendererMeshIngestQueue;

    alignas(CACHE_ALIGNMENT)
        std::atomic<class EntityPosition> playerPosition;


public:
    explicit GlobalApplicationState();
};
