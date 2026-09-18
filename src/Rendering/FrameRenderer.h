#pragma once
#include <queue>
#include <vector>

#include "ChunkRenderer.h"
#include "Fence.h"
#include "GuiRenderer.h"
#include "LinearBufferSuballocator.h"
#include "RenderResources.h"
#include "RenderTarget.h"
#include "SingleCommandBuffer.h"
#include "Core/RevetteCore.h"



class FrameRenderer {
private:
    VkDevice device;
    VkQueue queue;
    VmaAllocator allocator;
    RenderTarget& renderTarget;
    RenderResources& renderResources;
    ChunkRenderer& chunkRenderer;
    GuiRenderer& guiRenderer;

    LinearBufferSuballocator stagingBuffer;
    SingleCommandBuffer commandBuffer;
    Fence fenceBegin;

    VkSemaphore semaphoreImageAvailable{};
    VkSemaphore semaphorePresent{};

    std::queue<std::unique_ptr<MeshChunk>> meshDeletionQueue;

private:
    FrameRenderer(
        VkDevice _device,
        RenderTarget& _renderTarget,
        RenderResources& _renderResources,
        ChunkRenderer& _chunkRenderer,
        GuiRenderer& _guiRenderer,
        u32 queueFamilyIndex,
        VmaAllocator allocator
    );

    u32 beginFrame(
        std::queue<std::unique_ptr<MeshChunk::Data>> loadMeshes,
        std::unordered_map<ChunkPos, std::unique_ptr<MeshChunk>>& chunkMeshes
    );
    void uploadMeshes(
        std::vector<VkBufferMemoryBarrier2>& bufferBarriers,
        std::queue<std::unique_ptr<MeshChunk::Data>> loadMeshes,
        std::unordered_map<ChunkPos, std::unique_ptr<MeshChunk>>& chunkMeshes
    );
    void drawChunks(
        EntityPosition playerPosition,
        std::unordered_map<ChunkPos, std::unique_ptr<MeshChunk>>& chunkMeshes
    );
    void endFrame(u32 imageIndex);

public:
    FrameRenderer(
        VkDevice _device,
        VkQueue _queue,
        RenderTarget& _renderTarget,
        RenderResources& _renderResources,
        ChunkRenderer& _chunkRenderer,
        GuiRenderer& _guiRenderer,
        u32 queueFamilyIndex,
        VmaAllocator allocator
    );
    ~FrameRenderer();

    FrameRenderer(FrameRenderer&&);
    
    // I hate this language
    FrameRenderer(const FrameRenderer&) = delete;
    FrameRenderer operator=(FrameRenderer&&) = delete;
    FrameRenderer operator=(const FrameRenderer&) = delete;

    void drawFrame(
        std::queue<std::unique_ptr<MeshChunk::Data>> loadMeshes,
        EntityPosition playerPosition,
        std::unordered_map<ChunkPos, std::unique_ptr<MeshChunk>>& chunkMeshes
    );
    void queueMeshForDeletion(std::unique_ptr<MeshChunk> mesh);
};

