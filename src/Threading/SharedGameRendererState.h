#pragma once
#include <atomic>
#include <chrono>
#include <memory>

#include "ThreadQueue.h"
#include "../Rendering/Mesh/MeshChunk.h"
#include "../World/Entities/EntityPosition.h"
class ChunkPos;



struct SharedGameRendererState {
    std::atomic_uint64_t currentTick;
    std::chrono::steady_clock::time_point nextTickTimestamp;

    std::shared_ptr<ThreadQueue<std::unique_ptr<MeshChunk::Data>>> chunkMeshQueue;
	std::shared_ptr<ThreadQueue<ChunkPos>> chunkMeshQueueDeletion;

    std::atomic<EntityPosition> playerPosition;

    SharedGameRendererState();
};
