#include "ChunkRenderer.h"

#include <fstream>
#include <stdexcept>

#include <glm/gtc/matrix_transform.hpp>



namespace {

// Reads an entire binary file into a vector of char
std::vector<char> readBinaryFile(std::string& filepath) {
    std::ifstream file(filepath, std::ios::ate | std::ios::binary);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open shader file");
    }

    size_t fileSize = static_cast<size_t>(file.tellg());
    std::vector<char> buffer(fileSize);

    file.seekg(0);
    file.read(buffer.data(), fileSize);
    file.close();

    return buffer;
}



VkShaderModule createShaderModule(VkDevice device, std::string filepath) {
    std::vector<char> shaderCode = readBinaryFile(filepath);

    VkShaderModuleCreateInfo createInfo{
        .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
        .pNext{},
        .flags{},
        .codeSize = shaderCode.size(),
        .pCode = reinterpret_cast<const uint32_t*>(shaderCode.data())
    };

    VkShaderModule shader;
    if (vkCreateShaderModule(device, &createInfo, nullptr, &shader) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create shader module");
    }
    return shader;
}



VkPipeline createPipeline(
    VkDevice device,
    const RenderTarget& renderTarget,
    VkPipelineLayout layout,
    const char* shaderPath,
    VkPipelineColorBlendAttachmentState& blendAttachmentInfo
) {
    VkFormat swapchainFormat = renderTarget.getColourFormat();
    VkPipelineRenderingCreateInfo renderingInfo{
        .sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO,
        .pNext{},
        .viewMask{},
        .colorAttachmentCount = 1,
        .pColorAttachmentFormats = &swapchainFormat,
        .depthAttachmentFormat = renderTarget.getDepthFormat(),
        .stencilAttachmentFormat{}
    };

    VkShaderModule shaderModule = createShaderModule(device, shaderPath);
    std::array<VkPipelineShaderStageCreateInfo, 2> shaderStageInfos{
        VkPipelineShaderStageCreateInfo{
            .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
            .pNext{},
            .flags{},
            .stage = VK_SHADER_STAGE_VERTEX_BIT,
            .module = shaderModule,
            .pName = "vertMain",
            .pSpecializationInfo{}
        },
        VkPipelineShaderStageCreateInfo{
            .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
            .pNext{},
            .flags{},
            .stage = VK_SHADER_STAGE_FRAGMENT_BIT,
            .module = shaderModule,
            .pName = "fragMain",
            .pSpecializationInfo{}
        }
    };

    auto bindingDescriptions = Vertex::getBindingDescriptions();
    auto attributeDescriptions = Vertex::getAttributeDescriptions();
    VkPipelineVertexInputStateCreateInfo vertexInputInfo{
        .sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
        .pNext{},
        .flags{},
        .vertexBindingDescriptionCount = static_cast<uint32_t>(bindingDescriptions.size()),
        .pVertexBindingDescriptions = bindingDescriptions.data(),
        .vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size()),
        .pVertexAttributeDescriptions = attributeDescriptions.data()
    };

    VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo{
        .sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
        .pNext{},
        .flags{},
        .topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
        .primitiveRestartEnable{}
    };

    // Viewport and scissor specify counts with null pointers because we are using dynamic states for both
    VkPipelineViewportStateCreateInfo viewportStateInfo{
        .sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
        .pNext{},
        .flags{},
        .viewportCount = 1,
        .pViewports{},
        .scissorCount = 1,
        .pScissors{}
    };

    VkPipelineRasterizationStateCreateInfo rasterizationStateInfo{
        .sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
        .pNext{},
        .flags{},
        .depthClampEnable{},
        .rasterizerDiscardEnable{},
        .polygonMode = VK_POLYGON_MODE_FILL,
        .cullMode = VK_CULL_MODE_NONE,
        .frontFace = VK_FRONT_FACE_CLOCKWISE,
        .depthBiasEnable{},
        .depthBiasConstantFactor{},
        .depthBiasClamp{},
        .depthBiasSlopeFactor{},
        .lineWidth = 1.0f
    };

    VkPipelineMultisampleStateCreateInfo multisampleStateInfo{
        .sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
        .pNext{},
        .flags{},
        .rasterizationSamples = VK_SAMPLE_COUNT_1_BIT,
        .sampleShadingEnable{},
        .minSampleShading{},
        .pSampleMask{},
        .alphaToCoverageEnable{},
        .alphaToOneEnable{}
    };
    
    VkPipelineDepthStencilStateCreateInfo depthStencilInfo{
        .sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,
        .pNext{},
        .flags{},
        .depthTestEnable = true,
        .depthWriteEnable = true,
        .depthCompareOp = VK_COMPARE_OP_LESS,
        .depthBoundsTestEnable{},
        .stencilTestEnable{},
        .front{},
        .back{},
        .minDepthBounds{},
        .maxDepthBounds{}
    };

    VkPipelineColorBlendStateCreateInfo colourBlendStateInfo{
        .sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
        .pNext{},
        .flags{},
        .logicOpEnable{},
        .logicOp{},
        .attachmentCount = 1,
        .pAttachments = &blendAttachmentInfo,
        .blendConstants{}
    };

    std::array dynamicStates{
        VK_DYNAMIC_STATE_SCISSOR,
        VK_DYNAMIC_STATE_VIEWPORT
    };
    VkPipelineDynamicStateCreateInfo dynamicStateInfo{
        .sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
        .pNext{},
        .flags{},
        .dynamicStateCount = static_cast<uint32_t>(dynamicStates.size()),
        .pDynamicStates = dynamicStates.data()
    };

    VkGraphicsPipelineCreateInfo createInfo{
        .sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
        .pNext = &renderingInfo,
        .flags{},
        .stageCount = static_cast<uint32_t>(shaderStageInfos.size()),
        .pStages = shaderStageInfos.data(),
        .pVertexInputState = &vertexInputInfo,
        .pInputAssemblyState = &inputAssemblyInfo,
        .pTessellationState{},
        .pViewportState = &viewportStateInfo,
        .pRasterizationState = &rasterizationStateInfo,
        .pMultisampleState = &multisampleStateInfo,
        .pDepthStencilState = &depthStencilInfo,
        .pColorBlendState = &colourBlendStateInfo,
        .pDynamicState = &dynamicStateInfo,
        .layout = layout,
        .renderPass{},
        .subpass{},
        .basePipelineHandle{},
        .basePipelineIndex{}
    };
    
    VkPipeline pipeline;
    VkResult result = vkCreateGraphicsPipelines(device, {}, 1, &createInfo, nullptr, &pipeline);
    // Destroy the shader module regardless of whether the pipeline was successfully created
    vkDestroyShaderModule(device, shaderModule, nullptr);
    if (result != VK_SUCCESS) {
        throw std::runtime_error("Failed to create graphics pipeline");
    }
    return pipeline;
}

}



void ChunkRenderer::createPipelines(const RenderTarget& renderTarget) {
    VkPipelineColorBlendAttachmentState colourBlendNone{
        .blendEnable{},
        .srcColorBlendFactor{},
        .dstColorBlendFactor{},
        .colorBlendOp{},
        .srcAlphaBlendFactor{},
        .dstAlphaBlendFactor{},
        .alphaBlendOp{},
        .colorWriteMask =
            VK_COLOR_COMPONENT_R_BIT |
            VK_COLOR_COMPONENT_G_BIT |
            VK_COLOR_COMPONENT_B_BIT |
            VK_COLOR_COMPONENT_A_BIT
    };
    VkPipelineColorBlendAttachmentState colourBlendAlpha{
        .blendEnable = true,
        .srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA,
        .dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
        .colorBlendOp = VK_BLEND_OP_ADD,
        .srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE,
        .dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO,
        .alphaBlendOp = VK_BLEND_OP_ADD,
        .colorWriteMask =
            VK_COLOR_COMPONENT_R_BIT |
            VK_COLOR_COMPONENT_G_BIT |
            VK_COLOR_COMPONENT_B_BIT |
            VK_COLOR_COMPONENT_A_BIT
    };

    pipelineOpaque = createPipeline(
        device,
        renderTarget,
        pipelineLayout,
        "res/shaders/chunk_opaque.spv",
        colourBlendNone
    );
    pipelineTested = createPipeline(
        device,
        renderTarget,
        pipelineLayout,
        "res/shaders/chunk_tested.spv",
        colourBlendNone
    );
    pipelineBlended = createPipeline(
        device,
        renderTarget,
        pipelineLayout,
        "res/shaders/chunk_blended.spv",
        colourBlendAlpha
    );
}


void ChunkRenderer::createLayout(VkDescriptorSetLayout setLayout) {
    VkPushConstantRange pushConstantRange{
        .stageFlags = VK_SHADER_STAGE_VERTEX_BIT,
        .offset = 0,
        .size = 64
    };

    VkPipelineLayoutCreateInfo createInfo{
        .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
        .pNext{},
        .flags{},
        .setLayoutCount = 1,
        .pSetLayouts = &setLayout,
        .pushConstantRangeCount = 1,
        .pPushConstantRanges = &pushConstantRange
    };
    if (vkCreatePipelineLayout(device, &createInfo, nullptr, &pipelineLayout) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create pipeline layout");
    }
}



ChunkRenderer::ChunkRenderer(
    VkDevice _device,
    const RenderTarget& renderTarget,
    VkDescriptorSetLayout setLayout
) : ChunkRenderer() {
    device = _device;

    createLayout(setLayout);
    createPipelines(renderTarget);
}



ChunkRenderer::~ChunkRenderer() {
    vkDestroyPipeline(device, pipelineBlended, nullptr);
    vkDestroyPipeline(device, pipelineTested, nullptr);
    vkDestroyPipeline(device, pipelineOpaque, nullptr);
    vkDestroyPipelineLayout(device, pipelineLayout, nullptr);
}



void ChunkRenderer::draw(
    VkCommandBuffer commandBuffer,
    const glm::mat4& matrixProjectionView,
    ChunkPos playerChunkPos,
    const std::unordered_map<ChunkPos, std::unique_ptr<MeshChunk>>& chunkMeshes
) {
    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineOpaque);
    for (const auto& [pos, mesh] : chunkMeshes) {
        mesh->drawOpaque(
            commandBuffer,
            pipelineLayout,
            matrixProjectionView,
            playerChunkPos
        );
    }
    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineTested);
    for (const auto& [pos, mesh] : chunkMeshes) {
        mesh->drawTested(
            commandBuffer,
            pipelineLayout,
            matrixProjectionView,
            playerChunkPos
        );
    }
    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineBlended);
    for (const auto& [pos, mesh] : chunkMeshes) {
        mesh->drawBlended(
            commandBuffer,
            pipelineLayout,
            matrixProjectionView,
            playerChunkPos
        );
    }
}



VkPipelineLayout ChunkRenderer::getLayout() const { return pipelineLayout; }
