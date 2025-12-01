#pragma once
#include <unordered_map>

#include "RenderTarget.h"
#include "Mesh/MeshChunk.h"



class ChunkRenderer {
private:
    VkDevice device{};

    VkPipelineLayout pipelineLayout{};
    VkPipeline pipelineOpaque{};
    VkPipeline pipelineTested{};
    VkPipeline pipelineBlended{};

private:
    ChunkRenderer() = default;

    void createLayout(VkDescriptorSetLayout setLayout);
    void createPipelines(const RenderTarget& renderTarget);
    
public:
    ChunkRenderer(
        VkDevice _device,
        const RenderTarget& renderTarget,
        VkDescriptorSetLayout setLayout
    );
    ~ChunkRenderer();

    void draw(
        VkCommandBuffer commandBuffer,
        const glm::mat4& matrixProjectionView,
        ChunkPos playerChunkPos,
        const std::unordered_map<ChunkPos, std::unique_ptr<MeshChunk>>& chunkMeshes
    );

    ChunkRenderer(ChunkRenderer&&) = delete;
    ChunkRenderer(const ChunkRenderer&) = delete;
    ChunkRenderer operator=(ChunkRenderer&&) = delete;
    ChunkRenderer operator=(const ChunkRenderer&) = delete;

    VkPipelineLayout getLayout() const;
};

